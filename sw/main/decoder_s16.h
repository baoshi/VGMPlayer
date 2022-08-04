#pragma once

#include <ff.h>
#include "audio.h"
#include "decoder.h"

typedef struct decoder_s16_s
{
    decoder_t super;
    char const *fn;
    FIL fd;
    uint32_t samples;
} decoder_s16_t;


decoder_s16_t * decoder_s16_create(char const *file);
void decoder_s16_destroy(decoder_s16_t* me);

