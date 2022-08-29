#pragma once

#include <stdint.h>

//
// Generic Audio Decoder Interface
//


typedef struct decoder_s decoder_t;

typedef uint32_t     (*get_samples_t)(decoder_t *me, uint32_t *buf, uint32_t len);

struct decoder_s
{
    get_samples_t  get_samples;
    unsigned long  total_samples;
};
