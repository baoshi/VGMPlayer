#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include "sw_conf.h"
#include "hw_conf.h"
#include "my_debug.h"
#include "my_mem.h"
#include "event_ids.h"
#include "event_queue.h"
#include "tick.h"


int main()
{
    // Calculated using vcocalc.py, set sys clock to 120MHz
    set_sys_clock_pll(1440 * MHZ, 6, 2);

    tick_init();

    event_queue_init(10);
    
    tick_arm_time_event(100, true, EVENT_TOP_POLL);

    stdio_init_all();
    printf("\033[2J\033[H");  // Clear terminal
    stdio_flush();
    MY_MEM_INIT();

    hw_init();

    for (;;)
    {
        event_t evt;
        while (event_queue_pop(&evt))
        {
            switch (evt.code)
            {
            case EVENT_TOP_POLL:
                printf("Poll %d\n", (int)(evt.param));
                break;
            default:
                break;
            }
        }
    }

    return 0;
}