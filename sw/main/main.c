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
#include "input.h"
#include "splash.h"
#include "path_utils.h"
#include "app.h"

#include "eeprom.h"


void app_ctor(app_t* app)
{
    memset(app, 0, sizeof(app_t));
    hsm_ctor((hsm_t*)app, "app", (event_handler_t)top_handler);
    state_ctor(&(app->browser), "browser", &((hsm_t*)app)->top, (event_handler_t)browser_handler);
        state_ctor(&(app->browser_disk), "browser_disk", &(app->browser), (event_handler_t)browser_disk_handler);
        state_ctor(&(app->browser_nodisk), "browser_nodisk", &(app->browser), (event_handler_t)browser_nodisk_handler);
        state_ctor(&(app->browser_baddisk), "browser_baddisk", &(app->browser), (event_handler_t)browser_baddisk_handler);
    state_ctor(&(app->player), "player", &((hsm_t*)app)->top, (event_handler_t)player_handler);
        state_ctor(&(app->player_s16), "player_s16", &(app->player), (event_handler_t)player_s16_handler);
        state_ctor(&(app->player_vgm), "player_vgm", &(app->player), (event_handler_t)player_vgm_handler);
}


int main()
{
    uint32_t now;
    app_t app;

    // If using memory debugger
    MY_MEM_INIT();
    // main clock, calculated using vcocalc.py, set sys clock to 120MHz
    // set_sys_clock_pll(1440 * MHZ, 6, 2);    // 120MHz
    // tick timer and event queue
    tick_init();
    event_queue_init(10);
    // init console
    stdio_init_all();
    printf("\033[2J\033[H"); // clear terminal
    stdio_flush();
    // load config
    config_load();
    // share bus initialization (i2c)
    hw_shared_resource_init();
    // audio powerup stage 1
    audio_preinit();
    // initialize display
    display_init();
    // initailize input devices
    input_init();
    // draw splash screen without backlight
    splash();
    // Other H/W initialiation interleave with lvgl update to finish the drawing
    hw_shared_resource_init();
    ec_init();
    ec_pause_watchdog();
    disk_init();
    // Turn on backlight
    backlight_init(config.backlight_brigntness_normal, config.backlight_brignthess_dimmed, BACKLIGHT_IDLE_DIM_MS);
    // Manual turn on backlight
    for (int i = 0; i <= config.backlight_brigntness_normal; ++i)    
    {
        backlight_set_direct(i);
        sleep_ms(1);
    }
    // audio powerup stage 2
    audio_postinit();

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
            // Card detection
            if (disk_card_detect(now) > 0)  
            {
                backlight_keepalive(now);
            }
            // Jack detection
            if (audio_jack_detect(now) > 0)
            {
                backlight_keepalive(now);
            }
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
