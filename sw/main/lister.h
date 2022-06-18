#pragma once

#include <stdint.h>
#include <ff.h>


#define MAX_PAGES            64
#define CATALOG_FILE         ".file.cat"
#define RAW_CATALOG_FILE     ".temp.cat"
#define CHUNK_FILE_PATTERN   ".t%d.cat"


// LINES_PER_CHUNK affects directory sorting speed and memory consumption
// Experiments: (455 files)
// LINES_PER_CHUNK          Memory required         Catalog creation time
//      20                      5200                    21.05s
//      64                     16650                     8.79s
//     128                     33280                     5.49s
//     256                     66560                     3.85s

#define LINES_PER_CHUNK     128


enum
{
    LS_OK = 0,
    LS_ERR_MEMORY,
    LS_ERR_MISMATCH,
    LS_ERR_PATH_TOO_DEEP,
    LS_ERR_EOF,
    LS_ERR_FATFS,
};


enum 
{
    LS_TYPE_UNKNOWN = 0,
    LS_TYPE_FILE,
    LS_TYPE_DIRECTORY
};


typedef struct _lister_s
{
    char        dir[FF_LFN_BUF + 1];    // listing directory
    FIL         fd;                     // catalog file handle
    uint16_t    checksum;               // checksum
    int         count;                  // total count under this directory
    int         page_size;              // directory list pager size
    int         pages;                  // total pages
    uint32_t    page_offset[MAX_PAGES]; // offset to the first file of each page in catalog file
    char        cache[FF_LFN_BUF + 3];  // current entry
    int         cur_page;               // current page
    int         cur_index;              // current entry index in the page
} lister_t;


int lister_open_dir(const char *path, const char * const pats[], int page_size, bool safe_mode, lister_t **lister);

int lister_close(lister_t *lister);

int lister_select_page(lister_t *lister, int page);

int lister_move_to(lister_t *lister, int page, int index);

int lister_move_next(lister_t *lister, bool in_page, bool wrap);

int lister_move_prev(lister_t *lister, bool in_page, bool wrap);

int lister_get_entry(lister_t *lister, char *out, int len, uint8_t *type);

int lister_get_next_entry(lister_t *lister, bool in_page, bool wrap, char *out, int len, uint8_t *type);

int lister_get_prev_entry(lister_t *lister, bool in_page, bool wrap, char *out, int len, uint8_t *type);

FRESULT lister_get_fatfs_error();