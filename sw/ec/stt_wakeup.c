#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "led.h"
#include "adc.h"
#include "state.h"


// WAKEUP state
// Enter this state when one of more buttons are pressed
// If single is released, enter MAINLOOP state
// If L&R buttons are pressed at the same time, enter PRE_DFU state
// If buttons are held long or multiple buttons (not L&R) are held, back to SLEEP state


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
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_UP)    // Only UP was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_UP) != 0u)                                            // UP now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    else if (
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_DOWN)  // Only DOWN was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_DOWN) != 0u)                                          // DOWN now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    else if (
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_LEFT)  // Only LEFT was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_LEFT) != 0u)                                          // LEFT now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    else if (
        (((uint8_t)(~_wakeup_input_state) & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_RIGHT) // Only RIGHT was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_RIGHT) != 0u)                                         // RIGHT now released
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_MAINLOOP;
    }
    // If L/R both pressed, go into PRE_DFU state
    else if (
        (((uint8_t)(~io_input_state) & IO_STATUS_MASK_LEFT) != 0u)
        &&
        (((uint8_t)(~io_input_state) & IO_STATUS_MASK_RIGHT) != 0u)
       )
    {
        led_set(LED_OFF);
        r = MAIN_STATE_PRE_DFU;
    }
    else if ((systick - _wakeup_time) > 5000u) // Reach here if nothing happened (button not released, multiple button pressed at same time, etc), go back to sleep
    {
        led_set(LED_OFF);
        r = MAIN_STATE_SLEEP;
    }
    else
    {
        // Just stay in wakeup. Will go into sleep after 5 seconds
        r = MAIN_STATE_WAKEUP;
    }
    return r;
}

