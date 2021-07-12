#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "adc.h"
#include "uplink.h"
#include "led.h"
#include "state.h"


void state_mainloop_enter(void)
{
    led_set(LED_OFF);
    io_init();
    adc_start();
    uplink_start();
    io_main_power_on();
}


uint8_t state_mainloop_loop(void)
{
    uint8_t r = MAIN_STATE_MAINLOOP;
    uint8_t status;
    led_update();
    if (adc_update())
    {
        uplink_set_byte(0x01u, adc_bat);
    }
    io_debounce_inputs();
    // Status  [ X X USB CHG M P U D]
    status = 0x00;
    if ((io_input_state & IO_STATUS_MASK_DOWN) != 0u)
    {
        status = 0x01u;
    }
    if ((io_input_state & IO_STATUS_MASK_UP) != 0u)
    {
        status |= 0x02u;
    }
    if ((io_input_state & IO_STATUS_MASK_PLAY) != 0u)
    {
        status |= 0x04u;
    }
    if ((io_input_state & IO_STATUS_MASK_MODE) != 0u)
    {
        status |= 0x08u;
    }
    if ((io_input_state & IO_STATUS_MASK_CHARGER) != 0u)
    {
        status |= 0x10u;
    }
    if (adc_vdd >= USB_VDD_THRESHOLD)
    {
        status |= 0x20u;
    }
    uplink_set_byte(0x00, status);
    uplink_track_activity();
    if ((systick - uplink_recent_activity) > TIME_UPLINK_LOST_TO_OFF)
    {
        r = MAIN_STATE_OFF;
    }
    return r;
}


void state_mainloop_exit(void)
{
    uplink_stop();
    adc_stop();
    led_set(LED_OFF);
}



