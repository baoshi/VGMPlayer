#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <crc.h>    // From sdcard module for CRC16
#include "my_mem.h"
#include "path_utils.h"
#include "lister.h"


static FRESULT _fr;


static bool _get_unsorted_catalog_name(const char* path, char* out, int len)
{
    return path_concatenate(path, RAW_CATALOG_FILE, out, len, false);
}


static bool _get_catalog_name(const char* path, char* out, int len)
{
    return path_concatenate(path, CATALOG_FILE, out, len, false);
}


static bool _get_catalog_chunk_name(const char* path, int index, char* out, int len)
{
    char chunk[FF_LFN_BUF + 1];
    snprintf(chunk, FF_LFN_BUF + 1, CHUNK_FILE_PATTERN, index);
    return path_concatenate(path, chunk, out, len, false);
}


static bool _match_extension(const char* file, const char * const patterns[])
{
    bool r = false;
    char ext[FF_LFN_BUF + 1];

    if (0 == patterns)
    {
        // if no extension pattern specified, treat as matched
        r = true;
    }
    else if (path_get_ext(file, ext, FF_LFN_BUF + 1))
    {
        int i = 0;
        int len = strlen(ext);
        while (patterns[i] && patterns[i][0])
        {
            if (strncasecmp(ext, patterns[i], len) == 0)
            {
                r = true;
                break;
            }
            ++i;
        }
    }
    return r;
}


static FRESULT _calculate_checksum(const char* path, const char * const patterns[], int max_count, unsigned short *checksum, int* count)
{
    DIR dir = { 0 };
    FILINFO fno = { 0 };
    unsigned short sum = 0;
    int c = 0;
    _fr = f_opendir(&dir, path);
    if (FR_OK != _fr) 
    {
        return _fr;
    }
    for ( ; c < max_count; )
    {
        _fr = f_readdir(&dir, &fno);
        if (_fr != FR_OK || fno.fname[0] == 0) break;
        if (fno.fname[0] == '.') continue;  // skip all files starting with "."
        if ((fno.fattrib & AM_HID) || (fno.fattrib & AM_SYS)) continue; // skip hidden or system files/directories
        if (fno.fattrib & AM_DIR)
        {
            // directory, if not starting with ".", will always be included
            update_crc16(&sum, fno.fname, strlen(fno.fname));
        }
        else
        {
            if (!_match_extension(fno.fname, patterns)) continue; // only include know extension.
            update_crc16(&sum, fno.fname, strlen(fno.fname));
        }
        ++c;
    }
    f_closedir(&dir);
    *checksum = sum;
    *count = c;
    return _fr;
}


