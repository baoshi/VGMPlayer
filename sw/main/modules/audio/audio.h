#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void audio_init();
void audio_powerup_stage1();
void audio_powerup_stage2();
void audio_powerdown();
int audio_update(uint32_t now);

#ifdef __cplusplus
}
#endif