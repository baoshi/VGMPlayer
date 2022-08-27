#include <string.h>
#include <pico/time.h>
#include <inttypes.h>
#include "audio.h"
#include "cached_file_reader.h"
#include "vgm_conf.h"
#include "decoder_vgm.h"


static uint32_t decoder_vgm_get_samples(decoder_vgm_t *ctx, uint32_t *buf, uint32_t len)
{
    int16_t *buf16 = (uint16_t *)buf;
    int samples;
    MY_ASSERT(len <= AUDIO_FRAME_LENGTH);
    absolute_time_t start = get_absolute_time();
    // caller requested for len samples of uint32_t. we ask len samples of int16_t from vgm, then extend i16 to u32
    samples = vgm_get_samples(ctx->vgm, buf16, len);
    // |AA|BB|CC|DD|EE| --> |AA|AA|BB|BB|CC|CC|DD|DD|EE|EE|  (view as int16_t)
    for (int i = samples - 1; i >= 0; --i)
    {
        buf16[i + i + 1] = buf16[i];
        buf16[i + i] = buf16[i];
    }
    absolute_time_t end = get_absolute_time();
    int64_t us = absolute_time_diff_us(start, end);
    if (us > 1000000 * len / 44100)
        AUD_LOGD("Audio/vgm: %d samples in %" PRId64 " us\n", samples, us);
    return (uint32_t)samples;
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
        decoder->super.get_samples = (get_samples_t)decoder_vgm_get_samples;
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


