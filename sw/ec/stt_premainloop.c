#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "led.h"
#include "state.h"


uint16_t timestamp;

void state_premainloop_enter(void)
{
    io_main_power_on();
    timestamp = systick;
}


uint8_t state_premainloop_loop(void)
{
    if (systick - timestamp > 200)
    {
        led_toggle();
        timestamp = systick;
    }
    return MAIN_STATE_PRE_MAINLOOP;
}


void state_premainloop_exit(void)
{
    
}



