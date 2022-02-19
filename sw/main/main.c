#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include <lvgl.h>
#include "sw_conf.h"
#include "hw_conf.h"
#include "my_debug.h"
#include "my_mem.h"
#include "event_queue.h"
#include "hsm.h"
#include "event_ids.h"
#include "tick.h"
#include "backlight.h"
#include "ec.h"
#include "display.h"
#include "splash.h"


#define APP_TICK_INTERVAL       10
#define APP_BACKLIGHT_DIM_MS    10000

// Application level state machione
typedef struct app_s
{
    hsm_t super;
    // app members
    int         app_bl_alarm;
    int8_t      app_bl_normal_brightness;
    int8_t      app_bl_dim_brightness;
    uint8_t     app_ec_status;
} app_t;


event_t const *app_top(app_t *me, event_t const *evt)
{
    event_t const *r = evt;
    switch (evt->code)
    {
    case EVT_ENTRY:
        me->app_bl_alarm = -1;
        r = 0;
        break;
    case EVT_START:
        hw_init();
        ec_init();
        tick_arm_time_event(APP_TICK_INTERVAL, true, EVT_APP_TICK, true);
        me->app_bl_alarm = tick_arm_time_event(APP_BACKLIGHT_DIM_MS, true, EVT_APP_DIM_SCREEN, true);
        me->app_ec_status = ec_read_raw0();
        r = 0;
        break;
    case EVT_APP_TICK:
    {
        uint32_t now = (uint32_t)(evt->param);
        ec_update(now);
        uint8_t ec = ec_read_raw0();
        if (ec != me->app_ec_status)
        {
            me->app_ec_status = ec;
            tick_reset_time_event(me->app_bl_alarm);
            backlight_set(me->app_bl_normal_brightness, 100);
        }
        backlight_update(now);
        r = 0;
        break;
    }
    case EVT_APP_DIM_SCREEN:
        tick_pause_time_event(me->app_bl_alarm);
        backlight_set(me->app_bl_dim_brightness, 100);
        r = 0;
        break;
    }
    return r;
}


void app_ctor(app_t* me)
{
    hsm_ctor((hsm_t*)me, "app", (event_handler_t)app_top);
}


int main()
{
    app_t app;

    // main clock, calculated using vcocalc.py, set sys clock to 120MHz
    set_sys_clock_pll(1440 * MHZ, 6, 2);

    stdio_init_all();
    printf("\033[2J\033[H"); // clear terminal
    stdio_flush();

    // tick timer and event queue
    tick_init();
    event_queue_init(10);

    // in case using memory debugger
    MY_MEM_INIT();

    // initialize display is before everything else so error message can be displayed
    tick_register_hook(display_tick_hook, (void *)APP_TICK_PERIOD_MS);
    display_init();
    // display a splash screen and turn on backlight
    splash();
    backlight_init();
    int8_t brightness = 70;
    for (int i = 0; i < 10; ++i)
    {
        lv_timer_handler();
        sleep_ms(5);
    }
    for (int8_t b = 0; b <= brightness; ++b)
    {
        backlight_set(b, 0);
        lv_timer_handler();
        sleep_ms(5);
    }

    // initialize state machine
    app.app_bl_normal_brightness = brightness;
    app.app_bl_dim_brightness = 30;
    app_ctor(&app);
    hsm_on_start((hsm_t*)&app);

    for (;;)
    {
        event_t evt;
        while (event_queue_pop(&evt))
        {
            hsm_on_event((hsm_t*)&app, &evt);
        }
    }

    return 0;
}
