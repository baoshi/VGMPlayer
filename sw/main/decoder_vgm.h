#pragma once

#include <ff.h>
#include "audio.h"
#include "decoder.h"
#include "vgm.h"


typedef struct decoder_vgm_s
{
    decoder_t super;
    char const *fn;
    FIL fd;
    file_reader_t *reader;
    vgm_t *vgm;
    int16_t buffer[AUDIO_MAX_BUFFER_LENGTH];
} decoder_vgm_t;


decoder_vgm_t * decoder_vgm_create(char const *file);
void decoder_vgm_destroy(decoder_vgm_t* me);

