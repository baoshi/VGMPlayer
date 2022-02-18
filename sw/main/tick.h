#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" 
{
#endif

#ifndef TICK_PERIOD_MS
#  define TICK_PERIOD_MS    1
#endif


#ifndef TICK_MAX_EVENTS
#  define TICK_MAX_EVENTS   10
#endif


void tick_init();

typedef void (*tick_hook_t)(void*);
void tick_register_hook(tick_hook_t hook, void* param);

int  tick_arm_time_event(uint32_t timeout_ms, bool repeat, int event);
void tick_disarm_time_event(int id);

#ifdef __cplusplus
}
#endif


