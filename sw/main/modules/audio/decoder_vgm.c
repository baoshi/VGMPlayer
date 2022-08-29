#include <string.h>
#include <pico/time.h>
#include <inttypes.h>
#include "audio.h"
#include "cached_file_reader.h"
#include "vgm_conf.h"
#include "decoder_vgm.h"


static unsigned int decoder_vgm_get_samples(decoder_t *me, int16_t *buf, unsigned int len)
{
    decoder_vgm_t *ctx = (decoder_vgm_t *)me;
    int samples;
    MY_ASSERT(len <= AUDIO_FRAME_LENGTH);
    absolute_time_t start = get_absolute_time();
    samples = vgm_get_samples(ctx->vgm, buf, len);
    absolute_time_t end = get_absolute_time();
    int64_t us = absolute_time_diff_us(start, end);
    if (us > 1000000 * len / 44100)
        AUD_LOGD("Audio/vgm: %d samples in %" PRId64 " us\n", samples, us);
    return (unsigned int)samples;
}


decoder_vgm_t * decoder_vgm_create(char const *file)
{
    decoder_vgm_t *decoder = NULL;
    do
    {
        decoder = audio_malloc(sizeof(decoder_vgm_t));
        if (NULL == decoder)
        {
            AUD_LOGW("Audio/vgm: out of memory\n");
            break;
        }
        memset(decoder, 0, sizeof(decoder_vgm_t));
        decoder->reader = cfreader_create(file, VGM_FILE_CACHE_SIZE);
        if (NULL == decoder->reader)
        {
            AUD_LOGW("Audio/vgm: file reader creation failed\n");
            audio_free(decoder);
            break;
        }
        decoder->vgm = vgm_create(decoder->reader);
        if (NULL == decoder->vgm)
        {
            AUD_LOGW("Audio/vgm: vgm creation failed\n");
            cfreader_destroy(decoder->reader);
            audio_free(decoder);
            decoder = NULL;
            break;
        }
        vgm_prepare_playback(decoder->vgm, AUDIO_SAMPLE_RATE, true);
        decoder->super.get_sample_s16 = (get_sample_s16_t)decoder_vgm_get_samples;
        decoder->super.total_samples = (unsigned int)(decoder->vgm->complete_samples);
    } while (0);
    return decoder;
}


void decoder_vgm_destroy(decoder_vgm_t *ctx)
{
    if (ctx)
    {
        if (ctx->vgm) 
        {
            vgm_destroy(ctx->vgm);
        }
        if (ctx->reader)
        {
            cfreader_show_cache_status(ctx->reader);
            cfreader_destroy(ctx->reader);
        }
        audio_free(ctx);
    }
}


