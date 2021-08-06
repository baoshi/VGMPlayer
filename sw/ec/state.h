/* 
 * File:   state.h
 * Author: Baoshi
 *
 * Created on 1 July, 2021, 10:39 AM
 */

#ifndef STATE_H
#define	STATE_H

#ifdef	__cplusplus
extern "C"
{
#endif

enum 
{
  MAIN_STATE_START = 0,
  MAIN_STATE_SLEEP,
  MAIN_STATE_MAINLOOP,
  MAIN_STATE_OFF,
  MAIN_STATE_CHARGE,
  MAIN_STATE_PRE_DFU,
  MAIN_STATE_DFU
};

extern uint8_t main_state;

#ifdef	__cplusplus
}
#endif

#endif	/* STATE_H */

