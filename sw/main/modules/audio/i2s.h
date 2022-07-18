#pragma once

#ifdef __cplusplus
extern "C" {
#endif




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
