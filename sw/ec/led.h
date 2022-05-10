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
#define LED_BLINK_PRE_DFU   0x02    // On 100ms -- Off 400ms -- repeat
#define LED_BLINK_DFU       0x03    // On 200ms -- Off 100ms -- On 200ms -- Off 500ms -- repeat
#define LED_BLINK_PRE_OFF   0x04    // On 500ms -- Off 500ms -- repeat


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

