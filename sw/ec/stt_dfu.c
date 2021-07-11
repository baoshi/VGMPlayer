#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "io.h"
#include "led.h"
#include "uplink.h"
#include "adc.h"
#include "state.h"


enum 
{
    DFU_STATE_ENTER = 0,
    DFU_STATE_BOOTSEL_LOW,
    DFU_STATE_BOOTSEL_HOLD,
    DFU_STATE_WAIT_ACTIVITY,
    DFU_STATE_FAIL
};

static uint8_t _state;
static uint8_t _count;
static uint16_t _timestamp;


void state_dfu_enter(void)
{
    led_set(LED_BLINK_DFU);
    adc_start();
    _timestamp = systick;
    _count = 0;
    _state = DFU_STATE_ENTER;
}


uint8_t state_dfu_loop(void)
{
    uint8_t r = MAIN_STATE_DFU;
    switch (_state)
    {
    case DFU_STATE_ENTER:
        // Measure VDD. If VDD > 4.6V for more than 10 times, enter next stage
        // Otherwise if no VDD present for 1 minute, go reset
        if (adc_update() && (adc_vdd >= 138u))   // 4.6V
        {
            ++_count;
            if (_count > 10u)
            {
                io_set_bootsel_low();
                _timestamp = systick;
                _state = DFU_STATE_BOOTSEL_LOW;
                break;
            }
        }
        if ((systick - _timestamp) > 60000u)  // 60s
        {
            _state = DFU_STATE_FAIL;
        }
        break;
    case DFU_STATE_BOOTSEL_LOW:
        // Hold BOOTSEL low for 10ms then turn on main power
        // _timestamp already updated before enter this state
        if ((systick - _timestamp) > 10u)
        {
            io_main_power_on();
            _timestamp = systick;
            _state = DFU_STATE_BOOTSEL_HOLD;
        }
        break;
    case DFU_STATE_BOOTSEL_HOLD:
        // While main power is on, hold BOOTSEL for another 100ms then release
        if ((systick - _timestamp) > 100u)
        {
            io_set_bootsel_high();
            uplink_start();
            _timestamp = systick;
            _state = DFU_STATE_WAIT_ACTIVITY;
        }
        break;
    case DFU_STATE_WAIT_ACTIVITY:
        // If uplink has activity, go to MAINLOOP
        // If 60s passed without uplink activity, go to FAIL
        if ((uplink_activity - _timestamp) > 100u)  // newer uplink activity since uplink started
        {
            r = MAIN_STATE_MAINLOOP;
        }
        else
        {
            if ((systick - _timestamp) > 60000u)  // 60s
            {
                _state = DFU_STATE_FAIL;
            }
        }
        break;
    case DFU_STATE_FAIL:
        RESET();
        break;
    default:
        break;
    }
    led_update();
    return r;
}


void state_dfu_exit(void)
{
    adc_stop();
    led_set(LED_OFF);
}




