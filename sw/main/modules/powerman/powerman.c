#include <stdbool.h>
#include "my_debug.h"
#include "hw_conf.h"
#include "sw_conf.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "backlight.h"


#ifndef PM_DEBUG
# define PM_DEBUG 1
#endif


// Debug log
#if PM_DEBUG
#define PM_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define PM_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define PM_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define PM_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define PM_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define PM_LOGD(x, ...)
#define PM_LOGW(x, ...)
#define PM_LOGE(x, ...)
#define PM_LOGI(x, ...)
#define PM_DEBUGF(x, ...)
#endif


static uint32_t _idle_start;
static bool _backlight_dimmed;
static bool _autooff_sent;


void powerman_init()
{
    uint32_t now = tick_millis(); 
    _idle_start = now;
    _backlight_dimmed = false;
    _autooff_sent = false;
    PM_LOGI("PM: Start @ %d\n", now);
}


void powerman_keepalive(uint32_t now)
{
    _idle_start = now;
    if (_backlight_dimmed)
    {
        // retsore normal brightness
        PM_LOGI("PM: Restore backlight @ %d\n", now);
        backlight_restore();
        _backlight_dimmed = false;
    }
}


void powerman_update(uint32_t now)
{
    // Dim backlight if idle more than POWERMAN_BACKLIGHT_DIM_MS
    if ((!_backlight_dimmed) && (now - _idle_start >= POWERMAN_BACKLIGHT_DIM_MS))
    {
        PM_LOGI("PM: Dim backlight @ %d\n", now);
        backlight_dim();
        _backlight_dimmed = true;
    }
    // Auto power-off
    if ((!_autooff_sent) && (now - _idle_start >= POWERMAN_AUDO_OFF_MS))
    {
        PM_LOGI("PM: Auto off @ %d\n", now);
        EQ_QUICK_PUSH(EVT_APP_AUTO_OFF);
        _autooff_sent = true;   // auto-off is sent only once
    }
}
