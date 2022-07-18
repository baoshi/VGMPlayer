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
void audio_stop_playback();
void audio_pause_playback();
void audio_unpause_playback();

int  audio_jack_detection(uint32_t now);
bool audio_get_jack_state();
void audio_jack_enable(bool enable);

#ifdef __cplusplus
}
#endif