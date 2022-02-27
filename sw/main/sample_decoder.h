#pragma once

#include "decoder.h"

typedef struct sample_decoder_s
{
    decoder_t super;
    uint16_t *samples;
    uint32_t idx;
    uint32_t total;
} sample_decoder_t;


sample_decoder_t* sample_decoder_create();
void sample_decoder_destroy(sample_decoder_t* me);
