#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <crc.h>    // From sdcard module for CRC16
#include "my_mem.h"
#include "path_utils.h"
#include "catalog.h"


#define CATALOG_FILE            ".file.cat"
#define UNSORTED_CATALOG_FILE   ".temp.cat"
#define CATALOG_CHUNK_PATTERN   ".t%d.cat"


// ENTRIES_PER_CHUNK affects directory sorting speed and memory consumption
// Experiments: (455 files)
// ENTRIES_PER_CHUNK          Memory required         Catalog creation time
//      20                      5200                    21.05s
//      64                     16650                     8.79s
//     128                     33280                     5.49s
//     256                     66560                     3.85s

#define ENTRIES_PER_CHUNK     128


static FRESULT _fr;


static bool _get_unsorted_catalog_name(const char* path, char* out, int len)
{
    return path_concatenate(path, UNSORTED_CATALOG_FILE, out, len, false);
}


static bool _get_catalog_name(const char* path, char* out, int len)
{
    return path_concatenate(path, CATALOG_FILE, out, len, false);
}


static bool _get_catalog_chunk_name(const char* path, int index, char* out, int len)
{
    char chunk[FF_LFN_BUF + 1];
    snprintf(chunk, FF_LFN_BUF + 1, CATALOG_CHUNK_PATTERN, index);
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
    int r = CAT_OK;
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
            r = CAT_ERR_FATFS;
            break;
        }
        // open and interate dir
        _fr = f_opendir(&dir, path);
        if (_fr != FR_OK) 
        {
            r = CAT_ERR_FATFS;
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
        r = CAT_ERR_FATFS;
    }
    if (r != CAT_OK)
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


