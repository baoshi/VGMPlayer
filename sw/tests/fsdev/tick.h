#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" 
{
#endif


#ifndef TICK_MAX_EVENTS
#  define TICK_MAX_EVENTS   10
#endif


void tick_init();

typedef void (*tick_hook_t)(void*);
void tick_register_hook(tick_hook_t hook, void* param);

int  tick_arm_time_event(uint32_t timeout_ms, bool repeat, int event, bool start);
void tick_disarm_time_event(int id);
void tick_reset_time_event(int id);
void tick_pause_time_event(int id);
void tick_resume_time_event(int id);

uint32_t tick_millis();

#ifdef __cplusplus
}
#endif


