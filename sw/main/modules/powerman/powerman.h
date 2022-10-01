#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void powerman_init();
void powerman_keepalive(uint32_t now);
void powerman_update(uint32_t now);

#ifdef __cplusplus
}
#endif
