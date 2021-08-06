#include <xc.h>
#include <stdint.h>
#include "global.h"
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


#define MAX_VDD_LOST    100u
static uint8_t num_vdd_lost;

void state_charge_enter(void)
{
    num_vdd_lost = 0;
    adc_start();
}


uint8_t state_charge_loop(void)
{
    uint8_t r = MAIN_STATE_CHARGE;
    io_debounce_inputs();
    if (((uint8_t)(~io_input_state) & IO_STATUS_MASK_ANY_BUTTON) != 0u)
    {
        // Any button pressed (LOW)
        r = MAIN_STATE_MAINLOOP;
    }
    else
    {
        if (adc_update())
        {
            if (adc_vdd >= USB_VDD_THRESHOLD)
            {
                if ((io_input_state & IO_STATUS_MASK_CHARGER) != 0u)
                {
                    // High -> not charging
                    led_set(LED_OFF);
                }
                else
                {
                    led_set(LED_ON);
                }
                num_vdd_lost = 0;
            }
            else
            {
                // Won't be charging if VDD is lost, turn off LED
                led_set(LED_OFF);
                ++num_vdd_lost;
                if (num_vdd_lost > MAX_VDD_LOST)
                {
                    r = MAIN_STATE_SLEEP;
                }
            }
        }
    }
    return r;
}


void state_charge_exit(void)
{
    adc_stop();
    led_set(LED_OFF);
}
