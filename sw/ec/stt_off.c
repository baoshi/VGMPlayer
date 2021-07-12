#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "led.h"
#include "state.h"


static uint16_t _time_stamp;

void state_off_enter(void)
{
    led_set(LED_BLINK_OFF);
    _time_stamp = systick;
}


uint8_t state_off_loop(void)
{
    led_update();
    if ((systick - _time_stamp) > TIME_NOTICE_OFF)
    {
        io_debounce_inputs();
        if ((io_input_state & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_ANY_BUTTON)  // Wait for all button release
        {
            RESET();
        }
    }
    return MAIN_STATE_OFF;
}

