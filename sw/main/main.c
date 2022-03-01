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
    memset(me, 0, sizeof(app_t));
    // Browser ctx
    path_set_root(me->browser_ctx.cur_dir);
    hsm_ctor((hsm_t*)me, "app", (event_handler_t)app_top);
    state_ctor(&(me->browser), "browser", &((hsm_t*)me)->top, (event_handler_t)browser_handler);
        state_ctor(&(me->browser_disk), "browser_disk", &(me->browser), (event_handler_t)browser_disk_handler);
        state_ctor(&(me->browser_nodisk), "browser_nodisk", &(me->browser), (event_handler_t)browser_nodisk_handler);
        state_ctor(&(me->browser_baddisk), "browser_baddisk", &(me->browser), (event_handler_t)browser_baddisk_handler);
    state_ctor(&(me->player), "player", &((hsm_t*)me)->top, (event_handler_t)player_handler);
        state_ctor(&(me->player_s16), "player_s16", &(me->player), (event_handler_t)player_s16_handler);
            state_ctor(&(me->player_s16_playing), "player_s16_playing", &(me->player_s16), (event_handler_t)player_s16_playing_handler);
            state_ctor(&(me->player_s16_paused), "player_s16_paused", &(me->player_s16), (event_handler_t)player_s16_paused_handler);
    
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
    audio_preinit();
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
            ret = ec_update(now);
            if (ret > 0)
                backlight_keepalive(now);
            else if (ret < 0)
            {
                EQ_QUICK_PUSH(EVT_EC_FAILED);
            }
            // Disk update
            ret = disk_update(now);
            if (ret > 0)
                backlight_keepalive(now);
            // Audio update
            ret = audio_update(now);
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
