#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void backlight_init(int8_t normal, int8_t dimmed, uint32_t timeout);
void backlight_set(int8_t percentage, uint32_t duration_ms);
void backlight_set_direct(int8_t percentage);
void backlight_keepalive(uint32_t now);
void backlight_update(uint32_t now);

int8_t backlight_get();

#ifdef __cplusplus
}
#endif
