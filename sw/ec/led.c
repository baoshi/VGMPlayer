/*
 * File:   led.c
 * Author: Baoshi
 *
 * Created on May 1, 2021, 6:37 PM
 */

#include <xc.h>
#include "tick.h"
#include "led.h"


#define LED_BLINK_DFU_ON1   0x02
#define LED_BLINK_DFU_OFF1  0x12
#define LED_BLINK_DFU_ON2   0x22
#define LED_BLINK_DFU_OFF2  0x32

#define LED_BLINK_OFF_ON1   0x03
#define LED_BLINK_OFF_OFF1  0x13


static uint8_t _state;
static uint16_t _timestamp;

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
    case LED_BLINK_DFU_ON1:
        io_led_on();
        if (systick - _timestamp > 200)
        {
            _state = LED_BLINK_DFU_OFF1;
            _timestamp = systick;
        }
        break;
    case LED_BLINK_DFU_OFF1:
        io_led_off();
        if (systick - _timestamp > 100)
        {
            _state = LED_BLINK_DFU_ON2;
            _timestamp = systick;
        }
        break;
    case LED_BLINK_DFU_ON2:
        io_led_on();
        if (systick - _timestamp > 200)
        {
            _state = LED_BLINK_DFU_OFF2;
            _timestamp = systick;
        }
        break;
    case LED_BLINK_DFU_OFF2:
        io_led_off();
        if (systick - _timestamp > 500)
        {
            _state = LED_BLINK_DFU_ON1;
            _timestamp = systick;
        }
        break;
    case LED_BLINK_OFF_ON1:
        io_led_on();
        if (systick - _timestamp > 500)
        {
            _state = LED_BLINK_OFF_OFF1;
            _timestamp = systick;
        }
        break;
    case LED_BLINK_OFF_OFF1:
        io_led_off();
        if (systick - _timestamp > 500)
        {
            _state = LED_BLINK_OFF_ON1;
            _timestamp = systick;
        }
        break;
    }
}
