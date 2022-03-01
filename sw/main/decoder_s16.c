#include <string.h>
#include "my_debug.h"
#include "my_mem.h"
#include "decoder_s16.h"



#ifndef S16_DEBUG
#  define S16_DEBUG 1
#endif

// Debug log
#if S16_DEBUG
#define S16_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define S16_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define S16_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define S16_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define S16_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define S16_LOGD(x, ...)
#define S16_LOGW(x, ...)
#define S16_LOGE(x, ...)
#define S16_LOGI(x, ...)
#define S16_DEBUGF(x, ...)
#endif


uint32_t decoder_s16_get_samples(decoder_s16_t *me, uint32_t *buf, uint32_t len)
{
    // For len uint32_t samples, we need to read len uint16_t data, or len * 2 uint8_t data
    FRESULT fr;
    UINT ret = 0;
    fr = f_read(&(me->fd), me->buffer, len * 2, &ret);
    ret = ret / 2;
    for (int i = 0; i < ret; ++i)
    {
        buf[i] = (((uint32_t)(me->buffer[i])) << 16) | (me->buffer[i]);
    }
    S16_LOGD("S16: return %d samples\n", ret);
    return (uint32_t)ret;
}


decoder_s16_t * decoder_s16_create(char const *file)
{
    decoder_s16_t *decoder = MY_MALLOC(sizeof(decoder_s16_t));
    if (decoder)
    {
        memset(decoder, 0, sizeof(decoder_s16_t));
        if (FR_OK == f_open(&(decoder->fd), file, FA_READ))
        {
            decoder->samples = f_size(&(decoder->fd)) / 2;
            decoder->super.get_samples = (get_samples_t)decoder_s16_get_samples;
        }
        else
        {
            S16_LOGW("S16: open %s failed\n", file);
            MY_FREE(decoder);
            decoder = 0;
        }
    }
    else
    {
        S16_LOGW("S16: out of memory\n");
    }
    return decoder;
}


void decoder_s16_destroy(decoder_s16_t* me)
{
    if (me)
    {
        f_close(&(me->fd));
        MY_FREE(me);
    }
}
