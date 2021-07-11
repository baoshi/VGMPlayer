/* 
 * File:   tick.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 8:42 PM
 */

#ifndef TICK_H
#define	TICK_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include <stdint.h>

extern uint16_t systick;
  
void tick_init(void);

void tick_update(void);

void tick_tmr0_isr(void);

void tick_waste_ms(uint16_t ms);

#ifdef	__cplusplus
}
#endif

#endif	/* TICK_H */

