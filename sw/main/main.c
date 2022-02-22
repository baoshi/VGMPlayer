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
#include "event_ids.h"
#include "event_queue.h"
#include "hsm.h"
#include "tick.h"
#include "backlight.h"
#include "ec.h"
#include "display.h"
#include "disk.h"
#include "audio.h"
#include "splash.h"
#include "app.h"


event_t const *app_top(app_t *me, event_t const *evt)
{
    event_t const *r = evt;
    switch (evt->code)
    {
    case EVT_START:
        STATE_START(me, &me->browser);
        r = 0;
        break;
    }
    return r;
}


void app_ctor(app_t* me)
{
    hsm_ctor((hsm_t*)me, "app", (event_handler_t)app_top);
    state_ctor(&(me->browser), "browser", &((hsm_t*)me)->top, (event_handler_t)browser_handler);
        state_ctor(&(me->browser_disk), "browser_disk", &(me->browser), (event_handler_t)browser_disk_handler);
        state_ctor(&(me->browser_nodisk), "browser_nodisk", &(me->browser), (event_handler_t)browser_nodisk_handler);
        state_ctor(&(me->browser_baddisk), "browser_baddisk", &(me->browser), (event_handler_t)browser_baddisk_handler);
}


int main()
{
    uint32_t now;
    app_t app;

    // main clock, calculated using vcocalc.py, set sys clock to 120MHz
    set_sys_clock_pll(1440 * MHZ, 6, 2);

    // tick timer and event queue
    tick_init();
    event_queue_init(10);
    // init console
    stdio_init_all();
    printf("\033[2J\033[H"); // clear terminal
    stdio_flush();
    // in case using memory debugger
    MY_MEM_INIT();
    // share bus initialization (i2c)
    hw_shared_resource_init();
    // audio powerup stage 1
    audio_powerdown();
    sleep_ms(500);
    now = tick_millis();
    audio_powerup_stage1();
    // initialize display
    display_init();     // 323ms
    // draw splash screen without backlight
    splash();
    // Some more time to finish render splash screen
    lv_timer_handler();
    sleep_ms(5);
    lv_timer_handler();
    sleep_ms(5);
    lv_timer_handler(); // 63ms
    // Other H/W initialiation interleave with lvgl update to finish the drawing
    hw_shared_resource_init();
    ec_init();
    disk_init();
    // Turn on backlight
    backlight_init(backlight_brigntness_normal, backlight_brignthess_dimmed, BACKLIGHT_IDLE_DIM_MS);
    // Manaul turn on backlight
    for (int i = 0; i <= backlight_brigntness_normal; ++i)    
    {
        backlight_set_direct(i);
        sleep_ms(1);
    }
    // audio powerup stage 2
    printf("since power1: %d\n", tick_millis() - now);
    audio_powerup_stage2();

    // initialize state machine
    app_ctor(&app);
    hsm_on_start((hsm_t*)&app);

    // loop timing and control flags
    now = tick_millis();
    backlight_keepalive(now);
    uint32_t last_update_tick = now;
    
    // Super Loop
    for (;;)
    {
        int ret;
        event_t evt;
        now = tick_millis();
        if (now - last_update_tick >= SUPERLOOP_UPDATE_INTERVAL_MS)
        {
            // EC update
            ret = ec_update(now);
            if (ret > 0)
                backlight_keepalive(now);
            else if (ret < 0)
            {
                evt.code = EVT_EC_FAILED;
                evt.param = 0;
                event_queue_push_back(&evt);
            }
            // Disk update
            ret = disk_update(now);
            if (ret > 0)
                backlight_keepalive(now);
            // Backlight update
            backlight_update(now);
            // LVGL update
            lv_timer_handler();
            last_update_tick = now;
        }

        // HSM event loop
        while (event_queue_pop(&evt))
        {
            hsm_on_event((hsm_t*)&app, &evt);
        }
    }

    return 0;
}
