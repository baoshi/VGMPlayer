#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "io.h"
#include "adc.h"
#include "state.h"


void state_sleep_entry(void)
{
    // Make sure I/O is configured as known state
    io_init();
    io_main_power_off();
    io_set_bootsel_high();
    io_led_off();
    // Perform several io reading to get stable io state
    uint8_t t;
    for (t = 0; t < 10u; ++t)
    {
        io_debounce_inputs();
    }
}


uint8_t state_sleep_update(void)
{
    uint8_t r = MAIN_STATE_SLEEP;
    // Set LED based on charger status
    if ((io_input_state & IO_STATUS_MASK_CHARGER) == 0u) // is charging?
    {
        io_led_on();
    }
    else
    {
        io_led_off();
    }
    // Enter sleep
    io_main_power_off();
    io_set_bootsel_high();
    adc_prepare_sleep();
    io_prepare_sleep();
    VREGCONbits.VREGPM = 1; // Enable low power sleep mode
    SLEEP();    // Sleep in
    // Wakeup
    io_exit_sleep();
    // Read & debounce inputs ~10ms
    for (uint8_t i = 0; i < 10u; ++i)
    {
        io_debounce_inputs();
    }
    // If any of the "SW", "Play", "NE" buttons are down, go to PRE_MAIN state
    if ((~io_input_state & (IO_STATUS_MASK_SW | IO_STATUS_MASK_PLAY | IO_STATUS_MASK_NE)) != 0u)
    {
        r = MAIN_STATE_PRE_MAIN;
    }
    // One or both "NW" and "SE" buttons are down, take some more time to check if both are down
    else if ((~io_input_state & (IO_STATUS_MASK_NW | IO_STATUS_MASK_SE)) != 0u)
    {
        // Read & debounce inputs ~100ms
        for (uint8_t i = 0; i < 100u; ++i)
        {
            io_debounce_inputs();
        }
        // If both "NW" and "SE" are down, go into PRE_DFU, else go to PRE_MAIN
        if ( ((~io_input_state & IO_STATUS_MASK_NW) != 0u)
             &&
             ((~io_input_state & IO_STATUS_MASK_SE) != 0u)
           )
        {
            r = MAIN_STATE_PRE_DFU;
        }
        else
        {
            r = MAIN_STATE_PRE_MAIN;
        }
    }
    // else wake up may due to charge state change or glitch. Go back to SLEEP state.
    return r;
}
