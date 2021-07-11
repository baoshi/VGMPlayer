#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "led.h"
#include "state.h"


// PRE_DFU state
// Enter this state when EC is waken by press L&R buttons
// If both buttons are held for 3 seconds, enter DFU state
// Otherwise go to MAINLOOP state


static uint16_t _entry_time;


void state_pre_dfu_enter(void)
{
    _entry_time = systick;
}


uint8_t state_pre_dfu_loop(void)
{
    uint8_t r;
    io_debounce_inputs();
    if (!((uint8_t)(~io_input_state) & IO_STATUS_MASK_LEFT) || !((uint8_t)(~io_input_state) & IO_STATUS_MASK_RIGHT))
    {
        r = MAIN_STATE_MAINLOOP;
    }
    else if ((systick - _entry_time) > 3000u)
    {
        r = MAIN_STATE_DFU;
    }
    else
    {
        r = MAIN_STATE_PRE_DFU;
    }
    return r;
}


