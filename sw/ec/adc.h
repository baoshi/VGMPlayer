/* 
 * File:   adc.h
 * Author: Baoshi
 *
 * Created on May 3, 2021, 10:49 AM
 */

#ifndef ADC_H
#define	ADC_H

#include <stdint.h>
#include <stdbool.h>


#ifdef	__cplusplus
extern "C"
{
#endif

extern uint8_t volatile adc_vdd;
extern uint8_t volatile adc_bat;

void adc_start(void);
bool adc_update(void);  // return true if measurements are done
void adc_stop(void);
void adc_prepare_sleep(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

