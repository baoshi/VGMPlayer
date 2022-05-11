#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "clock.h"
#include "tick.h"
#include "isr.h"
#include "io.h"
#include "state.h"


// State Machine
static const struct 
{
    void (*entry)(void);
    uint8_t (*update)(void);
    void (*exit)(void);
} fsm[] = 
{
    /* MAIN_STATE_SLEEP   */ {   state_sleep_entry,      state_sleep_update,                        0 },
    /* MAIN_STATE_PRE_MAIN*/ { state_premain_entry,    state_premain_update,       state_premain_exit },
    /* MAIN_STATE_PRE_DFU */ {  state_predfu_entry,     state_predfu_update,        state_predfu_exit },
    /* MAIN_STATE_MAIN    */ {    state_main_entry,       state_main_update,          state_main_exit },
    /* MAIN_STATE_DFU     */ {     state_dfu_entry,        state_dfu_update,           state_dfu_exit },
    /* MAIN_STATE_OFF     */ {     state_off_entry,        state_off_update,                        0 }
};


void main(void)
{
    uint8_t cur_state;
    uint8_t nxt_state;
    
    clock_config();
    tick_init();
    enable_peripheral_int();
    enable_global_int();
        
    // Start from SLEEP state
    cur_state = MAIN_STATE_SLEEP;
    state_sleep_entry();
            
    while (1)
    {
        nxt_state = fsm[cur_state].update();
        if (nxt_state != cur_state)
        {
            if (fsm[cur_state].exit != NULL) 
            {
                fsm[cur_state].exit();
            }
            cur_state = nxt_state;
            if (fsm[cur_state].entry != NULL) 
            {
                fsm[cur_state].entry();
            }
        }
    }
}
