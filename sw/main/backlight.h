#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void backlight_init();
void backlight_set(int8_t percentage, uint32_t duration_ms);
void backlight_tick(uint32_t now);
int8_t backlight_get();

#ifdef __cplusplus
}
#endif
