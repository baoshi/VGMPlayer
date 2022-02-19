#include <stdbool.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include "my_debug.h"
#include "hw_conf.h"
#include "tick.h"


#ifndef BACKLIGHT_DEBUG
# define BACKLIGHT_DEBUG 1
#endif


// Debug log
#if BACKLIGHT_DEBUG
#define BL_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define BL_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define BL_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define BL_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define BL_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define BL_LOGD(x, ...)
#define BL_LOGW(x, ...)
#define BL_LOGE(x, ...)
#define BL_LOGI(x, ...)
#define BL_DEBUGF(x, ...)
#endif

static int8_t _normal_brightness, _dimmed_brightness;
static int8_t _current;
static int _start, _target;
static uint32_t _timestamp;
static uint32_t _duration;
static uint32_t _timeout;
static uint32_t _idle_start;
static bool _dimmed;


// Backlight PWM values for 0%-100% brightness
// Use cie1931.py to generate
static const uint16_t CIE[100] = 
{
    0,    1,    2,    3,    5,    6,    7,    8,    9,   10,   12,   13,   15,   16,   18,   20,
   22,   24,   26,   29,   31,   34,   37,   40,   43,   46,   49,   53,   57,   61,   65,   69,
   74,   79,   84,   89,   94,  100,  105,  111,  118,  124,  131,  138,  145,  152,  160,  168,
  176,  184,  193,  202,  211,  220,  230,  240,  250,  261,  272,  283,  295,  306,  319,  331,
  344,  357,  370,  384,  398,  412,  427,  442,  458,  474,  490,  506,  523,  541,  558,  576,
  595,  613,  633,  652,  672,  693,  713,  735,  756,  778,  801,  824,  847,  871,  895,  920,
  945,  970,  997, 1023
};


static void _backlight_set_internal(int8_t percentage, uint32_t duration_ms, uint32_t now)
{
    // Set brightness now or set as target and use backlight_update to change
    if (percentage != _current)
    {
        _target = percentage;
        _duration = duration_ms;
        if (_duration == 0)
        {
            _current = _target;
            MY_ASSERT((_current >= 0) && (_current <= 99));
            pwm_set_chan_level(ST7789_BCKL_PWM_SLICE, ST7789_BCKL_PWM_CHANNEL, CIE[_current]);
            BL_LOGD("BL: %d%%\n", _current);
        }
        else
        {
            _start = _current;
            _timestamp = now;    // _timestamp to track brightness changing
        }
    }
    _idle_start = _timestamp;   // _idle_Start to track idle time
    _dimmed = false;
}


void backlight_init(int8_t normal, int8_t dimmed, uint32_t timeout)
{
    // Backlight PWM pin
    gpio_set_function(ST7789_BCKL_PIN, GPIO_FUNC_PWM);
    // Required Backlight drive frequency 25kHz, with 1024 steps
    // Required PWM clock = 25.6MHz
    // PWM clock divider 120MHz / 25.6MHz = 4.6875
    pwm_set_clkdiv(ST7789_BCKL_PWM_SLICE, 4.6875);
    pwm_set_wrap(ST7789_BCKL_PWM_SLICE, 1023);
    pwm_set_chan_level(ST7789_BCKL_PWM_SLICE, ST7789_BCKL_PWM_CHANNEL, 0);  // Light off
    pwm_set_enabled(ST7789_BCKL_PWM_SLICE, true);
    _normal_brightness = normal;
    _dimmed_brightness = dimmed;
    _timeout = timeout;
    _current = 0;
    _target = 0;
    _start = 0;
    _timestamp = 0;
    _duration = 0;
}

    
void backlight_set(int8_t percentage, uint32_t duration_ms)
{
    if (percentage < 0) percentage = 0;
    if (percentage > 99) percentage = 99;
    // set brightness overrides normal brightness
    _normal_brightness = percentage;
    // dimmed brightness cannot exceed set brignthess
    if (percentage < _dimmed_brightness)
        _dimmed_brightness = percentage;
    _backlight_set_internal(percentage, duration_ms, tick_millis());
}


void backlight_set_direct(int8_t percentage)
{
    _backlight_set_internal(percentage, 0, tick_millis());
}


void backlight_keepalive(uint32_t now)
{
    _idle_start = now;
    if (_dimmed)
    {
        // retsore normal brightness
        BL_LOGD("Restore @ %d\n", now);
        _backlight_set_internal(_normal_brightness, 100, now);
        _dimmed = false;
    }
}


void backlight_update(uint32_t now)
{
    if ((!_dimmed) && (now - _idle_start >= _timeout))
    {
        // idle out, dim light
        BL_LOGD("Dim @ %d\n", now);
        _backlight_set_internal(_dimmed_brightness, 100, now);
        _dimmed = true;
    }
    if ((_duration != 0) && (_target != _current))
    {
        int t;
        if (_start < _target)
        {
            t = _start + (_target - _start) * (now - _timestamp) / _duration;
            if (t > _target) t = _target;
            _current = t;
            MY_ASSERT((_current >= 0) && (_current <= 99));
            pwm_set_chan_level(ST7789_BCKL_PWM_SLICE, ST7789_BCKL_PWM_CHANNEL, CIE[_current]);
            BL_LOGD("BL: %d%% @ %d\n", _current, now);
        }
        else if (_start > _target)
        {
            t = _start - (_start - _target) * (now - _timestamp) / _duration;
            if (t < _target) t = _target;
            _current = t;
            MY_ASSERT((_current >= 0) && (_current <= 99));
            pwm_set_chan_level(ST7789_BCKL_PWM_SLICE, ST7789_BCKL_PWM_CHANNEL, CIE[_current]);
            BL_LOGD("BL: %d%% @ %d\n", _current, now);
        }
    }
}


int8_t backlight_get()
{
    return _current;
}
