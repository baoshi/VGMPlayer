#include <string.h>
#include <hardware/structs/bus_ctrl.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>
#include "hw_conf.h"
#include "sw_conf.h"
#include "my_debug.h"
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


// Playback states
typedef enum
{
    PLAY_NONE = 0,
    PLAY_NORMAL,
    PLAY_SILENCE,
    PLAY_GAP,
    PLAY_GAP_PAUSED
} playback_state_t;


// command send to playback_proc
enum
{
    CMD_NONE = 0,
    CMD_STOP,
    CMD_PAUSE,
    CMD_RESUME
};


// context used during playback, shared by audio_xxx and playback_proc on core 1
typedef struct playback_ctx_s
{
    decoder_t *decoder;         // decoder interface
    playback_state_t state;     // playing state
    queue_t cmd_queue;          // for command byte send from audio_xxx to playback_proc
} playback_ctx_t;
static playback_ctx_t __audio_ram("audio") _playback;


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
    audio_mem_init();
    audio_cbuf_init();
    memset(&_playback, 0, sizeof(playback_ctx_t));
    queue_init(&(_playback.cmd_queue), sizeof(uint8_t), 2);
    // Disable jack output at beginning
    gpio_init(JACK_OUTEN_PIN);
    gpio_put(JACK_OUTEN_PIN, false);
    gpio_set_dir(JACK_OUTEN_PIN, GPIO_OUT);
    // Initailize WM8978
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
    _jack_state = JACK_NONE;
}


