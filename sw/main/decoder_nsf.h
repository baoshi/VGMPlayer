#pragma once

#include <ff.h>
#include "audio.h"
#include "decoder.h"
#include "nsf.h"
#include "nsfreader_file_fatfs.h"

typedef struct decoder_nsf_s
{
    decoder_t super;
    char const *fn;
    FIL fd;
    nsfreader_t *reader;
    nsf_t *nsf;
} decoder_nsf_t;


decoder_nsf_t * decoder_nsf_create(char const *file, int song);
void decoder_nsf_destroy(decoder_nsf_t* me);

