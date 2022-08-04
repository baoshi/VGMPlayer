#pragma once

#include <stdint.h>
#include <ff.h>


#define MAX_PAGES 64    // maximum number of entry pages


enum
{
    CAT_OK = 0,
    CAT_ERR_MEMORY,
    CAT_ERR_MISMATCH,
    CAT_ERR_PATH_TOO_DEEP,
    CAT_ERR_EOF,
    CAT_ERR_FATFS,
};


enum 
{
    CAT_TYPE_UNKNOWN = 0,
    CAT_TYPE_FILE,
    CAT_TYPE_DIRECTORY
};


typedef struct _catalog_s
{
    char        dir[FF_LFN_BUF + 1];    // catalog directory
    FIL         fd;                     // catalog file handle
    uint16_t    checksum;               // checksum
    int         count;                  // total count under this directory
    int         page_size;              // directory list pager size
    int         pages;                  // total pages
    uint32_t    page_offset[MAX_PAGES]; // offset to the first file of each page in catalog file
    char        cache[FF_LFN_BUF + 3];  // current entry
    int         cur_page;               // current page
    int         cur_index;              // current entry index in the page
} catalog_t;


int catalog_open_dir(const char *path, const char * const pats[], int page_size, bool safe_mode, catalog_t **lister);

int catalog_close(catalog_t *cat);

int catalog_move_cursor(catalog_t *cat, int page, int index);

int catalog_get_entry(catalog_t *cat, char *out, int len, bool absolute, uint8_t *type);

int catalog_get_next_entry(catalog_t *cat, bool in_page, bool wrap, char *out, int len, bool absolute, uint8_t *type);

int catalog_get_prev_entry(catalog_t *cat, bool in_page, bool wrap, char *out, int len, bool absolute, uint8_t *type);

FRESULT catalog_get_fatfs_error();