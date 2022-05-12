
#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "led.h"
#include "state.h"

// OFF State
// Entry:
//   LED set to PRE_OFF pattern
// Exit:
//   None
// Update:
//   After TIME_NOTICE_OFF elapses, wait button release and reset MCU
//   

static uint8_t _timestamp;

void state_off_entry(void)
{
    led_set(LED_BLINK_PRE_OFF);
    _timestamp = systick;
}


uint8_t state_off_update(void)
{
    led_update();
    if ((uint8_t)(systick - _timestamp) >= (TIME_NOTICE_OFF / MS_PER_TICK))
    {
        io_debounce_inputs();
        if ((io_input_state & IO_STATUS_MASK_BUTTONS) == IO_STATUS_MASK_BUTTONS)  // Wait for all button release
        {
            RESET();
        }
    }
    return MAIN_STATE_OFF;
}

