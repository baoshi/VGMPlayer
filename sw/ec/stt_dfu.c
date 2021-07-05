#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "led.h"
#include "state.h"


void state_dfu_enter(void)
{
    led_set(LED_BLINK_DFU);
}


uint8_t state_dfu_loop(void)
{
    led_update();
    return MAIN_STATE_DFU;
}


void state_dfu_exit(void)
{
    led_set(LED_OFF);
}




