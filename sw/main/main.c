#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "path_utils.h"
#include "app.h"



event_t const *top_handler(app_t *me, event_t const *evt)
{
    event_t const *r = evt;
    switch (evt->code)
    {
        case EVT_START:
        {
            // initial state is browser
            STATE_START(me, &me->browser);
            r = 0;
            break;
        }
    }
    return r;
}


void app_ctor(app_t* me)
{
    memset(me, 0, sizeof(app_t));
    hsm_ctor((hsm_t*)me, "app", (event_handler_t)top_handler);
    state_ctor(&(me->browser), "browser", &((hsm_t*)me)->top, (event_handler_t)browser_handler);
        state_ctor(&(me->browser_disk), "browser_disk", &(me->browser), (event_handler_t)browser_disk_handler);
        state_ctor(&(me->browser_nodisk), "browser_nodisk", &(me->browser), (event_handler_t)browser_nodisk_handler);
        state_ctor(&(me->browser_baddisk), "browser_baddisk", &(me->browser), (event_handler_t)browser_baddisk_handler);
    state_ctor(&(me->player), "player", &((hsm_t*)me)->top, (event_handler_t)player_handler);
        state_ctor(&(me->player_exp), "player_exp", &(me->player), (event_handler_t)player_exp_handler);
        state_ctor(&(me->player_volume), "player_volume", &(me->player), (event_handler_t)player_volume_handler);
        state_ctor(&(me->player_visual), "player_visual", &(me->player), (event_handler_t)player_visual_handler);
    // settings_xxx is substates of settings. settings state will be dynamically attached using settings_popup() call
    state_ctor(&(me->settings_brightness), "settings_brightness", &(me->settings), (event_handler_t)settings_brightness_handler);
    state_ctor(&(me->settings_volume), "settings_volume", &(me->settings), (event_handler_t)settings_volume_handler);
}


int main()
{
    uint32_t now;
    app_t app;

    // If using memory debugger
    MY_MEM_INIT();
    // main clock, calculated using vcocalc.py, set sys clock to 120MHz
    set_sys_clock_pll(1440 * MHZ, 6, 2);
    // tick timer and event queue
    tick_init();
    event_queue_init(10);
    // init console
    stdio_init_all();
    printf("\033[2J\033[H"); // clear terminal
    stdio_flush();
    // share bus initialization (i2c)
    hw_shared_resource_init();
    // audio powerup stage 1
    audio_preinit();
    // initialize display
    display_init();     // 323ms
    // draw splash screen without backlight
    splash();
    // Other H/W initialiation interleave with lvgl update to finish the drawing
    hw_shared_resource_init();
    ec_init();
    ec_pause_watchdog();
    disk_init();
    // Turn on backlight
    backlight_init(backlight_brigntness_normal, backlight_brignthess_dimmed, BACKLIGHT_IDLE_DIM_MS);
    // Manual turn on backlight
    for (int i = 0; i <= backlight_brigntness_normal; ++i)    
    {
        backlight_set_direct(i);
        sleep_ms(1);
    }
    // audio powerup stage 2 to be postponed before start play
    //audio_postinit();

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
            ret = ec_update(now); // return 1 if io changed
            if (ret > 0)
            {
                backlight_keepalive(now);
            }
            else if (ret < 0)
            {
                EQ_QUICK_PUSH(EVT_EC_FAILED);
            }
            // Disk update
            ret = disk_update(now); // return card inserted (1) / ejected (2)
            if (ret > 0)
                backlight_keepalive(now);
            // Audio update
            ret = audio_update(now); // return earpiece plugged (1) / unplugged (2)
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
