#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "adc.h"
#include "led.h"
#include "state.h"

// CHARGE state
// If any button down, enter PRE_MAINLOOP state
// Read VDD, if VDD < 4.6V, increase num_vdd_lost count
// If num_vdd_lost > MAX_VDD_LOST, enter SLEEP state
// If VDD >= 4.6V, set LED according to charging status



#define MAX_VDD_LOST    100
static uint8_t num_vdd_lost;

uint8_t state_charge_enter(void)
{
    num_vdd_lost = 0;
    led_on();
    tick_waste_ms(1000);
    led_off();
    adc_start();
    return MAIN_STATE_CHARGE;
}


uint8_t state_charge_loop(void)
{
    io_debounce_inputs();
    if (~io_input_state & IO_STATUS_MASK_ANY_BUTTON)
    {
        // Any button pressed (LOW)
        return MAIN_STATE_MAINLOOP;
    }
    if (adc_update())
    {
        if (adc_vdd >= 138)
        {
            if (io_input_state & IO_STATUS_MASK_CHARGER)
            {
                // High -> not charging
                led_off();
            }
            else
            {
                led_on();
            }
            num_vdd_lost = 0;
        }
        else
        {
            // Won't be charging if VDD is lost, turn off LED
            led_off();
            ++num_vdd_lost;
            if (num_vdd_lost > MAX_VDD_LOST)
                return MAIN_STATE_SLEEP;
        }
    }
    return MAIN_STATE_CHARGE;
}


void state_charge_exit(void)
{
    adc_stop();
    led_off();
}