static void playback_proc()
{
    AUD_LOGD("Audio: core1: playback entry\n");
    decoder_t *decoder = _playback.decoder;
    uint32_t sample, num_samples;
    register int16_t l, ll, r, rr;
    register int x, i;
    bool finished = false;
    uint8_t cmd;
    // number of silence buffer to send before finish the song
    int gaps = AUDIO_SONG_GAP_MS * AUDIO_SAMPLE_RATE / AUDIO_FRAME_LENGTH / 1000;

    audio_cbuf_reset();
    while (queue_try_remove(&(_playback.cmd_queue), &cmd)) { tight_loop_contents(); };   // empty command queue
    _playback.state = PLAY_NORMAL;
    // first audio buffer is a ramp up from 0 to first sound sample
    num_samples = decoder->get_samples(decoder, &sample, 1);
    if (num_samples == 0) sample = 0;   // no sample from decoder, just make a buffer of 0 and let playing finish by itself
    audio_frame_t *frame = audio_cbuf_get_write_buffer();
    MY_ASSERT(frame != NULL);
    l = (int16_t)(sample >> 16);
    r = (int16_t)(sample & 0xffff);
    for (int i = 0; i < AUDIO_FRAME_LENGTH - 1; ++i)
    {
        ll = l * i / (AUDIO_FRAME_LENGTH - 2);
        rr = r * i / (AUDIO_FRAME_LENGTH - 2);
        frame->data[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
    }
    frame->data[AUDIO_FRAME_LENGTH - 1] = sample;
    frame->length = AUDIO_FRAME_LENGTH;
    audio_cbuf_finish_write();
    // fill remaining buffer with actual samples
    while (1)
    {
        frame = audio_cbuf_get_write_buffer();
        if (NULL == frame) break;
        num_samples = decoder->get_samples(decoder, frame->data, AUDIO_FRAME_LENGTH);
        frame->length = num_samples;
        audio_cbuf_finish_write();
        if (0 == num_samples) break;
    }
    AUD_LOGD("Audio: core1: init I2S\n");
    i2s_init();
    wm8978_mute(false);
    i2s_start_playback();
    // play loop
    AUD_LOGD("Audio: core1: enter play loop\n");
    while (!finished)
    {
        audio_frame_t *frame = audio_cbuf_get_write_buffer();
        if (NULL == frame)  // audio circular buffer full, wait.
        {
            tight_loop_contents();
            continue;    
        }
        // we have empty frame now, see if we received any command
        if (!queue_try_remove(&(_playback.cmd_queue), (void *)&cmd))
            cmd = CMD_NONE;
        switch (_playback.state)
        {

        case PLAY_NORMAL:
            // PLAY_NORMAL:
            // cmd == CMD_NONE   :  Continue generate sample buffer, if no more buffer, send ramp down then enter PLAY_GAP
            // cmd == CMD_STOP   :  Generate 0 length buffer and set finished to true
            // cmd == CMD_PAUSE  :  Generate ramp down buffer and set state to PLAY_SILENCE
            // cmd == CMD_RESUME :  Do nothing
            switch (cmd)
            {
            case CMD_NONE:
                num_samples = decoder->get_samples(decoder, frame->data, AUDIO_FRAME_LENGTH);
                frame->length = num_samples;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                if (num_samples != 0)
                {
                    sample = frame->data[num_samples - 1];  // save last sample for ramp use
                } 
                else
                {
                    // no more samples from decoder, send ramp down then enter PLAY_GAP
                    AUD_LOGD("Audio: core1: decoding finished, sending gap\n");
                    l = (int16_t)(sample >> 16);
                    r = (int16_t)(sample & 0xffff);
                    for (i = 0; i < AUDIO_FRAME_LENGTH; ++i)
                    {
                        x = AUDIO_FRAME_LENGTH - 1 - i;
                        ll = l * x / (AUDIO_FRAME_LENGTH - 1);
                        rr = r * x / (AUDIO_FRAME_LENGTH - 1);
                        frame->data[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
                    }
                    frame->length = AUDIO_FRAME_LENGTH;
                    audio_cbuf_finish_write();
                    if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                        i2s_resume_playback();
                    --gaps;
                    _playback.state = PLAY_GAP;
                    break;
                }
                break;
            case CMD_STOP:
                AUD_LOGD("Audio: core1: CMD_STOP\n");
                frame->length = 0;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                finished = true;
                break;
            case CMD_PAUSE:
                AUD_LOGD("Audio: core1: CMD_PAUSE\n");
                l = (int16_t)(sample >> 16);
                r = (int16_t)(sample & 0xffff);
                for (i = 0; i < AUDIO_FRAME_LENGTH; ++i)
                {
                    x = AUDIO_FRAME_LENGTH - 1 - i;
                    ll = l * x / (AUDIO_FRAME_LENGTH - 1);
                    rr = r * x / (AUDIO_FRAME_LENGTH - 1);
                    frame->data[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
                }
                frame->length = AUDIO_FRAME_LENGTH;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                _playback.state = PLAY_SILENCE;
                break;
            }
            break;

        case PLAY_SILENCE:
            // PLAY_SILENCE:
            // cmd == CMD_NONE   :  Continue generate silence
            // cmd == CMD_STOP   :  Generate 0 length sample and set finished to true
            // cmd == CMD_PAUSE  :  Do nothing
            // cmd == CMD_RESUME :  Generate ramp up buffer and set state to PLAY_NORMAL
            switch (cmd)
            {
            case CMD_NONE:
                memset(frame->data, 0, AUDIO_FRAME_LENGTH * sizeof(uint32_t));
                frame->length = AUDIO_FRAME_LENGTH;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                break;
            case CMD_STOP:
                AUD_LOGD("Audio: core1: CMD_STOP\n");
                frame->length = 0;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                finished = true;
                break;
            case CMD_RESUME:
                l = (int16_t)(sample >> 16);
                r = (int16_t)(sample & 0xffff);
                for (i = 0; i < AUDIO_FRAME_LENGTH; ++i)
                {
                    ll = l * i / (AUDIO_FRAME_LENGTH - 1);
                    rr = r * i / (AUDIO_FRAME_LENGTH - 1);
                    frame->data[i] = ((uint32_t)ll) << 16 | (uint16_t)rr;
                }
                frame->length = AUDIO_FRAME_LENGTH;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                _playback.state = PLAY_NORMAL;
                break;
            }
            break;

        case PLAY_GAP:
            // PLAY_GAP:
            // cmd == CMD_NONE   :  Continue generate silence and decrease gap. If gap==0 send 0 length buffer and set finished to true
            // cmd == CMD_STOP   :  Generate 0 length sample and set finished to true
            // cmd == CMD_PAUSE  :  Generate silence and enter PLAY_GAP_PAUSED
            // cmd == CMD_RESUME :  Do nothing
            switch (cmd)
            {
            case CMD_NONE:
                if (gaps > 0)
                {
                    memset(frame->data, 0, AUDIO_FRAME_LENGTH * sizeof(uint32_t));
                    frame->length = AUDIO_FRAME_LENGTH;
                    audio_cbuf_finish_write();
                    if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                        i2s_resume_playback();
                    --gaps;
                }
                else
                {
                    AUD_LOGD("Audio: core1: gap finished\n");
                    frame->length = 0;
                    audio_cbuf_finish_write();
                    if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                        i2s_resume_playback();
                    finished = true;
                }
                break;
            case CMD_STOP:
                AUD_LOGD("Audio: core1: CMD_STOP\n");
                frame->length = 0;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                finished = true;
                break;
            case CMD_PAUSE:
                memset(frame->data, 0, AUDIO_FRAME_LENGTH * sizeof(uint32_t));
                frame->length = AUDIO_FRAME_LENGTH;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                --gaps;
                _playback.state = PLAY_GAP_PAUSED;
                break;
            }
            break;

        case PLAY_GAP_PAUSED:
            // PLAY_GAP:
            // cmd == CMD_NONE   :  Continue generate silence
            // cmd == CMD_STOP   :  Generate 0 length sample and set finished to true
            // cmd == CMD_PAUSE  :  Do nothing
            // cmd == CMD_RESUME :  Generate silence and enter PLAY_GAP
            switch (cmd)
            {
            case CMD_NONE:
                memset(frame->data, 0, AUDIO_FRAME_LENGTH * sizeof(uint32_t));
                frame->length = AUDIO_FRAME_LENGTH;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                break;
            case CMD_STOP:
                AUD_LOGD("Audio: core1: CMD_STOP\n");
                frame->length = 0;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                finished = true;
                break;
            case CMD_RESUME:
                memset(frame->data, 0, AUDIO_FRAME_LENGTH * sizeof(uint32_t));
                frame->length = AUDIO_FRAME_LENGTH;
                audio_cbuf_finish_write();
                if (i2s_buffer_underrun)    // If I2S buffer underrun occurred, resume sending
                    i2s_resume_playback();
                --gaps;
                _playback.state = PLAY_GAP;
                break;
            }
            break;

        }   // switch (_playback.state)
    }   // while (!finished)
    i2s_stop_playback();
    i2s_deinit();
    wm8978_mute(true);
    if (CMD_STOP == cmd)    // if we reach here because CMD_STOP was received
        EQ_QUICK_PUSH(EVT_AUDIO_SONG_TERMINATED);
    else
        EQ_QUICK_PUSH(EVT_AUDIO_SONG_FINISHED);
    AUD_LOGD("Audio: core1: playback exit\n");
}

/*

// decoder thread function running on core 1
void decoder_entry()
{
    uint32_t ints = save_and_disable_interrupts();
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
    restore_interrupts(ints);
}
*/


void audio_start_playback(decoder_t *decoder)
{
    uint8_t cmd;
    AUD_LOGD("Audio: core0: setup\n");
    _playback.decoder = decoder;
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_PROC1_BITS;        // bring up core1 bus priority
    multicore_reset_core1();
    multicore_launch_core1(playback_proc);
}


void audio_stop_playback()
{
    uint8_t cmd = CMD_STOP;
    queue_add_blocking(&(_playback.cmd_queue), (void *)&cmd);
    AUD_LOGD("Audio: core0: stop playback\n");
}


void audio_finish_playback()
{
    uint8_t cmd;
    multicore_reset_core1();
    _playback.decoder = NULL;
    _playback.state = PLAY_NONE;
    while (queue_try_remove(&(_playback.cmd_queue), (void *)&cmd)) {};   // empty command queue
}


void audio_pause_playback()
{
    uint8_t cmd = CMD_PAUSE;
    queue_add_blocking(&(_playback.cmd_queue), (void *)&cmd);
    AUD_LOGD("Audio: core0: pause playback\n");
}


void audio_resume_playback()
{
    uint8_t cmd = CMD_RESUME;
    queue_add_blocking(&(_playback.cmd_queue), (void *)&cmd);
    AUD_LOGD("Audio: core0: resume\n");

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
