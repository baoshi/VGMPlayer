#include <xc.h>
#include <stdint.h>
#include "io.h"
#include "led.h"
#include "adc.h"
#include "tick.h"
#include "state.h"


// START state
// Initial startup state, configure IO ports and use ADC to detect VDD
// If VDD exceed threshold (~4.6V), enter CHARGE state, otherwise enter
// SLEEP state


void state_start_enter(void)
{
    // Make sure I/O is configured as known state
    io_init();
    io_main_power_off();
    io_set_bootsel_high();
    io_led_off();
    // ADC is to determine if USB is connected
    adc_start();
}


uint8_t state_start_loop(void)
{
    uint8_t r = MAIN_STATE_START;
    if (adc_update())
    {
        if (adc_vdd >= 138u) // VDD > 138/30 = 4.6V ?
        {
            // USB connected
            r = MAIN_STATE_CHARGE;
        }
        else
        {
            // USB not connected
            r = MAIN_STATE_SLEEP;
        }
    }
    return r;
}


void state_start_exit(void)
{
    adc_stop();
}