static int _partition_unsorted_catalog(const char *uncat, const char *work_dir, int chunk_entries, int *chunk_count)
{
    int r = CAT_OK;
    char chunk_name[FF_LFN_BUF + 1];
    int count = 0;
    char *pool = 0;
    FIL in = { 0 };
    UINT wt;
    do
    {
        int d1 = chunk_entries * sizeof(char *);      // pointer to strings
        int d2 = chunk_entries * (FF_LFN_BUF + 1);    // string storage pool
        pool = MY_MALLOC(d1 + d2);
        if (0 == pool)
        {
            r = CAT_ERR_MEMORY;
            break;
        }
        char ** strings = (char **)pool;
        // assign string pointers
        for (int i = 0; i < d1; ++i) strings[i] = pool + d1 + i * (FF_LFN_BUF + 1);
        // open unsorted catalog
        _fr = f_open(&in, uncat, FA_OPEN_EXISTING | FA_READ);
        if (_fr != FR_OK)
        {
            r = CAT_ERR_FATFS;
            break;
        }
        for (;;)
        {
            int lines = 0;
            for (; lines < chunk_entries; ++lines)
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
                r = CAT_ERR_FATFS;
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
    char entry[FF_LFN_BUF + 1];
} chunk_info_t;


int _open_chunk(const char *chunk_name, chunk_info_t *ci)
{
    int r = CAT_OK;
    _fr = f_open(&(ci->fd), chunk_name, FA_OPEN_EXISTING | FA_READ);
    if (_fr != FR_OK)
    {
        r = CAT_ERR_FATFS;
    }
    else
    {
        ci->entry[0] = '\0';
        ci->eof = false;
    }
    return r;
}


static void _close_chunk(chunk_info_t *ci)
{
    if (ci)
        f_close(&(ci->fd));
}


static const char* _fetch_chunk_entry(chunk_info_t *ci)
{
    const char* r;

    if (ci->entry[0])
    {
        r = ci->entry;
    }
    else if (ci->eof)
    {
        r = 0;
    }
    else
    {
        if (0 == f_gets(ci->entry, FF_LFN_BUF + 1, &(ci->fd)))
        {
            ci->eof = true;
            r = 0;
        }
        else
        {
            r = ci->entry;
        }
    }
    return r;
}


static void _clean_chunk_line(chunk_info_t *ci)
{
    ci->entry[0] = '\0';
}


static int _merge_catalog_chunks(catalog_t* cat, int chunk_count)
{
    int r = CAT_OK;
    char cat_name[FF_LFN_BUF + 1] = { 0 };
    char chunk_name[FF_LFN_BUF + 1];
    chunk_info_t* chunks = 0;
    int select;
    UINT wt;
    do
    {
        if (!_get_catalog_name(cat->dir, cat_name, FF_LFN_BUF + 1))
        {
            r = CAT_ERR_PATH_TOO_DEEP;
            break;
        }
        _fr = f_open(&(cat->fd), cat_name, FA_CREATE_ALWAYS | FA_WRITE);
        if (_fr != FR_OK)
        {
            r = CAT_ERR_FATFS;
            break;
        }
        if (chunk_count > 0)
        {
            chunks = MY_CALLOC(chunk_count, sizeof(chunk_info_t));
            if (0 == chunks)
            {
                r = CAT_ERR_MEMORY;
                break;
            }
            // open all chunks
            for (int i = 0; i < chunk_count; ++i)
            {
                _get_catalog_chunk_name(cat->dir, i, chunk_name, FF_LFN_BUF + 1);
                r = _open_chunk(chunk_name, &(chunks[i]));
                if (r != CAT_OK) break;
            }
            // catalog file header
            f_printf(&(cat->fd), "%d\n%d\n", cat->checksum, cat->count);
            int index = 0;
            int page = 0;
            for (;;)
            {
                bool alleofed = true;
                const char *min = 0, *str;
                int min_idx = 0;
                for (int i = 0; i < chunk_count; ++i)
                {
                    str = _fetch_chunk_entry(&(chunks[i]));
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
                if (0 == (index % cat->page_size))
                {
                    cat->page_offset[page] = (uint32_t)f_tell(&(cat->fd));
                    if (page + 1 >= MAX_PAGES)
                        break;
                    ++page;
                }
                ++index;
                _fr = f_write(&(cat->fd), min, strlen(min), &wt);
                if (_fr != FR_OK)
                {
                    r = CAT_ERR_FATFS;
                    break;
                }
                _clean_chunk_line(&(chunks[min_idx]));
            }
            cat->pages = page;
        }
        else
        {
            // the directory contains no entry
            // catalog file header
            f_printf(&(cat->fd), "%d\n%d\n", cat->checksum, cat->count);
            // set page 0 offset
            cat->page_offset[0] = (uint32_t)f_tell(&(cat->fd));
            cat->pages = 0;
        }
        // reopen catalog file
        f_close(&(cat->fd));
        f_open(&(cat->fd), cat_name, FA_OPEN_EXISTING | FA_READ);
        // set cur_page and cur_index to invalid values so catalog_move_cursor will move to new page
        cat->cur_page = -1;
        cat->cur_index = -1;
        r = catalog_move_cursor(cat, 0, 0);
    } while (0);
    if (r != CAT_OK)
    {
        // close catalog file object if failed
        f_close(&(cat->fd));
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


static int _create_catalog(const char * const patterns[], catalog_t *cat)
{
    int r = CAT_OK;
    char ucat[FF_LFN_BUF + 1] = { 0 }; // unsorted catalog file
    int chunk_count = 0;
    do
    {
        // create unsorted catalog
        if (!_get_unsorted_catalog_name(cat->dir, ucat, FF_LFN_BUF + 1))
        {
            r = CAT_ERR_PATH_TOO_DEEP;
            break;
        }
        r = _create_unsorted_catalog(cat->dir, ucat, patterns, cat->page_size * MAX_PAGES);
        if (r != CAT_OK)
            break;
        // partition unsorted catalog into chunks
        r = _partition_unsorted_catalog(ucat, cat->dir, ENTRIES_PER_CHUNK, &chunk_count);
        if (r != CAT_OK) 
        {
            break;
        }
        r = _merge_catalog_chunks(cat, chunk_count);
        if (r != CAT_OK) 
        {
            break;
        }
    } while (0);
    // delete unsorted catalog
    f_unlink(ucat);
    // delete chunks
    // if fail, chunk_count is unreliable. delete all possible unsorted chunks.
    if ((chunk_count == 0) && (r != CAT_OK)) chunk_count = MAX_PAGES;
    for (int i = 0; i < chunk_count; ++i)
    {
        // unsorted catalog file name is nolong being used, reuse here
        _get_catalog_chunk_name(cat->dir, i, ucat, FF_LFN_BUF + 1);    // wont fail
        f_unlink(ucat);
    }
    return r;    
}


// Open existing catalog file
// if safe_mode is true, compare checksum inside catalog object with the catalog file, and retrurn CAT_ERR_MISMATCH if mismatched
// if safe_mode is false, read checksum/count from catalog file
static int _open_catalog(catalog_t *cat, bool safe_mode)
{
    int r = CAT_OK;
    char cat_file[FF_LFN_BUF + 1];
    char temp[16];
    int val;

    do
    {
        if (!_get_catalog_name(cat->dir, cat_file, FF_LFN_BUF + 1))
        {
            r = CAT_ERR_PATH_TOO_DEEP;
            break;
        }
        _fr = f_open(&(cat->fd), cat_file, FA_READ | FA_OPEN_EXISTING);
        if (FR_OK != _fr)
        {
            r = CAT_ERR_FATFS;
            break;
        }
        if (safe_mode)
        {
            // 1st line is checksum
            if (0 == f_gets(temp, 16, &(cat->fd)))
            {
                r = CAT_ERR_MISMATCH;
                break;
            }
            val = atoi(temp);
            if (cat->checksum != val)
            {
                r = CAT_ERR_MISMATCH;
                break;
            }
            // 2nd line is entry count
            if (0 == f_gets(temp, 16, &(cat->fd)))
            {
                r = CAT_ERR_MISMATCH;
                break;
            }
            val = atoi(temp);
            if (cat->count != val)
            {
                r = CAT_ERR_MISMATCH;
                break;
            }
        }
        else
        {
            // unsafe mode, assume everything read from file are correct, unless cannot read
            // 1st line is checksum
            if (0 == f_gets(temp, 16, &(cat->fd)))
            {
                r = CAT_ERR_MISMATCH;
                break;
            }
            val = atoi(temp);
            cat->checksum = val;
            // 2nd line is entry count
            if (0 == f_gets(temp, 16, &(cat->fd)))
            {
                r = CAT_ERR_MISMATCH;
                break;
            }
            val = atoi(temp);
            cat->count = val;
        }
        // read all lines and get the offset to each page
        int index = 0;
        int page = 0;
        while (1)
        {
            if (0 == (index % cat->page_size))
            {
                cat->page_offset[page] = (uint32_t)f_tell(&(cat->fd));
                if (page + 1 >= MAX_PAGES)
                    break;
                ++page;
            }
            if (f_gets(cat_file, FF_LFN_BUF + 1, &(cat->fd)))  // reuse cat_file as buffer
            {
                ++index;
            }
            else
            {
                if (f_eof(&(cat->fd)))
                {
                    // f_gets returns 0 because of EOF
                    // if we just added a new page, remove it because it is empty
                    if (0 == (index % cat->page_size))
                    {
                        cat->page_offset[page] = 0;
                        --page;
                    }
                }
                else
                {
                    // f_gets returns 0 because of file system error
                    _fr = f_error(&(cat->fd));
                    r = CAT_ERR_FATFS;
                }
                break;
            }
        }
        if (r != CAT_OK) break; // probably CAT_ERR_FATFS
        // other structure members
        cat->pages = page;
        cat->count = index;
        // set cur_page and cur_index to invalid values so catalog_move_cursor will move to new page
        cat->cur_page = -1;
        cat->cur_index = -1;
        r = catalog_move_cursor(cat, 0, 0);
    } while (0);
    
    return r;
}



// If safe_mode is false, existing directory catalog will be read.
// If safe_mode is true, existing directory catalog will be verified before read.
// If no existing catalog found, it will be created.
int catalog_open_dir(const char *path, const char * const patterns[], int page_size, bool safe_mode, catalog_t **cat)
{
    // 1. Allocate catalog object
    // 2. Open directory and calculate checksum
    // 3. Open catalog file, if no valid catalog file found then create one
    int r = CAT_OK;
    catalog_t* c = 0;
    int count = 0;
    uint16_t checksum = 0;
    do
    {
        c = MY_CALLOC(sizeof(catalog_t), 1);
        if (0 == c)
        {
            r = CAT_ERR_MEMORY;
            break;
        }
        // calculate checksum for the directory (if safe_mode is true)
       if (safe_mode && (_calculate_checksum(path, patterns, page_size * MAX_PAGES, &checksum, &count) != FR_OK))
        {
            r = CAT_ERR_FATFS;
            break;
        }
        c->page_size = page_size;
        c->count = count;
        c->checksum = checksum;
        path_duplicate(path, c->dir, FF_LFN_BUF + 1);
        // try open existing catalog
        r = _open_catalog(c, safe_mode);
        if (r == CAT_OK) // success
            break;
        // Reach here if no valid catalog found. If we havn't calculated checksum (safe_mode is off), we need to do now
        if (!safe_mode)
        { 
            if (_calculate_checksum(path, patterns, page_size * MAX_PAGES, &checksum, &count) != FR_OK)
            {
                r = CAT_ERR_FATFS;
                break;
            }
            c->count = count;
            c->checksum = checksum;
        }
        r = _create_catalog(patterns, c);
    } while (0);

    if (r != CAT_OK) MY_FREE(c);
    *cat = c;

    return r;
}


int catalog_close(catalog_t *cat)
{
    if (cat)
    {
        f_close(&(cat->fd)); // file object is validated inside f_close
        MY_FREE(cat);
    }
    return CAT_OK;
}


// move catalog cursor to the specific page and index.
// the following catalog_get_entry will retrieve the entry.
int catalog_move_cursor(catalog_t *cat, int page, int index)
{
    int r = CAT_OK;
    do
    {
        // We need to rewind/move to another page in case:
        if ((page != cat->cur_page)                                  // a different page is requested
            ||
            (index < cat->cur_index)                                 // a previous entry is requested
            ||
            (index == cat->cur_index && cat->cache[0] != '\0')    // requesting entry at the cursor but this entry is already read
        )
        {
            _fr = f_lseek(&(cat->fd), (FSIZE_t)(cat->page_offset[page]));
            if (_fr != FR_OK)
            {
                r = CAT_ERR_FATFS;
                break;
            }
            cat->cur_page = page;
            cat->cur_index = 0;
            cat->cache[0] = '\0';
        }
        // read all entries before the requested entry then discard.
        // note if cache is not empty, we will start reading from cur_index + 1.
        int start = ('\0' == cat->cache[0]) ? cat->cur_index : cat->cur_index + 1;
        for (; start < index; ++start)
        {
            if (0 == f_gets(cat->cache, FF_LFN_BUF + 3, &(cat->fd)))
            {
                if (f_eof(&(cat->fd)))
                {
                    r = CAT_ERR_EOF;
                }
                else
                {
                    _fr = f_error(&(cat->fd));
                    r = CAT_ERR_FATFS;
                }
                break;
            }
            cat->cache[0] = '\0';
        }
        // set index
        if (cat->cur_index != index)
        {
            cat->cur_index = index;
            cat->cache[0] = '\0';
        }
    } while (0);
    return r;    
}


// get content of an entry
// read from cache if already read
int catalog_get_entry(catalog_t *cat, char *out, int len, bool absolute, uint8_t *type)
{
    char fn[FF_MAX_LFN + 1];
    int r = CAT_OK;
    do
    {
        // fetch cache if not already fetched
        if ('\0' == cat->cache[0])
        {
            if (0 == f_gets(cat->cache, FF_LFN_BUF + 3, &(cat->fd)))
            {
                if (f_eof(&(cat->fd)))
                {
                    r = CAT_ERR_EOF;
                }
                else
                {
                    _fr = f_error(&(cat->fd));
                    r = CAT_ERR_FATFS;
                }
                break;
            }
            path_trim_back(cat->cache);
        }
        if ('!' == cat->cache[0])
        {
            if (out != 0) path_duplicate(&(cat->cache[1]), fn, len);
            if (type != 0) *type = CAT_TYPE_DIRECTORY;
        }
        else
        {
            if (out != 0) path_duplicate(cat->cache, fn, len);
            if (type != 0) *type = CAT_TYPE_FILE;
        }
        // return absolute or relative path
        if (absolute)
        {
            path_concatenate(cat->dir, fn, out, len, false);
        }
        else
        {
            strncpy(out, fn, len);
        }
    } while (0);
    return r;
}


// get next entry
// when at the end of a page:
//   in_page = true: return EOF; in_page = false: advance to the next page
// when at the end of the catalog:
//   wrap = true: wrap to the very beginning of the catalog; wrap = false: return EOF
int catalog_get_next_entry(catalog_t *cat, bool in_page, bool wrap, char *out, int len, bool absolute, uint8_t *type)
{
    int r = CAT_OK;
    do
    {
        if ('\0' == cat->cache[0])
        {
            // cache is empty if we just opened catalog or moved cursor. it is not necessary to move cursor again
            break;
        }
        else
        {
            int i = cat->cur_index;
            int p = cat->cur_page;
            int last = (cat->count - 1) % cat->page_size;   // calculate index of last entry
            if ((p + 1 == cat->pages) && (i == last)) // if we are at the last entry of last page
            {
                if (!wrap)
                {
                    // not wrap operation, return eof
                    r = CAT_ERR_EOF;
                    break;
                }
                else
                {
                    // wrap to beginning
                    r = catalog_move_cursor(cat, 0, 0);
                    break;
                }
            }
            else if (i + 1 >= cat->page_size)    // if we are at the last entry of a middle page
            {
                if (in_page)
                {
                    // in_page operation, return eof
                    r = CAT_ERR_EOF;
                    break;
                }
                else
                {
                    // advance to next page
                    r = catalog_move_cursor(cat, p + 1, 0);
                    break;
                }
            }
            else
            {
                // no page adjustment, advance index
                r = catalog_move_cursor(cat, p, i + 1);
                break;
            }
        }
    } while (0);
    if (CAT_OK == r) r = catalog_get_entry(cat, out, len, absolute, type);
    return r;
}


// get previous entry
// when at the beginning of a page:
//   in_page = true: return EOF; in_page = false: move up to the last entry of previous page
// when at the very beginning of the catalog:
//   wrap = true: wrap to the last entry; wrap = false: return EOF
int catalog_get_prev_entry(catalog_t *cat, bool in_page, bool wrap, char *out, int len, bool absolute, uint8_t *type)
{
    // vs. move_next: 
    // in move_next, if cache is empty, we will fetch cache instead of move forward
    // in move_prev, even if cache is empty. we will move backward
    int r = CAT_OK;
    int i = cat->cur_index;
    int p = cat->cur_page;
    do
    {
        if (i <= 0) // if we are at the first entry of a page
        {
            if (in_page)
            {
                // in_page operation, return EOF
                r = CAT_ERR_EOF;
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
                        r = CAT_ERR_EOF;
                        break;
                    }
                    else
                    {
                        // go to last page, last entry, fetch cache
                        p = cat->pages - 1;
                        i = (cat->count - 1) % cat->page_size;
                        r = catalog_move_cursor(cat, p, i);
                        break;
                    }
                }
                else
                {
                    // go to the last entry of previous page
                    --p;
                    i = cat->page_size - 1;  // previous page will always be a full page, no need to check i
                    r = catalog_move_cursor(cat, p, i);
                    break;
                }
            }
        }
        else
        {
            // no page adjustment, just move index
            r = catalog_move_cursor(cat, p, i - 1);
            break;
        }
    } while (0);
    // read entry
    if (CAT_OK == r) r = catalog_get_entry(cat, out, len, absolute, type);
    return r;
}


FRESULT catalog_get_fatfs_error()
{
    return _fr;
}
