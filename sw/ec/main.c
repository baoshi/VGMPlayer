#include <xc.h>         /* XC8 General Include File */
#include <stdint.h>
#include "global.h"
#include "clock.h"
#include "tick.h"
#include "interrupt.h"
#include "io.h"
#include "led.h"
#include "adc.h"
#include "uplink.h"
#include "state.h"

void    state_start_enter(void);
uint8_t state_start_loop(void);
void    state_start_exit(void);

uint8_t state_sleep_loop(void);


void    state_wakeup_enter(void);
uint8_t state_wakeup_loop(void);


void    state_charge_enter(void);
uint8_t state_charge_loop(void);
void    state_charge_exit(void);

void    state_mainloop_enter(void);
uint8_t state_mainloop_loop(void);
void    state_mainloop_exit(void);

void    state_off_enter(void);
uint8_t state_off_loop(void);

void    state_pre_dfu_enter(void);
uint8_t state_pre_dfu_loop(void);

void    state_dfu_enter(void);
uint8_t state_dfu_loop(void);
void    state_dfu_exit(void);


// State Machine
static const struct 
{
    void (*enter)(void);
    uint8_t (*loop)(void);
    void (*exit)(void);
} fsm[] = 
{
    /* MAIN_STATE_START        */ {        state_start_enter,       state_start_loop,             state_start_exit },
    /* MAIN_STATE_SLEEP        */ {                        0,       state_sleep_loop,                            0 },
    /* MAIN_STATE_WAKE         */ {       state_wakeup_enter,      state_wakeup_loop,                            0 },
    /* MAIN_STATE_MAINLOOP     */ {     state_mainloop_enter,    state_mainloop_loop,          state_mainloop_exit },
    /* MAIN_STATE_OFF          */ {          state_off_enter,         state_off_loop,                            0 },
    /* MAIN_STATE_CHARGE       */ {       state_charge_enter,      state_charge_loop,            state_charge_exit },
    /* MAIN_STATE_PRE_DFU      */ {      state_pre_dfu_enter,     state_pre_dfu_loop,                            0 },
    /* MAIN_STATE_DFU          */ {          state_dfu_enter,         state_dfu_loop,               state_dfu_exit }
};

void main(void)
{
    uint8_t cur_state;
    uint8_t nxt_state;
    
    clock_config();
    // Initialize and start systick
    tick_init();
    enable_peripheral_int();
    enable_global_int();
        
    // Start from START state
    cur_state = MAIN_STATE_START;
    state_start_enter();
    
    while (1)
    {
        nxt_state = fsm[cur_state].loop();
        if (nxt_state != cur_state)
        {
            if (fsm[cur_state].exit != NULL) 
            {
                fsm[cur_state].exit();
            }
            cur_state = nxt_state;
            if (fsm[cur_state].enter != NULL) 
            {
                fsm[cur_state].enter();
            }
        }
        tick_update();
    }
}
