#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "adc.h"
#include "led.h"
#include "i2c.h"
#include "state.h"


// MAIN State
// Entry:
//   LED off
//   Start ADC, I2C
// Exit:
//   Stop ADC, I2C
// Update:
//   Update IO, ADC
//   If I2C activity lost for more than TIME_I2C_LOST_TO_OFF, go to OFF state


void state_main_entry(void)
{
    adc_start();
    i2c_start();
    io_main_power_on();
    led_set(LED_OFF);
}


uint8_t state_main_update(void)
{
    uint8_t r = MAIN_STATE_MAIN;
    // Read battery and save to I2C buffer
    adc_update();
    io_debounce_inputs();
    i2c_track_activity();
    // Check if host request off
    if (global_power_off) r = MAIN_STATE_OFF;
    // Check if I2C is alive
    if ((uint8_t)(systick - i2c_recent_activity) >= (TIME_I2C_LOST_TO_OFF / MS_PER_TICK))
    {
        // I2C communication lost may due to the host freeze or host resets at wrong time 
        // (e.g., host resets after sending address byte, EC will assert SDA (as ACK) and waiting 
        // more clocks but nothing happened). It should be addressable by certain interrupt but
        // we just tap timeout to reset i2c bus.
        i2c_stop();
        i2c_start();
        if (i2c_watchdog)
        {
            r = MAIN_STATE_TIMEOUT;
        }
    }
    return r;
}
   

void state_main_exit(void)
{
    i2c_stop();
    adc_stop();
}
