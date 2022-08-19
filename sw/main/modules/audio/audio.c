#include <string.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include "hw_conf.h"
#include "sw_conf.h"
#include "my_debug.h"
#include "my_mem.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "wm8978.h"
#include "i2s.h"
#include "audio.h"


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

// Audio buffer
uint32_t audio_tx_buf0[AUDIO_MAX_BUFFER_LENGTH];
volatile int32_t audio_tx_buf0_len = 0;
uint32_t audio_tx_buf1[AUDIO_MAX_BUFFER_LENGTH];
volatile int32_t audio_tx_buf1_len = 0;
volatile bool audio_cur_tx_buf = false;    // current buffer being tx'd. false:buf0; true:buf1

// Playback states
typedef enum playback_states
{
    PLAY_NORMAL,
    PLAY_RAMPDOWN,
    PLAY_SILENCE,
    PLAY_RAMPUP,
    PLAY_STOPPING
} playback_state_t;
static playback_state_t _play_state;


// context used during playback, shared by audio_xxx and decoder_entry on core 1
typedef struct playback_ctx_s
{
    decoder_t *decoder;
} playback_ctx_t;
static playback_ctx_t playback_ctx;


// Commands send to decoding thread
enum decoder_cmds
{
    DECODER_GET_SAMPLE_BLOCKING = 1,
    DECODER_GET_SAMPLE,
    DECODER_GEN_RAMPDOWN,
    DECODER_GEN_RAMPUP,
    DECODER_GEN_SILENCE,
    DECODER_STOP,
    DECODER_FINISH,
};


// Jack detection fsm
#define JACK_DEBOUNCE_MS 100
static uint32_t _jack_timestamp = 0;
static enum jack_states
{
    JACK_NONE,
    JACK_EMPTY,
    JACK_BOUNCING_IN,
    JACK_INSERTED,
    JACK_BOUNCING_OUT
} _jack_state;


void audio_preinit()
{
    AUD_LOGD("Audio: PreInit @ %d\n", tick_millis());
    // Disable jack output at beginning
    gpio_init(JACK_OUTEN_PIN);
    gpio_put(JACK_OUTEN_PIN, false);
    gpio_set_dir(JACK_OUTEN_PIN, GPIO_OUT);
    // Initailize I2S and WM8978
    i2s_init();
    wm8978_preinit();
    // Enable jack detection GPIO
    gpio_init(JACK_DETECTION_PIN);
    gpio_disable_pulls(JACK_DETECTION_PIN);
    gpio_set_dir(JACK_DETECTION_PIN, GPIO_IN);
    gpio_init(JACK_DETECTION_ADC_PIN);
    gpio_disable_pulls(JACK_DETECTION_ADC_PIN);
    gpio_set_dir(JACK_DETECTION_ADC_PIN, GPIO_IN);
    _jack_state = JACK_EMPTY;
    _jack_timestamp = 0;
}


void audio_postinit()
{
    AUD_LOGD("Audio: PostInit @ %d\n", tick_millis());
    wm8978_postinit();
    audio_set_headphone_volume(config.volume_headphone);
    audio_set_speaker_volume(config.volume_speaker);
    audio_jack_enable(true);
}


void audio_close()
{
    wm8978_powerdown();
    i2s_deinit();
    _jack_state = JACK_NONE;
}


