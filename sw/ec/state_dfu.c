#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "io.h"
#include "adc.h"
#include "led.h"
#include "i2c.h"
#include "state.h"

// DFU State
// Entry:
//   LED set to DFU pattern
//   Prepare DFU sub-state machine
// Exit:
//   LED off
// Update:
//   DFU sub-state machine process
//   


enum 
{
    DFU_STATE_ENTER = 0,        // Just enter, wait all button released
    DFU_STATE_WAIT_USB,         // Wait for USB. When connected, hold BOOTSEL down, power up main processor
    DFU_STATE_BOOTSEL,          // Wait TIME_BOOTSEL_HOLD then release BOOTSEL
    DFU_STATE_FLASH,            // If I2C query received, to MAIN_STATE_MAIN; If any button down, go EXIT
    DFU_STATE_EXIT              // Wait button release then RESET
};


static uint8_t _state;
static uint8_t _timestamp;

void state_dfu_entry(void)
{
    led_set(LED_BLINK_DFU);
    adc_start();
    _timestamp = systick;
    _state = DFU_STATE_ENTER;
}


uint8_t state_dfu_update(void)
{
    uint8_t r = MAIN_STATE_DFU;
    switch (_state)
    {
    case DFU_STATE_ENTER:  // wait all button to release
        io_debounce_inputs();
        if ((io_input_state & IO_STATUS_MASK_BUTTONS) == IO_STATUS_MASK_BUTTONS)  // If all button released
        {
            _timestamp = systick;
            _state = DFU_STATE_WAIT_USB;
        }
        break;
       
    case DFU_STATE_WAIT_USB:
        // check if USB connected and stable
        if (adc_update())
        { 
            if (adc_vdd >= USB_VDD_THRESHOLD)            {
                if ((uint8_t)(systick - _timestamp) >= (40 / MS_PER_TICK))
                {
                    // USB connected and VDD stable
                    io_set_bootsel_low();
                    io_main_power_on();
                    _timestamp = systick;
                    _state = DFU_STATE_BOOTSEL;
                    break;
                }
            }
            else
            {
                _timestamp = systick;
            }
        }
        // if any button down, abort DFU
        io_debounce_inputs();
        if ((~io_input_state) & IO_STATUS_MASK_BUTTONS)
        {
            _state = DFU_STATE_EXIT;
            break;
        }
        break;
   
    case DFU_STATE_BOOTSEL:
        // Wait TIME_BOOTSEL_HOLD then release BOOTSEL
        if ((uint8_t)(systick - _timestamp) >= (TIME_BOOTSEL_HOLD / MS_PER_TICK))
        {
            io_set_bootsel_high();
            i2c_start();
            _timestamp = systick;
            _state = DFU_STATE_FLASH;
        }
        break;

    case DFU_STATE_FLASH:
        // If I2C query received, to MAIN_STATE_MAIN
        // I2C data is not actually set here because we only interested in if host is querying I2C
        i2c_track_activity();
        if (i2c_recent_activity != _timestamp) // When i2c starting, i2c_recent_activity == _timestamp
        {
            r = MAIN_STATE_MAIN;
            break;
        }
        // If any button down, go EXIT
        io_debounce_inputs();
        if ((~io_input_state) & IO_STATUS_MASK_BUTTONS)
        {
            _state = DFU_STATE_EXIT;
        }
        break;
        
    case DFU_STATE_EXIT:
        // Wait button release then reset
        io_debounce_inputs();
        if ((io_input_state & IO_STATUS_MASK_BUTTONS) == IO_STATUS_MASK_BUTTONS)
        {
            r = MAIN_STATE_OFF;
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
    led_set(LED_OFF);
}
