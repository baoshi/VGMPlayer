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


// Application level state machione
typedef struct app_s
{
    hsm_t super;
} app_t;


event_t const *app_top(app_t *me, event_t const *evt)
{
    event_t const *r = evt;
    switch (evt->code)
    {
    case EVT_ENTRY:
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
    uint32_t now;
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

    // initialize display
    tick_register_hook(display_tick_hook, (void *)TICK_GRANULARITY_MS); // LVGL timer
    display_init();
    // draw splash screen without backlight
    splash();
    // Other H/W initialiation interleave with lvgl update to finish the drawing
    hw_init();
    lv_timer_handler();
    ec_init();
    int last_ec = ec_read_raw0();
    // Some more time to finish splash
    for (int i = 0; i < 10; ++i)
    {
        lv_timer_handler();
        sleep_ms(5);
    }
    // Turn on backlight
    backlight_init(backlight_brigntness_normal, backlight_brignthess_dimmed, BACKLIGHT_IDLE_DIM_MS);
    // Manaul turn on backlight
    for (int i = 0; i <= backlight_brigntness_normal; ++i)    
    {
        backlight_set_direct(i);
        lv_timer_handler();
        sleep_ms(1);
    }
    
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
        now = tick_millis();
        if (now - last_update_tick >= SUPERLOOP_UPDATE_INTERVAL_MS)
        {
            ec_update(now);
            // if ec reading changes, restart backlight idle count
            uint8_t ec = ec_read_raw0();
            if (ec != last_ec)
            {
                backlight_keepalive(now);
                last_ec = ec;
            }
            last_update_tick = now;
        }
        backlight_update(now);
        lv_timer_handler();

        // HSM event loop
        event_t evt;
        while (event_queue_pop(&evt))
        {
            hsm_on_event((hsm_t*)&app, &evt);
        }

        
    }

    return 0;
}