static int _create_unsorted_catalog(const char *path, const char *cname, const char * const patterns[], int max_count)
{
    int r = LS_OK;
    FIL fd;
    DIR dir = { 0 };
    FILINFO fno = { 0 };
    UINT wt;
    int c = 0;
    do
    {
        _fr = f_open(&fd, cname, FA_CREATE_ALWAYS | FA_WRITE);
        if (_fr != FR_OK) 
        {
            r = LS_ERR_FATFS;
            break;
        }
        // open and interate dir
        _fr = f_opendir(&dir, path);
        if (_fr != FR_OK) 
        {
            r = LS_ERR_FATFS;
            break;
        }
        for (; (FR_OK == _fr) && (c < max_count); )
        {
            _fr = f_readdir(&dir, &fno);
            if (_fr != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;  // skip all files starting with "."
            if ((fno.fattrib & AM_HID) || (fno.fattrib & AM_SYS)) continue; // skip hidden or system files/directories
            if (fno.fattrib & AM_DIR)
            {
                ++c;
                // output filename with "!" in front
                _fr = f_write(&fd, "!", 1, &wt);
                _fr = f_write(&fd, fno.fname, strlen(fno.fname), &wt);
                _fr = f_write(&fd, "\n", 1, &wt);
            }
            else
            {
                ++c;
                if (!_match_extension(fno.fname, patterns)) continue; // only output know extension
                _fr = f_write(&fd, fno.fname, strlen(fno.fname), &wt);
                _fr = f_write(&fd, "\n", 1, &wt);
            }
        }
        f_closedir(&dir);
        f_close(&fd);
    } while (0);
    // conclusion
    if (_fr != FR_OK)
    {
        r = LS_ERR_FATFS;
    }
    if (r != LS_OK)
    {
        // if any error occurred, delete unsorted catalog file
        f_unlink(cname);
    }
    return r;
}


static int _compare_strings(const void* p, const void* q)
{
    const char* str1 = *(const char**)p;
    const char* str2 = *(const char**)q;
    return strncasecmp(str1, str2, FF_LFN_BUF + 1);
}


static int _partition_unsorted_catalog(const char *uncat, const char *work_dir, int chunk_lines, int *chunk_count)
{
    int r = LS_OK;
    char chunk_name[FF_LFN_BUF + 1];
    int count = 0;
    char *pool = 0;
    FIL in = { 0 };
    UINT wt;
    do
    {
        int d1 = chunk_lines * sizeof(char *);      // pointer to strings
        int d2 = chunk_lines * (FF_LFN_BUF + 1);    // string storage pool
        pool = MY_MALLOC(d1 + d2);
        if (0 == pool)
        {
            r = LS_ERR_MEMORY;
            break;
        }
        char ** strings = (char **)pool;
        // assign string pointers
        for (int i = 0; i < d1; ++i) strings[i] = pool + d1 + i * (FF_LFN_BUF + 1);
        // open unsorted catalog
        _fr = f_open(&in, uncat, FA_OPEN_EXISTING | FA_READ);
        if (_fr != FR_OK)
        {
            r = LS_ERR_FATFS;
            break;
        }
        for (;;)
        {
            int lines = 0;
            for (; lines < chunk_lines; ++lines)
            {
                if (0 == f_gets(strings[lines], FF_LFN_BUF + 1, &in)) break;
            }
            if (lines <= 0)
                break;
            // sort strings
            qsort(strings, lines, sizeof(char *), _compare_strings);
            // save chunk
            _get_catalog_chunk_name(work_dir, count, chunk_name, FF_LFN_BUF + 1);    // wont fail
            FIL c;
            _fr = f_open(&c, chunk_name, FA_CREATE_ALWAYS | FA_WRITE);
            if (FR_OK == _fr)
            {
                for (int i = 0; (i < lines) && (FR_OK == _fr); ++i)
                    _fr = f_write(&c, strings[i],  strlen(strings[i]), &wt);
                f_close(&c);
                ++count;
            }
            else
            {
                r = LS_ERR_FATFS;
                break;
            }
        }
        *chunk_count = count;
    } while (0);

    f_close(&in);
    if (pool != 0)
        MY_FREE(pool);
    return r;
}


typedef struct chunk_info_s
{
    FIL fd;
    bool eof;
    char line[FF_LFN_BUF + 1];
} chunk_info_t;


int _open_chunk(const char *chunk_name, chunk_info_t *ci)
{
    int r = LS_OK;
    _fr = f_open(&(ci->fd), chunk_name, FA_OPEN_EXISTING | FA_READ);
    if (_fr != FR_OK)
    {
        r = LS_ERR_FATFS;
    }
    else
    {
        ci->line[0] = '\0';
        ci->eof = false;
    }
    return r;
}


static void _close_chunk(chunk_info_t *ci)
{
    if (ci)
        f_close(&(ci->fd));
}


static const char* _fetch_chunk_line(chunk_info_t *ci)
{
    const char* r;

    if (ci->line[0])
    {
        r = ci->line;
    }
    else if (ci->eof)
    {
        r = 0;
    }
    else
    {
        if (0 == f_gets(ci->line, FF_LFN_BUF + 1, &(ci->fd)))
        {
            ci->eof = true;
            r = 0;
        }
        else
        {
            r = ci->line;
        }
    }
    return r;
}


static void _clean_chunk_line(chunk_info_t *ci)
{
    ci->line[0] = '\0';
}


static int _merge_catalog_chunks(lister_t* lister, int chunk_count)
{
    int r = LS_OK;
    char cat_name[FF_LFN_BUF + 1] = { 0 };
    char chunk_name[FF_LFN_BUF + 1];
    chunk_info_t* chunks = 0;
    int select;
    UINT wt;
    do
    {
        if (!_get_catalog_name(lister->dir, cat_name, FF_LFN_BUF + 1))
        {
            r = LS_ERR_PATH_TOO_DEEP;
            break;
        }
        _fr = f_open(&(lister->fd), cat_name, FA_CREATE_ALWAYS | FA_WRITE);
        if (_fr != FR_OK)
        {
            r = LS_ERR_FATFS;
            break;
        }
        chunks = MY_CALLOC(chunk_count, sizeof(chunk_info_t));
        if (0 == chunks)
        {
            r = LS_ERR_MEMORY;
            break;
        }
        // open all chunks
        for (int i = 0; i < chunk_count; ++i)
        {
            _get_catalog_chunk_name(lister->dir, i, chunk_name, FF_LFN_BUF + 1);
            r = _open_chunk(chunk_name, &(chunks[i]));
            if (r != LS_OK) break;
        }
        // catalog file header
        f_printf(&(lister->fd), "%d\n%d\n", lister->checksum, lister->count);
        // set page 0 offset in case chunk_count is 0
        lister->page_offset[0] = (uint32_t)f_tell(&(lister->fd));
        int index = 0;
        int page = 0;
        for (;;)
        {
            bool alleofed = true;
            const char *min = 0, *str;
            int min_idx = 0;
            for (int i = 0; i < chunk_count; ++i)
            {
                str = _fetch_chunk_line(&(chunks[i]));
                if (str == 0) continue;
                alleofed = false;
                // assume first line is minimal, then compare with other lines
                if (min == 0)
                {
                    min = str;
                    min_idx = i;
                }
                else
                {
                    if (strncasecmp(min, str, FF_LFN_BUF + 1) > 0)
                    {
                        min = str;
                        min_idx = i;
                    }
                }
            }
            if (alleofed)
                break;
            if (0 == (index % lister->page_size))
            {
                lister->page_offset[page] = (uint32_t)f_tell(&(lister->fd));
                if (page + 1 >= MAX_PAGES)
                    break;
                ++page;
            }
            ++index;
            _fr = f_write(&(lister->fd), min, strlen(min), &wt);
            if (_fr != FR_OK)
            {
                r = LS_ERR_FATFS;
                break;
            }
            _clean_chunk_line(&(chunks[min_idx]));
        }
        lister->pages = page;
        // reopen catalog file
        f_close(&(lister->fd));
        f_open(&(lister->fd), cat_name, FA_OPEN_EXISTING | FA_READ);
        // set cur_page and cur_index to invalid values so lister_move_to will move to new page
        lister->cur_page = -1;
        lister->cur_index = -1;
        r = lister_move_to(lister, 0, 0);
    } while (0);

    if (r != LS_OK)
    {
        // close catalog file object if failed
        f_close(&(lister->fd));
        if (cat_name[0])
            f_unlink(cat_name);
    }
    if (chunks)
    {
        for (int i = 0; i < chunk_count; ++i)
        {
            _close_chunk(&(chunks[i]));
        }
        MY_FREE(chunks);
    }
    return r;
}


static int _create_catalog(const char * const patterns[], lister_t *lister)
{
    int r = LS_OK;
    char ucat[FF_LFN_BUF + 1] = { 0 }; // unsorted catalog file
    int chunk_count = 0;
    do
    {
        // create unsorted catalog
        if (!_get_unsorted_catalog_name(lister->dir, ucat, FF_LFN_BUF + 1))
        {
            r = LS_ERR_PATH_TOO_DEEP;
            break;
        }
        r = _create_unsorted_catalog(lister->dir, ucat, patterns, lister->page_size * MAX_PAGES);
        if (r != LS_OK)
            break;
        // partition unsorted catalog into chunks
        r = _partition_unsorted_catalog(ucat, lister->dir, LINES_PER_CHUNK, &chunk_count);
        if (r != LS_OK) 
        {
            break;
        }
        r = _merge_catalog_chunks(lister, chunk_count);
        if (r != LS_OK) 
        {
            break;
        }
    } while (0);
    // delete unsorted catalog
    f_unlink(ucat);
    // delete chunks
    if (chunk_count == 0) chunk_count = MAX_PAGES;
    for (int i = 0; i < chunk_count; ++i)
    {
        // unsorted catalog file name is nolong being used, reuse here
        _get_catalog_chunk_name(lister->dir, i, ucat, FF_LFN_BUF + 1);    // wont fail
        f_unlink(ucat);
    }
    return r;    
}


// Open existing catalog file
// if safe_mode is true, compare checksum inside lister object with the catalog file, and retrurn LS_ERR_MISMATCH if mismatched
// if safe_mode is false, read checksum/count from catalog file
static int _open_catalog(lister_t *lister, bool safe_mode)
{
    int r = LS_OK;
    char cat[FF_LFN_BUF + 1];
    char temp[16];
    int val;

    do
    {
        if (!_get_catalog_name(lister->dir, cat, FF_LFN_BUF + 1))
        {
            r = LS_ERR_PATH_TOO_DEEP;
            break;
        }
        _fr = f_open(&(lister->fd), cat, FA_READ | FA_OPEN_EXISTING);
        if (FR_OK != _fr)
        {
            r = LS_ERR_FATFS;
            break;
        }
        if (safe_mode)
        {
            // 1st line is checksum
            f_gets(temp, 16, &(lister->fd));
            val = atoi(temp);
            if (lister->checksum != val)
            {
                r = LS_ERR_MISMATCH;
                break;
            }
            // 2nd line is entry count
            f_gets(temp, 16, &(lister->fd));
            val = atoi(temp);
            if (lister->count != val)
            {
                r = LS_ERR_MISMATCH;
                break;
            }
        }
        else
        {
            // 1st line is checksum
            f_gets(temp, 16, &(lister->fd));
            val = atoi(temp);
            lister->checksum = val;
            // 2nd line is entry count
            f_gets(temp, 16, &(lister->fd));
            val = atoi(temp);
            lister->count = val;
        }
        // read all lines and get the offset to each page
        int index = 0;
        int page = 0;
        while (1)
        {
            if (0 == (index % lister->page_size))
            {
                lister->page_offset[page] = (uint32_t)f_tell(&(lister->fd));
                if (page + 1 >= MAX_PAGES)
                    break;
                ++page;
            }
            if (f_gets(cat, FF_LFN_BUF + 1, &(lister->fd)))  // reuse cat as buffer
            {
                ++index;
            }
            else
            {
                // if we just added a new page, remove it because it is empty
                if (0 == (index % lister->page_size))
                {
                    lister->page_offset[page] = 0;
                    --page;
                }
                break;
            }
        }
        // other structure members
        lister->pages = page;
        lister->count = index;
        // set cur_page and cur_index to invalid values so lister_move_to will move to new page
        lister->cur_page = -1;
        lister->cur_index = -1;
        r = lister_move_to(lister, 0, 0);
    } while (0);
    
    return r;
}



// If safe_mode is false, existing directory catalog will be read.
// If safe_mode is true, existing directory catalog will be verified before read.
// If no existing catalog found, it will be created.
int lister_open_dir(const char *path, const char * const patterns[], int page_size, bool safe_mode, lister_t **lister)
{
    // 1. Allocate lister object
    // 2. Open directory and calculate checksum
    // 3. Open catalog file, If no valid catalog found then create one
    int r = LS_OK;
    lister_t* lst = 0;
    int count = 0;
    uint16_t checksum = 0;
    do
    {
        lst = MY_CALLOC(sizeof(lister_t), 1);
        if (0 == lst)
        {
            r = LS_ERR_MEMORY;
            break;
        }
        // calculate checksum for the directory (if safe_mode is true)
       if (safe_mode && (_calculate_checksum(path, patterns, page_size * MAX_PAGES, &checksum, &count) != FR_OK))
        {
            r = LS_ERR_FATFS;
            break;
        }
        lst->page_size = page_size;
        lst->count = count;
        lst->checksum = checksum;
        path_copy(path, lst->dir, FF_LFN_BUF + 1);
        // try open existing catalog
        r = _open_catalog(lst, safe_mode);
        if (r == LS_OK) // success
            break;
        // Reach here if no valid catalog found. If we havn't calculated checksum (safe_mode is off), we need to do now
        if (!safe_mode)
        { 
            if (_calculate_checksum(path, patterns, page_size * MAX_PAGES, &checksum, &count) != FR_OK)
            {
                r = LS_ERR_FATFS;
                break;
            }
            lst->count = count;
            lst->checksum = checksum;
        }
        r = _create_catalog(patterns, lst);
    } while (0);

    if (r != LS_OK) MY_FREE(lst);
    *lister = lst;

    return r;
}


int lister_close(lister_t *lister)
{
    if (lister)
    {
        f_close(&(lister->fd)); // file object is validated inside f_close
        MY_FREE(lister);
    }
    return LS_OK;
}


// move lister pointer to the specific page and index.
// the following lister_get_entry will retrieve the entry.
int lister_move_to(lister_t *lister, int page, int index)
{
    int r = LS_OK;
    char *p;
    do
    {
        // We need to rewind/move to another page in case:
        if ((page != lister->cur_page)                                  // a different page is requested
            ||
            (index < lister->cur_index)                                 // a previous entry is requested
            ||
            (index == lister->cur_index && lister->cache[0] != '\0')    // requesting entry at the cursor but this entry is already read
        )
        {
            _fr = f_lseek(&(lister->fd), (FSIZE_t)(lister->page_offset[page]));
            if (_fr != FR_OK)
            {
                r = LS_ERR_FATFS;
                break;
            }
            lister->cur_page = page;
            lister->cur_index = 0;
            lister->cache[0] = '\0';
        }
        // read all entries before the requested entry then discard.
        // note if cache is not empty, we will start reading from cur_index + 1.
        int start = ('\0' == lister->cache[0]) ? lister->cur_index : lister->cur_index + 1;
        for (; start < index; ++start)
        {
            if (0 == f_gets(lister->cache, FF_LFN_BUF + 3, &(lister->fd)))
            {
                r = LS_ERR_EOF;
                break;
            }
            lister->cache[0] = '\0';
        }
        // set index
        if (lister->cur_index != index)
        {
            lister->cur_index = index;
            lister->cache[0] = '\0';
        }
    } while (0);
    return r;    
}

// move curor to the next entry
// when at the end of a page:
//   in_page = true: return EOF; in_page = false: advance to the next page
// when at the end of the catalog:
//   wrap = true: wrap to the very beginning of the catalog; wrap = false: return EOF
int lister_move_next(lister_t *lister, bool in_page, bool wrap)
{
    int r = LS_OK;
    do
    {
        if ('\0' == lister->cache[0])
        {
            // cache is empty if we just opened catalog or called lister_select_page, do nothing
            // following fetch_entry will do actual reading
            break;
        }
        else
        {
            int i = lister->cur_index;
            int p = lister->cur_page;
            int last = (lister->count - 1) % lister->page_size;   // calculate index of last entry
            if ((p + 1 == lister->pages) && (i == last)) // if we are at the last entry of last page
            {
                if (!wrap)
                {
                    // not wrap operation, return eof
                    r = LS_ERR_EOF;
                    break;
                }
                else
                {
                    // wrap to beginning
                    r = lister_move_to(lister, 0, 0);
                    break;
                }
            }
            else if (i + 1 >= lister->page_size)    // if we are at the last entry of a middle page
            {
                if (in_page)
                {
                    // in_page operation, return eof
                    r = LS_ERR_EOF;
                    break;
                }
                else
                {
                    // advance to next page
                    r = lister_move_to(lister, p + 1, 0);
                    break;
                }
            }
            else
            {
                // no page adjustment, advance index
                r = lister_move_to(lister, p, i + 1);
                break;
            }
        }
    } while (0);
    return r;
}


// move cursor to the previous entry
// when at the beginning of a page:
//   in_page = true: return EOF; in_page = false: move up to the last entry of previous page
// when at the very beginning of the catalog:
//   wrap = true: wrap to the last entry; wrap = false: return EOF
int lister_move_prev(lister_t *lister, bool in_page, bool wrap)
{
    // vs. move_next: 
    // in move_next, if cache is empty, we will fetch cache instead of move forward
    // in move_prev, even if cache is empty. we will move backward
    int r = LS_OK;
    int i = lister->cur_index;
    int p = lister->cur_page;
    do
    {
        if (i <= 0) // if we are at the first entry of a page
        {
            if (in_page)
            {
                // in_page operation, return EOF
                r = LS_ERR_EOF;
                break;
            }
            else
            {
                // we need to go to previous page
                if (p == 0) // if we are at 1st page
                {
                    if (!wrap)
                    {
                        // !wrap operation, return eof
                        r = LS_ERR_EOF;
                        break;
                    }
                    else
                    {
                        // go to last page, last entry, fetch cache
                        p = lister->pages - 1;
                        i = (lister->count - 1) % lister->page_size;
                        r = lister_move_to(lister, p, i);
                        break;
                    }
                }
                else
                {
                    // go to the last entry of previous page
                    --p;
                    i = lister->page_size - 1;  // previous page will always be a full page, no need to check i
                    r = lister_move_to(lister, p, i);
                    break;
                }
            }
        }
        else
        {
            // no page adjustment, just move index
            r = lister_move_to(lister, p, i - 1);
            break;
        }
    } while (0);
    return r;
}


// get content of an entry
// read from cache if alraedy read
int lister_get_entry(lister_t *lister, char *out, int len, uint8_t *type)
{
    int r = LS_OK;
    do
    {
        // fetch cache if not already fetched
        if ('\0' == lister->cache[0])
        {
            if (0 == f_gets(lister->cache, FF_LFN_BUF + 3, &(lister->fd)))
            {
                r = LS_ERR_EOF;
                break;
            }
            path_trim_back(lister->cache);
        }
        if ('!' == lister->cache[0])
        {
            if (out != 0) path_copy(&(lister->cache[1]), out, len);
            *type = LS_TYPE_DIRECTORY;
        }
        else
        {
            if (out != 0) path_copy(lister->cache, out, len);
            *type = LS_TYPE_FILE;
        }
    } while (0);
    return r;
}


// get next entry
// convinient call to move_next and get_entry
int lister_get_next_entry(lister_t *lister, bool in_page, bool wrap, char *out, int len, uint8_t *type)
{
    int r;
    r = lister_move_next(lister, in_page, wrap);
    if (LS_OK == r)
    {
        r = lister_get_entry(lister, out, len, type);
    }
    return r;
}


// get previous entry
// convinient call to move_prev and get_entry
int lister_get_prev_entry(lister_t *lister, bool in_page, bool wrap, char *out, int len, uint8_t *type)
{
    int r;
    r = lister_move_prev(lister, in_page, wrap);
    if (LS_OK == r)
    {
        r = lister_get_entry(lister, out, len, type);
    }
    return r;
}


FRESULT lister_get_fatfs_error()
{
    return _fr;
}
