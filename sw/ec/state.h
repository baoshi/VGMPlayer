/* 
 * File:   state.h
 * Author: Baoshi
 *
 * Created on Mar 14, 2022, 10:49 AM
 */

#ifndef STATE_H
#define STATE_H

#include <stdint.h>

enum 
{
  MAIN_STATE_SLEEP = 0,
  MAIN_STATE_PRE_MAIN,
  MAIN_STATE_PRE_DFU,
  MAIN_STATE_MAIN,
  MAIN_STATE_DFU,
  MAIN_STATE_TIMEOUT,
  MAIN_STATE_OFF
};

/* MAIN_STATE_SLEEP */
void    state_sleep_entry(void);
uint8_t state_sleep_update(void);

/* MAIN_STATE_PRE_MAIN */
void state_premain_entry(void);
uint8_t state_premain_update(void);
void state_premain_exit(void);

/* MAIN_STATE_PRE_DFU */
void state_predfu_entry(void);
uint8_t state_predfu_update(void);
void state_predfu_exit(void);

/* MAIN_STATE_MAIN */
void state_main_entry(void);
uint8_t state_main_update(void);
void state_main_exit(void);

/* MAIN_STATE_DFU */
void state_dfu_entry(void);
uint8_t state_dfu_update(void);
void state_dfu_exit(void);

/* MAIN_STATE_TIMEOUT */
void state_timeout_entry(void);
uint8_t state_timeout_update(void);

/* MAIN_STATE_OFF */
void state_off_entry(void);
uint8_t state_off_update(void);

#endif /* STATE_H */