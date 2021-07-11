#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "led.h"
#include "state.h"


void state_mainloop_enter(void)
{
    io_main_power_on();
    led_set(LED_BLINK_OFF);
}


uint8_t state_mainloop_loop(void)
{
    led_update();
    return MAIN_STATE_MAINLOOP;
}


void state_mainloop_exit(void)
{
    led_set(LED_OFF);
}



