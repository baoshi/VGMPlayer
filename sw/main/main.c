#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
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


#define APP_TICK_INTERVAL   10

// Application level state machione
typedef struct app_s
{
    hsm_t super;
    // app members
    int app_alarm_tick;
    int app_alarm_bl_dimmer;
    uint32_t app_bl_dimmer_counter;
} app_t;


event_t const *app_top(app_t *me, event_t const *evt)
{
    event_t const *r = evt;
    switch (evt->code)
    {
    case EVT_ENTRY:
        me->app_alarm_tick = -1;
        me->app_alarm_bl_dimmer = -1;
        me->app_bl_dimmer_counter = 0;
        r = 0;
        break;
    case EVT_START:
        hw_init();
        backlight_init();
        me->app_alarm_tick = tick_arm_time_event(APP_TICK_INTERVAL, true, EVT_APP_TICK);
        backlight_set(70, 2000);
        r = 0;
        break;
    case EVT_APP_TICK:
        {
            uint32_t now = (uint32_t)(evt->param);
            backlight_tick(now);
        }
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

    // initialize state machine
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
