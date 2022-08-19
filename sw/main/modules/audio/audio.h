#pragma once

#include <stdint.h>
#include "decoder.h"


#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_MAX_BUFFER_LENGTH 1024

void audio_preinit();
void audio_postinit();
void audio_close();

void audio_setup_playback(decoder_t *decoder);
void audio_start_playback();
void audio_pause_playback();
void audio_resume_playback();
void audio_stop_playback();
void audio_finish_playback();
void audio_set_speaker_volume(int8_t vol);
void audio_set_headphone_volume(int8_t vol);

int  audio_jack_detect(uint32_t now);
bool audio_get_jack_state();
void audio_jack_enable(bool enable);

extern uint32_t  audio_tx_buf0[];
extern volatile int32_t audio_tx_buf0_len;
extern uint32_t  audio_tx_buf1[];
extern volatile int32_t audio_tx_buf1_len;
extern volatile bool audio_cur_tx_buf; 

#ifdef __cplusplus
}
#endif