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

struct decoder_s
{
    char const *    (*get_meta_str)(decoder_t *me, int key);
    int             (*get_meta_int)(decoder_t *me, int key);
    uint32_t        (*get_samples)(decoder_t *me, uint32_t *buf, uint32_t len);     // decoder main interface
};
