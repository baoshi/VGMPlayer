
#include <stdio.h>
#include "my_debug.h"
#include "my_mem.h"
#include "sample_decoder.h"

#include "sound.inc"


char const * sample_decoder_get_meta_str(sample_decoder_t *me, int key)
{
    switch (key)
    {
    case DECODER_STR_META_TITLE:
        return "sound sample";
    case DECODER_STR_META_COPYRIGHT:
        return "no rights";
    case DECODER_STR_META_ARTEST:
        return "vintage game sound";
    }
    return 0;
}


int sample_decoder_get_meta_int(sample_decoder_t *me, int key)
{
    switch (key)
    {
        case DECODER_INT_META_LENGTH_MS:
            return (me->total / 48);
        case DECODER_INT_META_NUM_TRACKS:
            return 1;
    }
    return -1;
}


uint32_t sample_decoder_get_samples(sample_decoder_t *me, uint32_t *buf, uint32_t len)
{
    uint32_t i;
    for (i = 0; (i < len && me->idx < me->total); ++i, ++(me->idx))
    {
        uint32_t t = me->samples[me->idx];
        buf[i] = t << 16 | t;
    }
    return i;
}


sample_decoder_t* sample_decoder_create()
{
    sample_decoder_t* decoder = MY_MALLOC(sizeof(sample_decoder_t));
    decoder->super.get_meta_str = (get_meta_str_t)sample_decoder_get_meta_str;
    decoder->super.get_meta_int = (get_meta_int_t)sample_decoder_get_meta_int;
    decoder->super.get_samples = (get_samples_t)sample_decoder_get_samples;
    decoder->samples = (uint16_t*)sound_bin;
    decoder->total = sound_bin_len / 2;
    decoder->idx = 0;
    return decoder;
}


void sample_decoder_destroy(sample_decoder_t* me)
{
    if (me)
        MY_FREE(me);
}

