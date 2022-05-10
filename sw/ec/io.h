/* 
 * File:   io.h
 * Author: Baoshi
 *
 * Created on May 1, 2021, 6:38 PM
 */

#ifndef IO_H
#define	IO_H


#include <stdint.h>


#ifdef	__cplusplus
extern "C" {
#endif
  
// RA0 - IO_SW_NW
// RA1 - IO_SW_SW
// RA2 - IO_SW_PLAY
// RA3 - IO_SW_NE
// RA4 - IO_SW_SE
// RA5 - IO_CHARGER_STATUS
extern uint8_t volatile io_input_state;  
extern uint8_t volatile io_prev_input_state;

#define IO_STATUS_MASK_NW           0x01u
#define IO_STATUS_MASK_SW           0x02u
#define IO_STATUS_MASK_PLAY         0x04u
#define IO_STATUS_MASK_NE           0x08u
#define IO_STATUS_MASK_SE           0x10u
#define IO_STATUS_MASK_BUTTONS      0x1Fu
#define IO_STATUS_MASK_CHARGER      0x20u


void io_init(void);
void io_debounce_inputs(void);
void io_main_power_on(void);
void io_main_power_off(void);
void io_set_bootsel_low(void);
void io_set_bootsel_high(void);
void io_led_on(void);
void io_led_off(void);
void io_prepare_sleep(void);
void io_exit_sleep(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */

