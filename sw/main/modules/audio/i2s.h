#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/*
 * To use I2S module:
 * 1. Declare these variables:
 *    uint32_t  audio_tx_buf0[];      // buffer 0
 *    uint32_t  audio_tx_buf0_len;    // buffer 0 data length
 *    uint32_t  audio_tx_buf1[];      // buffer 1
 *    uint32_t  audio_tx_buf1_len;    // buffer 1 data length
 *    bool      audio_cur_tx_buf;     // current buffer buf0:false, buf1:true
 * 2. Fill audio_tx_buf0/audio_tx_buf0_len or audio_tx_buf1/audio_tx_buf1_len and set audio_cur_tx_buf accordingly
 * 3. Write i2s_notify_cb_t callback function, in the callback, expect notification to be
 *    I2S_NOTIFY_SAMPLE_REQUESTED 
 *      => Current buffer is sent to I2S FIFO, provide new data in the other buffer or set new buffer length to 0 to finish
 *         Note RP2040 FIFO is 4x32bit. This function must finish within 128bits sending time.
 *    I2S_NOTIFY_PLAYBACK_FINISHING
 *      => When receive this notification, last data in the buffer is sent to I2S FIFO but output is not finished yet.
 *         Call i2s_stop_playback() to gaurantee buffer are sent
 */

enum 
{
    I2S_NOTIFY_SAMPLE_REQUESTED = 1,
    I2S_NOTIFY_PLAYBACK_FINISHING = 2
};
typedef void (*i2s_notify_cb_t)(int notification, void *);

void i2s_init();
void i2s_deinit();
void i2s_start_playback(i2s_notify_cb_t callback, void *param);
void i2s_stop_playback();

void i2s_send_buffer_blocking(uint32_t *buf, uint32_t len);


#ifdef __cplusplus
}
#endif
