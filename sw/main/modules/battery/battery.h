#pragma once

#ifdef __cpluscplus
extern "C" {
#endif

typedef enum 
{
    BATTERY_STATE_NONE,
    BATTERY_STATE_UNKNOWN,
    BATTERY_STATE_CHARGING,
    BATTERY_STATE_100,
    BATTERY_STATE_75,
    BATTERY_STATE_50,
    BATTERY_STATE_25,
    BATTERY_STATE_0,
} battery_state_t;

battery_state_t battery_get_state();

#ifdef __cplusplus
}
#endif