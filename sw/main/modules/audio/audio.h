#pragma once

#include <stdint.h>
#include "decoder.h"


#ifdef __cplusplus
extern "C" {
#endif


void audio_preinit();
void audio_postinit();
void audio_close();

void audio_setup_playback(decoder_t *decoder);
void audio_start_playback();
void audio_stop_playback();
void audio_pause_playback();
void audio_unpause_playback();

int  audio_update(uint32_t now);

#ifdef __cplusplus
}
#endif