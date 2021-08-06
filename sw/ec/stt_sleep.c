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
// enter CHARGE or WAKEUP state respectively


uint8_t state_sleep_loop(void)
{
    uint8_t r;
    io_main_power_off();
    io_set_bootsel_high();
    io_led_off();
    adc_prepare_sleep();
    io_prepare_sleep();
    VREGCONbits.VREGPM = 1; // Enable low power sleep mode
    SLEEP();    // Sleep in
    // Wakeup
    io_exit_sleep();
    // Read & debounce input. If input is not persistent, wakeup is probably due to glitch
    for (uint8_t i = 0; i < 10u; ++i)    // Around 10ms debounce time
    {
        io_debounce_inputs();
    }
    if (((uint8_t)(~io_input_state) & IO_STATUS_MASK_CHARGER) != 0u)        // CHARGER_STATUS goes low
    {
        r = MAIN_STATE_CHARGE;
    }
    else if (((uint8_t)(~io_input_state) & IO_STATUS_MASK_UP) != 0u)        // If UP button down, go main loop
    {
        r = MAIN_STATE_MAINLOOP;
    }
    else if (((uint8_t)(~io_input_state) & IO_STATUS_MASK_DOWN) != 0u)      // If DOWN button down, go main loop
    {
        r = MAIN_STATE_MAINLOOP;
    }
    else if (
        (((uint8_t)(~io_input_state) & IO_STATUS_MASK_MODE) != 0u)          // One of PLAY or MODE pressed
        ||
        (((uint8_t)(~io_input_state) & IO_STATUS_MASK_PLAY) != 0u)
       )
    {
        // Debounce for some more time
        for (uint8_t i = 0; i < 80u; ++i)    // Around 80ms debounce time
        {
            io_debounce_inputs();
        }
        if (
            (((uint8_t)(~io_input_state) & IO_STATUS_MASK_MODE) != 0u)      // PLAY and MODE pressed
            &&
            (((uint8_t)(~io_input_state) & IO_STATUS_MASK_PLAY) != 0u)
            )
        {
            r = MAIN_STATE_PRE_DFU;
        }
        else
        {
            r = MAIN_STATE_MAINLOOP;
        }
    }
    else    // Bogus wakeup, go back sleep
    {
        r = MAIN_STATE_SLEEP;
    }
    return r;
}