// decoder thread function running on core 1
void decoder_entry()
{
    uint32_t last_cmd = 0;
    uint32_t last_sample = 0;
    bool finishing = false;
    int silence_gap = AUDIO_SONG_GAP_MS * AUDIO_SAMPLE_RATE / AUDIO_MAX_BUFFER_LENGTH / 1000;    // number of silence buffer to send before finish the song
    bool stop = false;
    AUD_LOGD("Audio: core1: entry\n");
    while (!stop)
    {
        uint32_t cmd = multicore_fifo_pop_blocking();
        // find which buffer to receive new samples
        uint32_t *obuf = audio_cur_tx_buf ? audio_tx_buf0 : audio_tx_buf1;
        uint32_t *olen = audio_cur_tx_buf ? (uint32_t *)&audio_tx_buf0_len : (uint32_t *)&audio_tx_buf1_len;
        register int16_t l, ll, r, rr;
        register int x;
        event_t e;
        switch (cmd)
        {
        case DECODER_GET_SAMPLE_BLOCKING:
            if (cmd != last_cmd) AUD_LOGD("Audio: core1: GET_SAMPLE_BLOCKING\n");
            finishing = false;
            *olen = playback_ctx.decoder->get_samples(playback_ctx.decoder, obuf, AUDIO_MAX_BUFFER_LENGTH);
            // Notify new audio samples
            e.code = EVT_AUDIO_SAMPLE_READY;
            e.param = (void *)(!audio_cur_tx_buf);  // true if new samples in buf1, false if new samples in buf0
            event_queue_push_back(&e, true);
            multicore_fifo_push_blocking(0);    // unblock caller
            break;

        case DECODER_GET_SAMPLE:
            if (cmd != last_cmd) AUD_LOGD("Audio: core1: GET_SAMPLE\n");
            if (!finishing)
            {
                // decoder not finished, continue get sample
                *olen = playback_ctx.decoder->get_samples(playback_ctx.decoder, obuf, AUDIO_MAX_BUFFER_LENGTH);
                if (*olen == 0)
                {
                    // no more samples. we will prepare rampdown buffer then finish
                    AUD_LOGD("Audio: core1: auto rampdown\n");
                    finishing = true;   // next time DECODER_GET_SAMPLE will have finishing == true
                    int16_t l = (int16_t)(last_sample >> 16);
                    int16_t r = (int16_t)(last_sample & 0xffff);
                    register int16_t ll, rr;
                    register int x;
                    for (register int i = 0; i < AUDIO_MAX_BUFFER_LENGTH; ++i)
                    {
                        x = AUDIO_MAX_BUFFER_LENGTH - 1 - i;
                        ll = l * x / (AUDIO_MAX_BUFFER_LENGTH - 1);
                        rr = r * x / (AUDIO_MAX_BUFFER_LENGTH - 1);
                        obuf[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
                    }
                    *olen = AUDIO_MAX_BUFFER_LENGTH;
                }
                else
                {
                    last_sample = obuf[*olen - 1];   // keep last sampled value for rampdown
                    // Notify new audio samples
                    e.code = EVT_AUDIO_SAMPLE_READY;
                    e.param = (void *)(!audio_cur_tx_buf);  // true if new samples in buf1, false if new samples in buf0
                    event_queue_push_back(&e, true);
                }
            }
            else if (silence_gap > 0)
            {
                // decoder finished, sending silence buffer
                --silence_gap;
                memset(obuf, 0, AUDIO_MAX_BUFFER_LENGTH * sizeof(uint32_t));
                *olen = AUDIO_MAX_BUFFER_LENGTH;
            }
            else
            {
                // sending slience buffer finished
                *olen = 0;  // This will tell i2s to finish
                AUD_LOGD("Audio: core1: no more samples\n");
            }
            break;

        case DECODER_GEN_RAMPDOWN:
            if (cmd != last_cmd) AUD_LOGD("Audio: core1: GEN_RAMPDOWN\n");
            l = (int16_t)(last_sample >> 16);
            r = (int16_t)(last_sample & 0xffff);
            for (register int i = 0; i < AUDIO_MAX_BUFFER_LENGTH; ++i)
            {
                x = AUDIO_MAX_BUFFER_LENGTH - 1 - i;
                ll = l * x / (AUDIO_MAX_BUFFER_LENGTH - 1);
                rr = r * x / (AUDIO_MAX_BUFFER_LENGTH - 1);
                obuf[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
            }
            *olen = AUDIO_MAX_BUFFER_LENGTH;
            break;

        case DECODER_GEN_RAMPUP:
            if (cmd != last_cmd) AUD_LOGD("Audio: core1: GEN_RAMPUP\n");
            l = (int16_t)(last_sample >> 16);
            r = (int16_t)(last_sample & 0xffff);
            for (register int i = 0; i < AUDIO_MAX_BUFFER_LENGTH; ++i)
            {
                ll = l * i / (AUDIO_MAX_BUFFER_LENGTH - 1);
                rr = r * i / (AUDIO_MAX_BUFFER_LENGTH - 1);
                obuf[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
            }
            *olen = AUDIO_MAX_BUFFER_LENGTH;
            break;

        case DECODER_GEN_SILENCE:
            if (cmd != last_cmd) AUD_LOGD("Audio: core1: GEN_SILENCE\n");
            memset(obuf, 0, AUDIO_MAX_BUFFER_LENGTH * sizeof(uint32_t));
            *olen = AUDIO_MAX_BUFFER_LENGTH;
            e.code = EVT_AUDIO_SAMPLE_READY;
            e.param = (void *)(!audio_cur_tx_buf);  // true if new samples in buf1, false if new samples in buf0
            event_queue_push_back(&e, true);
            break;

        case DECODER_STOP:
            if (cmd != last_cmd) AUD_LOGD("Audio: core1: STOP\n");
            *olen = 0;  // set output sample size to 0 so i2s won't request new samples anymore
            stop = true;
            break;

        case DECODER_FINISH:
            if (cmd != last_cmd) AUD_LOGD("Audio: core1: FINISH\n");
            // no break
        default:
            stop = true;
            break;
        }
        last_cmd = cmd;
    }
    multicore_fifo_push_blocking(0);    // Core0 may wait for thread exit
    AUD_LOGD("Audio: core1: exit\n");
}


void i2s_notify_cb(int notify, void *param)
{
    switch (notify)
    {
    case I2S_NOTIFY_SAMPLE_REQUESTED:
        switch (_play_state)
        {
        case PLAY_NORMAL:
            multicore_fifo_push_blocking(DECODER_GET_SAMPLE);
            break;
        case PLAY_RAMPDOWN:
            multicore_fifo_push_blocking(DECODER_GEN_RAMPDOWN);
            _play_state = PLAY_SILENCE;
            break;
        case PLAY_RAMPUP:
            multicore_fifo_push_blocking(DECODER_GEN_RAMPUP);
            _play_state = PLAY_NORMAL;
            break;
        case PLAY_SILENCE:
            multicore_fifo_push_blocking(DECODER_GEN_SILENCE);
            break;
        case PLAY_STOPPING:
            multicore_fifo_push_blocking(DECODER_STOP);
            break;
        }
        break;
    case I2S_NOTIFY_PLAYBACK_FINISHING:
        multicore_fifo_push_blocking(DECODER_FINISH);
        EQ_QUICK_PUSH(EVT_AUDIO_SONG_ENDED);
        break;
    }
}


void audio_setup_playback(decoder_t *decoder)
{
    AUD_LOGD("Audio: core0: setup\n");
    i2s_stop_playback();
    playback_ctx.decoder = decoder;
    // launch decoder thread on core1, the thread will be blocked until command is received
    multicore_fifo_drain();
    multicore_reset_core1();
    multicore_launch_core1(decoder_entry);
}


void audio_start_playback()
{
    AUD_LOGD("Audio: core0: start playback\n");
    _play_state = PLAY_NORMAL;
    audio_cur_tx_buf = false; // Cause the next DECODER_GET_SAMPLE_BLOCKING to fetch sample in audio_tx_buf1
    multicore_fifo_push_blocking(DECODER_GET_SAMPLE_BLOCKING);
    multicore_fifo_pop_blocking();  // wait decoding thread to unblock when finish receive one buffer
    if (audio_tx_buf1_len > 0)
    {
        // Prepare a ramp-up sample buffer in audio_tx_buf0
        uint32_t first_sample = audio_tx_buf1[0];
        int16_t l = (int16_t)(first_sample >> 16);
        int16_t r = (int16_t)(first_sample & 0xffff);
        register int16_t ll, rr;
        for (register int i = 0; i < AUDIO_MAX_BUFFER_LENGTH; ++i)
        {
            ll = l * i / (AUDIO_MAX_BUFFER_LENGTH - 1);
            rr = r * i / (AUDIO_MAX_BUFFER_LENGTH - 1);
            audio_tx_buf0[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
        }
        audio_tx_buf0_len = AUDIO_MAX_BUFFER_LENGTH;
        wm8978_mute(false);
        // Send rampup buffer
        i2s_send_buffer_blocking(audio_tx_buf0, audio_tx_buf0_len);
        // Start auto sending from audio_tx_buf1
        audio_cur_tx_buf = true;
        i2s_start_playback(i2s_notify_cb, 0);
        // Meanwhile get more samples in audio_tx_buf0
        multicore_fifo_push_blocking(DECODER_GET_SAMPLE);
    }
    else
    {
        // first buffer is 0 length. Just send a silent buffer and let i2s finish on its own
        memset(audio_tx_buf0, 0, AUDIO_MAX_BUFFER_LENGTH * sizeof(uint32_t));
        audio_tx_buf0_len = AUDIO_MAX_BUFFER_LENGTH;
        wm8978_mute(false);
        i2s_start_playback(i2s_notify_cb, 0);
        multicore_fifo_push_blocking(DECODER_GET_SAMPLE);   // This will receive 0 length buffer again and finish
    }
}


void audio_stop_playback()
{
    AUD_LOGD("Audio: core0: stop playback\n");
    _play_state = PLAY_STOPPING;
}


void audio_finish_playback()
{
    // wait decoding join, flush I2S FIFO
    multicore_fifo_pop_blocking();
    AUD_LOGD("Audio: core0: core1 joined\n");
    i2s_stop_playback();
    wm8978_mute(true);
}


void audio_pause_playback()
{
    AUD_LOGD("Audio: core0: pause playback\n");
    if (_play_state == PLAY_NORMAL)
        _play_state = PLAY_RAMPDOWN;
}


void audio_resume_playback()
{
    AUD_LOGD("Audio: core0: resume\n");
    _play_state = PLAY_RAMPUP;
}


void audio_set_speaker_volume(int8_t vol)
{
    if (vol < 0) vol = 0;
    if (vol > 63) vol = 63;
    wm8978_set_volume(WM8978_OUTPUT_SPEAKER, (uint8_t)vol);
}


void audio_set_headphone_volume(int8_t vol)
{
    if (vol < 0) vol = 0;
    if (vol > 63) vol = 63;
    wm8978_set_volume(WM8978_OUTPUT_HEADPHONE, (uint8_t)vol);
}


/**
 * @brief Call this function repeatedly to receive jack detection notification
 * 
 * @param now       Timestamp
 * @return 0 if no change; 1 if headphone plugged; 2 if headphone unplugged;
 */
int audio_jack_detect(uint32_t now)
{
    int ret = 0;
    bool detected = gpio_get(JACK_DETECTION_PIN);   // JACK_DETECTION_PIN is high if inserted
    switch (_jack_state)
    {
    case JACK_EMPTY:
        if (detected)
        {
            _jack_timestamp = now;
            _jack_state = JACK_BOUNCING_IN;
        }
        break;
    case JACK_BOUNCING_IN:
        if (!detected)
        {
            _jack_state = JACK_EMPTY;
        }
        else if (now - _jack_timestamp >= JACK_DEBOUNCE_MS)
        {
            // jack plugged
            AUD_LOGI("Audio: headphone plugged @%d\n", now);
            ret = 1;
            _jack_state = JACK_INSERTED;
            EQ_QUICK_PUSH(EVT_HEADPHONE_PLUGGED);
        }
        break;
    case JACK_INSERTED:
        if (!detected)
        {
            _jack_timestamp = now;
            _jack_state = JACK_BOUNCING_OUT;
        }
        break;
    case JACK_BOUNCING_OUT:
        if (detected)
        {
            _jack_state= JACK_INSERTED;
        }
        else if (now - _jack_timestamp >= JACK_DEBOUNCE_MS)
        {
            // jack unplugged
            AUD_LOGI("Audio: headphone unplugged @%d\n", now);
            ret = 2;
            _jack_state = JACK_EMPTY;
            EQ_QUICK_PUSH(EVT_HEADPHONE_UNPLUGGED);
        }
        break;
    case JACK_NONE:
        // no break
    default:
        break;
    }
    return ret;
}


/**
 * @brief Get jack status
 * @return true if headphone plugged, false if headphone unplugged
 */
bool audio_get_jack_state()
{
    bool r = false;
    switch (_jack_state)
    {
    case JACK_INSERTED:
        // no break
    case JACK_BOUNCING_OUT:
        r = true;
        break;
    case JACK_EMPTY:
        // no break
    case JACK_BOUNCING_IN:
        // no break
    case JACK_NONE:
        // no break
    default:
        break;
    }
    return r;
}


void audio_jack_enable(bool enable)
{
    gpio_put(JACK_OUTEN_PIN, enable);
}
