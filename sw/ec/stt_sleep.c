#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "led.h"
#include "adc.h"
#include "state.h"

// SLEEP state
// Off main processor, config interrupt-on-change on IO pins and enter sleep
// When wake up, check wake up reason is USB connected or button down, then
// enter CHARGE or PRE_MAINLOOP state respectively

uint16_t wakeup_time = 0;
uint8_t wakeup_input_state;

uint8_t state_sleep_enter(void)
{
    while (1)
    {
        io_main_power_off();
        io_set_bootsel_high();
        io_led_off();
        adc_prepare_sleep();
        io_prepare_sleep();
        VREGCONbits.VREGPM = 1; // Enable low power sleep mode
        SLEEP();    // Sleep in
        // Wakeup
        io_exit_sleep();
        // Read & debounce input. If input is not persistent, wakeup is probably
        // due to glitch and we shall go back sleep
        for (uint8_t i = 0; i < 30; ++i)    // Around 37ms debounce time
        {
            io_debounce_inputs();
        }
        if (~io_input_state & IO_STATUS_MASK_CHARGER)
        {
            return MAIN_STATE_CHARGE;
        }
        if (~io_input_state & IO_STATUS_MASK_ANY_BUTTON)
        {
            wakeup_input_state = io_input_state;
            wakeup_time = systick;  
            led_on();
            break;
        }
    }
    return MAIN_STATE_SLEEP;
}


uint8_t state_sleep_loop(void)
{
    io_debounce_inputs();
    
    if (
        ((~wakeup_input_state & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_UP)   // Only UP was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_UP))                                     // UP now released
       )
    {
        led_off();
        return MAIN_STATE_MAINLOOP;
    }
    if (
        ((~wakeup_input_state & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_DOWN)  // Only DOWN was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_DOWN))                                    // DOWN now released
       )
    {
        led_off();
        return MAIN_STATE_MAINLOOP;
    }
    if (
        ((~wakeup_input_state & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_LEFT)  // Only LEFT was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_LEFT))                                    // LEFT now released
       )
    {
        led_off();
        return MAIN_STATE_MAINLOOP;
    }
    if (
        ((~wakeup_input_state & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_RIGHT)// Only RIGHT was pressed
        &&
        ((io_input_state & IO_STATUS_MASK_RIGHT))                                  // RIGHT now released
       )
    {
        led_off();
        return MAIN_STATE_MAINLOOP;
    }
    // If nothing happened for 5 sec, go back to sleep
    if (systick - wakeup_time > 5000)
    {
        return state_sleep_enter();
    }
    return MAIN_STATE_SLEEP;
}

