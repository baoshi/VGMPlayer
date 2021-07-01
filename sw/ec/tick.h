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

#define DECLARE_REPEAT_BLOCK(id) static uint16_t _ts_##id = 0;

#define REPEAT_BLOCK_BEGIN(id, interval) if (systick - _ts_##id > interval) {

#define REPEAT_BLOCK_END(id) _ts_##id = systick; }


#ifdef	__cplusplus
}
#endif

#endif	/* TICK_H */

