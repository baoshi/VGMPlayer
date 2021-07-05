/* 
 * File:   led.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 8:05 PM
 */

#ifndef LED_H
#define	LED_H

#include "io.h"

#define LED_OFF             0x00
#define LED_ON              0x01
#define LED_BLINK_DFU       0x02
#define LED_BLINK_OFF       0x03


#ifdef	__cplusplus
extern "C"
{
#endif


void led_set(uint8_t pattern);
void led_update(void);

  
#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

