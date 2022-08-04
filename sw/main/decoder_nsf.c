#include <string.h>
#include <pico/time.h>
#include "my_debug.h"
#include "my_mem.h"
#include "sw_conf.h"
#include "audio.h"
#include "decoder_nsf.h"



#ifndef NSF_DEBUG
#  define NSF_DEBUG 1
#endif

// Debug log
#if NSF_DEBUG
#define NSF_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define NSF_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define NSF_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define NSF_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define NSF_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define NSF_LOGD(x, ...)
#define NSF_LOGW(x, ...)
#define NSF_LOGE(x, ...)
#define NSF_LOGI(x, ...)
#define NSF_DEBUGF(x, ...)
#endif


static uint32_t decoder_nsf_get_samples(decoder_nsf_t *ctx, uint32_t *buf, uint32_t len)
{
    int16_t samples;
    uint16_t buffer[AUDIO_MAX_BUFFER_LENGTH];
    MY_ASSERT(len <= AUDIO_MAX_BUFFER_LENGTH);
    absolute_time_t start = get_absolute_time();
    samples = nsf_get_samples(ctx->nsf, len, buffer);
    for (int16_t i = 0; i < samples; ++i)
    {
        buf[i] = (((uint32_t)(buffer[i])) << 16) | (buffer[i]);
    }
    absolute_time_t end = get_absolute_time();
    int64_t us = absolute_time_diff_us(start, end);
    NSF_LOGD("NSF: %d samples in %u us\n", samples, us);
    return (uint32_t)samples;
}


decoder_nsf_t * decoder_nsf_create(char const *file, int song)
{
    decoder_nsf_t *decoder = NULL;
    do
    {
        decoder = MY_MALLOC(sizeof(decoder_nsf_t));
        if (0 == decoder)
            break;
        memset(decoder, 0, sizeof(decoder_nsf_t));
        // create nsf reader
        decoder->reader = nfr_create(file, NSF_CACHE_SIZE);
        if (0 == decoder->reader)
        {
            NSF_LOGW("NSF: nsf reader creation failed\n");
            MY_FREE(decoder);
            decoder = NULL;
            break;
        }
        // create nsf emulator
        decoder->nsf = nsf_create();
        if (0 == decoder->nsf)
        {
            NSF_LOGW("NSF: nsf creation failed\n");
            nfr_destroy(decoder->reader);
            MY_FREE(decoder);
            decoder = NULL;
            break;
        }
        int r = nsf_start_emu(decoder->nsf, decoder->reader, AUDIO_MAX_BUFFER_LENGTH, AUDIO_SAMPLE_RATE, NSF_OVER_SAMPLE_RATE);
        if (r != NSF_ERR_SUCCESS)
        {
            NSF_LOGW("NSF: nsf emulation startup returned %d\n", r);
            nsf_destroy(decoder->nsf);
            nfr_destroy(decoder->reader);
            MY_FREE(decoder);
            decoder = NULL;
            break;
        }
        nesbus_dump_handlers(decoder->nsf->bus);
        decoder->super.get_samples = (get_samples_t)decoder_nsf_get_samples;
        nsf_init_song(decoder->nsf, song);

        uint16_t buffer[AUDIO_MAX_BUFFER_LENGTH];
        absolute_time_t start = get_absolute_time();
        int16_t samples = nsf_get_samples(decoder->nsf, AUDIO_MAX_BUFFER_LENGTH, buffer);
        absolute_time_t end = get_absolute_time();
        int64_t us = absolute_time_diff_us(start, end);
        NSF_LOGD("NSF: %d samples in %u us\n", samples, us);

    } while (0);

    return decoder;
}


void decoder_nsf_destroy(decoder_nsf_t *ctx)
{
    if (ctx)
    {
        if (ctx->nsf) nsf_destroy(ctx->nsf);
        nfr_show_cache_status(ctx->reader);
        if (ctx->reader) nfr_destroy(ctx->reader);
        MY_FREE(ctx);
    }
}


