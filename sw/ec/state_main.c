#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "adc.h"
#include "led.h"
#include "i2c.h"
#include "state.h"


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
    // Temporary disabled for dev
    if (0) // ((uint8_t)(systick - i2c_recent_activity) >= (TIME_I2C_LOST_TO_OFF / MS_PER_TICK))
    {
        r = MAIN_STATE_OFF;
    }
    return r;
}
    

void state_main_exit(void)
{
    i2c_stop();
    adc_stop();
}
