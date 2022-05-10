#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "io.h"
#include "led.h"
#include "state.h"


void state_premain_entry(void)
{
    led_set(LED_ON);
}


uint8_t state_premain_update(void)
{
    uint8_t r = MAIN_STATE_PRE_MAIN;
    io_debounce_inputs();
    // If all button released, enter MAIN
    if ((io_input_state & IO_STATUS_MASK_BUTTONS) == IO_STATUS_MASK_BUTTONS)
    {
        r = MAIN_STATE_MAIN;
    }
    return r;
}
    

void state_premain_exit(void)
{
    led_set(LED_OFF);
}