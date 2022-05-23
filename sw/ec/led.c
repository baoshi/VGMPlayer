/*
 * File:   led.c
 * Author: Baoshi
 *
 * Created on May 1, 2021, 6:37 PM
 */

#include <xc.h>
#include "tick.h"
#include "led.h"


#define LED_BLINK_PRE_DFU_ON1   (LED_BLINK_PRE_DFU)
#define LED_BLINK_PRE_DFU_OFF1  (LED_BLINK_PRE_DFU | 0x10)

#define LED_BLINK_DFU_ON1       (LED_BLINK_DFU)
#define LED_BLINK_DFU_OFF1      (LED_BLINK_DFU | 0x10)
#define LED_BLINK_DFU_ON2       (LED_BLINK_DFU | 0x20)
#define LED_BLINK_DFU_OFF2      (LED_BLINK_DFU | 0x30)

#define LED_BLINK_OFF_ON1       (LED_BLINK_PRE_OFF)
#define LED_BLINK_OFF_OFF1      (LED_BLINK_PRE_OFF | 0x10)


static uint8_t _timestamp;
static uint8_t _state;


void led_set(uint8_t pattern)
{
    _state = pattern;
    _timestamp = systick;
    led_update();
}


void led_update(void)
{
    switch (_state)
    {
    case LED_OFF:
        io_led_off();
        break;
        
    case LED_ON:
        io_led_on();
        break;
        
    /* PRE_DFU pattern */
    case LED_BLINK_PRE_DFU_ON1:
        io_led_on();
        if ((uint8_t)(systick - _timestamp) >= (100 / MS_PER_TICK)) 
        {
            _state = LED_BLINK_PRE_DFU_OFF1;
            _timestamp = systick;
        }
        break;
        
    case LED_BLINK_PRE_DFU_OFF1:
        io_led_off();
        if ((uint8_t)(systick - _timestamp) >= (400 / MS_PER_TICK))
        {
            _state = LED_BLINK_PRE_DFU_ON1;
            _timestamp = systick;
        }
        break;
        
    /* DFU pattern */
    case LED_BLINK_DFU_ON1:
        io_led_on();
        if ((uint8_t)(systick - _timestamp) >= (200 / MS_PER_TICK))
        {
            _state = LED_BLINK_DFU_OFF1;
            _timestamp = systick;
        }
        break;
        
    case LED_BLINK_DFU_OFF1:
        io_led_off();
        if ((uint8_t)(systick - _timestamp) >= (100 / MS_PER_TICK))
        {
            _state = LED_BLINK_DFU_ON2;
            _timestamp = systick;
        }
        break;
        
    case LED_BLINK_DFU_ON2:
        io_led_on();
        if ((uint8_t)(systick - _timestamp) >= (200 / MS_PER_TICK))
        {
            _state = LED_BLINK_DFU_OFF2;
            _timestamp = systick;
        }
        break;
        
    case LED_BLINK_DFU_OFF2:
        io_led_off();
        if ((uint8_t)(systick - _timestamp) >= (500 / MS_PER_TICK))
        {
            _state = LED_BLINK_DFU_ON1;
            _timestamp = systick;
        }
        break;
        
    case LED_BLINK_OFF_ON1:
        io_led_on();
        if ((uint8_t)(systick - _timestamp) >= (100 / MS_PER_TICK))
        {
            _state = LED_BLINK_OFF_OFF1;
            _timestamp = systick;
        }
        break;
        
    case LED_BLINK_OFF_OFF1:
        io_led_off();
        if ((uint8_t)(systick - _timestamp) >= (100 / MS_PER_TICK))
        {
            _state = LED_BLINK_OFF_ON1;
            _timestamp = systick;
        }
        break;
        
    default:
        break;
    }
}
