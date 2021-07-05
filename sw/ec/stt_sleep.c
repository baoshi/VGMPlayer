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
    for (uint8_t i = 0; i < 10; ++i)    // Around 10ms debounce time
    {
        io_debounce_inputs();
    }
    if (~io_input_state & IO_STATUS_MASK_CHARGER)       // CHARGER_STATUS goes low
    {
        return MAIN_STATE_CHARGE;
    }
    if (~io_input_state & IO_STATUS_MASK_ANY_BUTTON)    // Button down, wake up
    {
        return MAIN_STATE_WAKEUP;
    }
    // Nothing caused wakeup, go back sleep
    return MAIN_STATE_SLEEP;
}

