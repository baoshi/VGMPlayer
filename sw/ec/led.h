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

// LED
#define led_on() do { io_led_on(); } while (0)
#define led_off() do { io_led_off(); } while (0)
#define led_toggle() do { io_led_toggle(); } while (0)
  
#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

