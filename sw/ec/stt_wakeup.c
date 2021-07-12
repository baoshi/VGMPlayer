#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "led.h"
#include "adc.h"
#include "state.h"


// WAKEUP state
// Enter this state when one of more buttons are pressed
// If single button is released, enter MAINLOOP state
// If PLAY & MODE buttons are pressed at the same time, enter PRE_DFU state
// If buttons (not PLAY & MODE) are held too long (TIME_IDLE_STAY_AWAKE) back to SLEEP state


static uint16_t _wakeup_time = 0;
static uint8_t _wakeup_input_state;


void state_wakeup_enter(void)
{
    _wakeup_input_state = io_input_state;
    _wakeup_time = systick;  
    led_set(LED_ON);
}


uint8_t state_wakeup_loop(void)
{
    uint8_t r;
    io_debounce_inputs();
    if (
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_UP)    // Only UP was pressed when wake up
        &&
        ((io_input_state & IO_STATUS_MASK_UP) != 0u)                                            // UP now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    else if (
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_DOWN)  // Only DOWN was pressed when wake up
        &&
        ((io_input_state & IO_STATUS_MASK_DOWN) != 0u)                                          // DOWN now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    else if (
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_MODE)  // Only MODE was pressed when wake up
        &&
        ((io_input_state & IO_STATUS_MASK_MODE) != 0u)                                          // MODE now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    else if (
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_PLAY) // Only PLAY was pressed when wake up
        &&
        ((io_input_state & IO_STATUS_MASK_PLAY) != 0u)                                         // PLAY now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    // If PLAY & MODE both pressed, go into PRE_DFU state
    else if (
        (((uint8_t)(~io_input_state) & IO_STATUS_MASK_MODE) != 0u)
        &&
        (((uint8_t)(~io_input_state) & IO_STATUS_MASK_PLAY) != 0u)
       )
    {
        r = MAIN_STATE_PRE_DFU;
    }
    else if ((systick - _wakeup_time) > TIME_IDLE_STAY_AWAKE) // Anything else (button not released, multiple button pressed at same time, etc) for 5 seconds
    {
        led_set(LED_OFF);
        r = MAIN_STATE_SLEEP;
    }
    else    // Anything else less than 5 seconds, keep waiting
    {
        r = MAIN_STATE_WAKEUP;
    }
    return r;
}

