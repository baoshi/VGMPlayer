#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "adc.h"
#include "state.h"

// SLEEP state
// Off main processor, config interrupt-on-change on IO pins and enter sleep
// When wake up, check wake up reason is USB connected or button down, then
// enter CHARGE or PRE_MAINLOOP state respectively


uint8_t state_sleep_loop(void)
{
    io_main_power_off();
    io_set_bootsel_high();
    io_led_off();
    adc_prepare_sleep();
    io_prepare_sleep();
    VREGCONbits.VREGPM = 1; // Enable low power sleep mode
    SLEEP();    // Sleep in
    // zzzzzz
    uint8_t f = IOCAF;  // wake up, read IOC reason
    io_exit_sleep();
    if (f & IO_STATUS_MASK_ANYBUTTON)
    {
        return MAIN_STATE_PRE_MAINLOOP;
    }
    else
    {
        // wake up because battery starts to charge
        // we use this to detect if wake up is because USB connected
        // it is not accurate when battery is full and usb connected, but rare
        return MAIN_STATE_CHARGE;
    }
}

