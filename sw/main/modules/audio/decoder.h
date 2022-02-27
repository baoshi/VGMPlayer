#pragma once

#include <stdint.h>

//
// Generic Audio Decoder Interface
//

#define DECODER_STR_META_TITLE        1001
#define DECODER_STR_META_COPYRIGHT    1002
#define DECODER_STR_META_ARTEST       1003
#define DECODER_INT_META_LENGTH_MS    2001
#define DECODER_INT_META_NUM_TRACKS   2002

typedef struct decoder_s decoder_t;

typedef char const * (*get_meta_str_t)(decoder_t *me, int key);
typedef int          (*get_meta_int_t)(decoder_t *me, int key);
typedef uint32_t     (*get_samples_t)(decoder_t *me, uint32_t *buf, uint32_t len);


struct decoder_s
{
    get_meta_str_t get_meta_str;
    get_meta_int_t get_meta_int;
    get_samples_t  get_samples;
};
