#pragma once

#include <stdint.h>
#include "audio_buffer.h"
#include "decoder.h"


#ifdef __cplusplus
extern "C" {
#endif


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

extern audio_cbuf_t *audio_buffer;

#ifdef __cplusplus
}
#endif