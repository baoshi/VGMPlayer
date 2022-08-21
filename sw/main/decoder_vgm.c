#include <string.h>
#include <pico/time.h>
#include <inttypes.h>
#include "my_debug.h"
#include "my_mem.h"
#include "sw_conf.h"
#include "audio.h"
#include "file_reader_cached_fatfs.h"
#include "decoder_vgm.h"


#ifndef VGM_DEBUG
#  define VGM_DEBUG 1
#endif

// Debug log
#if VGM_DEBUG
#define VGM_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define VGM_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define VGM_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define VGM_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define VGM_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define VGM_LOGD(x, ...)
#define VGM_LOGW(x, ...)
#define VGM_LOGE(x, ...)
#define VGM_LOGI(x, ...)
#define VGM_DEBUGF(x, ...)
#endif


static uint32_t decoder_vgm_get_samples(decoder_vgm_t *ctx, uint32_t *buf, uint32_t len)
{
    int16_t samples;
    uint16_t nibble;
    MY_ASSERT(len <= AUDIO_MAX_BUFFER_LENGTH);
    absolute_time_t start = get_absolute_time();
    samples = vgm_get_samples(ctx->vgm, ctx->buffer, len);
    for (int16_t i = 0; i < samples; ++i)
    {
        nibble = (uint16_t)(ctx->buffer[i]);
        buf[i] = (((uint32_t)nibble) << 16) | nibble;
    }
    absolute_time_t end = get_absolute_time();
    int64_t us = absolute_time_diff_us(start, end);
    if (us > 1000000 * len / 44100)
        VGM_LOGD("VGM: %d samples in %" PRId64 " us\n", samples, us);
    return (uint32_t)samples;
}


decoder_vgm_t * decoder_vgm_create(char const *file)
{
    decoder_vgm_t *decoder = NULL;
    do
    {
        decoder = MY_MALLOC(sizeof(decoder_vgm_t));
        if (0 == decoder)
            break;
        memset(decoder, 0, sizeof(decoder_vgm_t));
        // create nsf reader
        decoder->reader = cfr_create(file, VGM_CACHE_SIZE);
        if (0 == decoder->reader)
        {
            VGM_LOGW("VGM: file reader creation failed\n");
            MY_FREE(decoder);
            decoder = NULL;
            break;
        }
        // create nsf emulator
        decoder->vgm = vgm_create(decoder->reader);
        if (0 == decoder->vgm)
        {
            VGM_LOGW("VGM: vgm creation failed\n");
            cfr_destroy(decoder->reader);
            MY_FREE(decoder);
            decoder = NULL;
            break;
        }
        vgm_prepare_playback(decoder->vgm, AUDIO_SAMPLE_RATE, true);
        decoder->super.get_samples = (get_samples_t)decoder_vgm_get_samples;
        uint16_t buffer[AUDIO_MAX_BUFFER_LENGTH];
        int16_t samples = vgm_get_samples(decoder->vgm, buffer, AUDIO_MAX_BUFFER_LENGTH);
    } while (0);
    return decoder;
}


void decoder_vgm_destroy(decoder_vgm_t *ctx)
{
    if (ctx)
    {
        if (ctx->vgm) vgm_destroy(ctx->vgm);
        if (ctx->reader)
        {
            cfr_show_cache_status(ctx->reader);
            cfr_destroy(ctx->reader);
        }
        MY_FREE(ctx);
    }
}


