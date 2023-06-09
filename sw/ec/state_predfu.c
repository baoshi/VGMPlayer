#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "io.h"
#include "led.h"
#include "tick.h"
#include "state.h"


// PRE_DFU State
// Entry:
//   LED set to PRE_DFU pattern
// Exit:
//   LED off
// Update:
//   If NW and SE are held more than TIME_HOLD_TO_ENTER_DFU, enter DFU
//   If any of NW/SE released, enter MAIN


static uint8_t _timestamp;

void state_predfu_entry(void)
{
    led_set(LED_BLINK_PRE_DFU);
    _timestamp = systick;
}


uint8_t state_predfu_update(void)
{
    uint8_t r = MAIN_STATE_PRE_DFU;
    io_debounce_inputs();
    if (                                                                        // if
        ((io_input_state & IO_STATUS_MASK_NW) == IO_STATUS_MASK_NW)             // "NW" button released
        ||                                                                      // or
        ((io_input_state & IO_STATUS_MASK_SE) == IO_STATUS_MASK_SE)             // "SE" button released
       )
    {
        r = MAIN_STATE_MAIN;
    }
    else if ((uint8_t)(systick - _timestamp) >= (TIME_HOLD_TO_ENTER_DFU / MS_PER_TICK))
    {
        r = MAIN_STATE_DFU;
    }
    led_update();
    return r;
}
    

void state_predfu_exit(void)
{
    led_set(LED_OFF);
}
