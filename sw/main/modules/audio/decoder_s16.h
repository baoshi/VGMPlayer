#pragma once

#include "audio.h"
#include "decoder.h"

typedef struct decoder_s16_s
{
    decoder_t super;
    audio_file_reader_t *reader;
    unsigned int samples;
} decoder_s16_t;


decoder_s16_t * decoder_s16_create(char const *file);
void decoder_s16_destroy(decoder_s16_t* me);

