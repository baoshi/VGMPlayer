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


static uint16_t entry_time;


void state_pre_dfu_enter(void)
{
    entry_time = systick;
}


uint8_t state_pre_dfu_loop(void)
{
    io_debounce_inputs();
    if (!(~io_input_state & IO_STATUS_MASK_LEFT) || !(~io_input_state & IO_STATUS_MASK_RIGHT))
    {
        return MAIN_STATE_MAINLOOP;
    }
    
    if (systick - entry_time > 3000)
        return MAIN_STATE_DFU;
    
    return MAIN_STATE_PRE_DFU;
}


