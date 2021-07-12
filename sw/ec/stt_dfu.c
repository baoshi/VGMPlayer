#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "led.h"
#include "uplink.h"
#include "adc.h"
#include "state.h"



#define TIME_BOOTSEL_SETUP      10u
#define TIME_BOOTSEL_HOLD       100u 


enum 
{
    DFU_STATE_ENTER = 0,        // Just enter, wait all button released
    DFU_STATE_WAIT_USB,         // Wait for USB connection
    DFU_STATE_BOOTSEL_LOW,      // Hold BOOTSEL down for 10ms then power on main processor
    DFU_STATE_BOOTSEL_HOLD,     // Continue hold BOOTSEL for 100ms
    DFU_STATE_WAIT_ACTIVITY,    // Check if I2C has activity , if not go FAIL otherwise go MAINLOOP
    DFU_STATE_FAIL              // RESET
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
        // Wait for all button release
        io_debounce_inputs();
        if ((io_input_state & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_ANY_BUTTON)  // If all button released
        {
            _state = DFU_STATE_WAIT_USB;
        }
        break;
    case DFU_STATE_WAIT_USB:
        // Check USB stable (10 times), if USB connected enter next stage
        // If any button down, exit to FAIL
        // Otherwise if no USB present for 1 minute, go reset
        if (adc_update() && (adc_vdd >= USB_VDD_THRESHOLD))
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
        io_debounce_inputs();
        if (((uint8_t)(~io_input_state) & IO_STATUS_MASK_ANY_BUTTON) != 0u)
        {
            _state = DFU_STATE_FAIL;
            break;
        }
        if ((systick - _timestamp) > TIME_NO_USB_TIMEOUT)
        {
            _state = DFU_STATE_FAIL;
        }
        break;
    case DFU_STATE_BOOTSEL_LOW:
        // Hold BOOTSEL low for TIME_BOOTSEL_SETUP then turn on main power
        if ((systick - _timestamp) > TIME_BOOTSEL_SETUP)
        {
            io_main_power_on();
            _timestamp = systick;
            _state = DFU_STATE_BOOTSEL_HOLD;
        }
        break;
    case DFU_STATE_BOOTSEL_HOLD:
        // While main power is on, hold BOOTSEL for another TIME_BOOTSEL_HOLD then release
        if ((systick - _timestamp) > TIME_BOOTSEL_HOLD)
        {
            io_set_bootsel_high();
            uplink_start();
            _timestamp = systick;
            _state = DFU_STATE_WAIT_ACTIVITY;
        }
        break;
    case DFU_STATE_WAIT_ACTIVITY:
        // If uplink has activity, go to MAINLOOP
        // If any button down, exit to FAIL
        // If 60s passed without uplink activity, go to FAIL
        uplink_track_activity();
        if (uplink_recent_activity != _timestamp) // When entering, uplink_recent_activity == _timestamp
        {
            r = MAIN_STATE_MAINLOOP;
            break;
        }
        io_debounce_inputs();
        if (((uint8_t)(~io_input_state) & IO_STATUS_MASK_ANY_BUTTON) != 0u)
        {
            _state = DFU_STATE_FAIL;
            break;
        }
        if ((systick - _timestamp) > TIME_FIRMWARE_FLASH_TIMEOUT)
        {
            _state = DFU_STATE_FAIL;
        }
        break;
    case DFU_STATE_FAIL:
        // Wait button release then reset
        io_debounce_inputs();
        if ((io_input_state & IO_STATUS_MASK_ANY_BUTTON) == IO_STATUS_MASK_ANY_BUTTON)
        {
            RESET();
        }
        break;
    default:
        break;
    }
    led_update();
    return r;
}


void state_dfu_exit(void)
{
    uplink_stop();
    adc_stop();
    led_set(LED_OFF);
}




