/* 
 * File:   led.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 8:05 PM
 */

#ifndef LED_H
#define	LED_H

#include "io.h"


#ifdef	__cplusplus
extern "C"
{
#endif

// LED -> RC4
#define LED_On io_led_on
#define LED_Off io_led_off 
#define LED_Toggle io_led_toggle 
  
#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

