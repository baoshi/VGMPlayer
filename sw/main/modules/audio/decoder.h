#pragma once

#include <stdint.h>

//
// Generic Audio Decoder Interface
//


typedef struct decoder_s decoder_t;

typedef int16_t  (*get_sample_s16_t)(decoder_t *me, int16_t *buf, unsigned int len);

struct decoder_s
{
    get_sample_s16_t get_sample_s16;
    unsigned long    total_samples;
};
