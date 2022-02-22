#pragma once

#ifdef __cplusplus
extern "C" {
#endif


void i2s_init();
void i2s_kickstart();


typedef void (*i2s_notification_cb_t)(void *);

typedef struct i2s_notification_s
{
    i2s_notification_cb_t sample_requested_cb;      // Called after starting transmit one sample buffer
    void* sample_requested_cb_param;                // Use this function to start prepare next buffer
    i2s_notification_cb_t xmit_finished_cb;         // Called after next buffer length is 0 (to indicate finish playing)
    void* xmit_finished_cb_param;                   // Use this function to tell caller playback finished
} i2s_notification_t;

void i2s_setup_notifications(i2s_notification_t* notifications);

#ifdef __cplusplus
}
#endif
