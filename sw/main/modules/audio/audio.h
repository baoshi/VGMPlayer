#pragma once

#include <stdint.h>
#include "decoder.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct _audio_playback_ctx
{
    decoder_t *decoder;
} audio_playback_ctx;

void audio_preinit();
void audio_postinit();
void audio_close();

audio_playback_ctx * audio_setup_playback(decoder_t *decoder);
void audio_playback(audio_playback_ctx *ctx);

int  audio_update(uint32_t now);

#ifdef __cplusplus
}
#endif