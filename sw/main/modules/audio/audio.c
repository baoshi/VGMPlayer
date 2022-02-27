#include <hardware/gpio.h>
#include "hw_conf.h"
#include "my_debug.h"
#include "my_mem.h"
#include "event_ids.h"
#include "event_queue.h"
#include "wm8978.h"
#include "i2s.h"
#include "audio.h"


#define AUDIO_MAX_BUFFER_LENGTH 1024


#ifndef AUDIO_DEBUG
#  define AUDIO_DEBUG 1
#endif

// Debug log
#if AUDIO_DEBUG
#define AUD_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define AUD_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define AUD_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define AUD_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define AUD_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define AUD_LOGD(x, ...)
#define AUD_LOGW(x, ...)
#define AUD_LOGE(x, ...)
#define AUD_LOGI(x, ...)
#define AUD_DEBUGF(x, ...)
#endif


uint32_t tx_buf0[AUDIO_MAX_BUFFER_LENGTH];
uint32_t tx_buf0_len = 0;
uint32_t tx_buf1[AUDIO_MAX_BUFFER_LENGTH];
uint32_t tx_buf1_len = 0;
bool cur_tx_buf = false;    // current buffer being tx'd. false:buf0; true:buf1


// Jack detection fsm
#define JACK_DEBOUNCE_MS 100
static uint32_t _jd_timestamp = 0;
static enum 
{
    JACK_NONE,
    JACK_EMPTY,
    JACK_BOUNCING_IN,
    JACK_INSERTED,
    JACK_BOUNCING_OUT
} _jd_state;


void audio_preinit()
{
    i2s_init();
    wm8978_preinit();
    // Jack detection GPIO pin
    gpio_init(JACK_DETECTION_PIN);
    gpio_disable_pulls(JACK_DETECTION_PIN);
    gpio_set_dir(JACK_DETECTION_PIN, GPIO_IN);
    _jd_state = JACK_EMPTY;
    _jd_timestamp = 0;
}


void audio_postinit()
{
    wm8978_postinit();
}


void audio_close()
{
    wm8978_powerdown();
    i2s_deinit();
    _jd_state = JACK_NONE;
}


void i2s_notify_cb(int notify, void *param)
{

}


audio_playback_ctx * audio_setup_playback(decoder_t *decoder)
{
    audio_playback_ctx *ctx = MY_ALLOC(sizeof(audio_playback_ctx));
    MY_ASSERT(ctx);
    ctx->decoder = decoder;
}


void audio_playback(audio_playback_ctx *ctx)
{

}


/**
 * @brief Call this function repeatedly to receive jack detection notification
 * 
 * @param now       Timestamp
 * @return * int    1 if earpiece plugged
 *                  2 if earpiece unplugged
 *                  0 if no change
 */
int audio_update(uint32_t now)
{
    int ret = 0;
    bool detected = gpio_get(JACK_DETECTION_PIN);   // JACK_DETECTION_PIN is high if inserted
    switch (_jd_state)
    {
    case JACK_EMPTY:
        if (detected)
        {
            _jd_timestamp = now;
            _jd_state = JACK_BOUNCING_IN;
        }
        break;
    case JACK_BOUNCING_IN:
        if (!detected)
        {
            _jd_state = JACK_EMPTY;
        }
        else if (now - _jd_timestamp >= JACK_DEBOUNCE_MS)
        {
            // jack plugged
            AUD_LOGI("Audio: Earpiece plugged @%d\n", now);
            ret = 1;
            _jd_state = JACK_INSERTED;
            EQ_QUICK_PUSH(EVT_EARPIECE_PLUGGED);
        }
        break;
    case JACK_INSERTED:
        if (!detected)
        {
            _jd_timestamp = now;
            _jd_state = JACK_BOUNCING_OUT;
        }
        break;
    case JACK_BOUNCING_OUT:
        if (detected)
        {
            _jd_state= JACK_INSERTED;
        }
        else if (now - _jd_timestamp >= JACK_DEBOUNCE_MS)
        {
            // jack unplugged
            AUD_LOGI("Audio: Earpiece unplugged @%d\n", now);
            ret = 2;
            _jd_state = JACK_EMPTY;
            EQ_QUICK_PUSH(EVT_EARPIECE_UNPLUGGED);
        }
        break;
    case JACK_NONE:
        // no break
    default:
        break;
    }
    return ret;
}
