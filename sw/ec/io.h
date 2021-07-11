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
  
// RA0 - IO_SW_LEFT
// RA1 - IO_SW_UP
// RA2 - IO_CHARGER_STATUS
// RA3 - 0
// RA4 - IO_SW_RIGHT
// RA5 - IO_SW_DOWN
extern uint8_t io_input_state;  
extern uint8_t io_prev_input_state;

#define IO_STATUS_MASK_LEFT         0x01u
#define IO_STATUS_MASK_UP           0x02u
#define IO_STATUS_MASK_RIGHT        0x10u
#define IO_STATUS_MASK_DOWN         0x20u
#define IO_STATUS_MASK_ANY_BUTTON   0x33u
#define IO_STATUS_MASK_CHARGER      0x04u


void io_init(void);
void io_debounce_inputs(void);
void io_main_power_on(void);
void io_main_power_off(void);
void io_set_bootsel_low(void);
void io_set_bootsel_high(void);
void io_led_on(void);
void io_led_off(void);
void io_led_toggle(void);
void io_prepare_sleep(void);
void io_exit_sleep(void);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */

