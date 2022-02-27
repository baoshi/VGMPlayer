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
void audio_playback();

int  audio_update(uint32_t now);

#ifdef __cplusplus
}
#endif