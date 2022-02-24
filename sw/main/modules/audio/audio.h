#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void audio_preinit();
void audio_postinit();
void audio_powerdown();
int audio_update(uint32_t now);

#ifdef __cplusplus
}
#endif