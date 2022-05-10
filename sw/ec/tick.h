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
#include <stdbool.h>

extern volatile uint8_t systick;

#define MS_PER_TICK 20

void tick_init(void);

void tick_tmr0_isr(void);



#ifdef	__cplusplus
}
#endif

#endif	/* TICK_H */

