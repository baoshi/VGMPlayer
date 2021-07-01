/* 
 * File:   adc.h
 * Author: Baoshi
 *
 * Created on May 3, 2021, 10:49 AM
 */

#ifndef ADC_H
#define	ADC_H



#ifdef	__cplusplus
extern "C"
{
#endif

extern uint8_t volatile adc_vdd;
extern uint8_t volatile adc_bat;

void adc_start(void);
void adc_update(void);
void adc_stop(void);
void adc_prepare_sleep(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

