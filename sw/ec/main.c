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


void    state_charge_enter(void);
uint8_t state_charge_loop(void);
void    state_charge_exit(void);

void    state_premainloop_enter(void);
uint8_t state_premainloop_loop(void);
void    state_premainloop_exit(void);


// State Machine
static const struct 
{
    uint8_t state;
    void (*enter)(void);
    uint8_t (*loop)(void);
    void (*exit)(void);
} fsm[] = 
{
    { MAIN_STATE_START,               state_start_enter,       state_start_loop,             state_start_exit },
    { MAIN_STATE_SLEEP,                               0,       state_sleep_loop,                            0 },
    { MAIN_STATE_PRE_MAINLOOP,  state_premainloop_enter, state_premainloop_loop,       state_premainloop_exit },
    { MAIN_STATE_MAINLOOP,                            0,                      0,                            0 },
    { MAIN_STATE_PRE_OFF,                             0,                      0,                            0 },
    { MAIN_STATE_OFF,                                 0,                      0,                            0 },
    { MAIN_STATE_CHARGE,             state_charge_enter,      state_charge_loop,            state_charge_exit },
    { MAIN_STATE_PRE_DFU,                             0,                      0,                            0 },
    { MAIN_STATE_DFU,                                 0,                      0,                            0 }
};


void main(void)
{
    uint8_t cur_state, nxt_state;
    
    // Initialize and start systick
    tick_init();
    enable_peripheral_int();
    enable_global_int();
        
    // Start from START state
    cur_state = MAIN_STATE_START;
    state_start_enter();
    
    while (1)
    {
        if (fsm[cur_state].loop)
        {
            nxt_state = fsm[cur_state].loop();
        }
        if (nxt_state != cur_state)
        {
            // state switch
            if (fsm[cur_state].exit) fsm[cur_state].exit();
            if (fsm[nxt_state].enter) fsm[nxt_state].enter();
            cur_state = nxt_state;
        }
        tick_update();
    }
}


#if 0

void main(void)
{
    uint8_t status;
    DECLARE_REPEAT_BLOCK(EVERY500MS);
    clock_config();
    io_init();
    tick_init();
    adc_start();
    uplink_start();
    enable_peripheral_int();
    enable_global_int();

    tick_waste_ms(500);
    io_main_power_on();
    
    while (1)
    {
        tick_update();
        adc_update();
        uplink_set_b1(adc_bat);
        io_debounce_inputs();
        // Status  [ X X USB CHG L R U D]
        status = 0x00;
        if (!(io_input_state & IO_STATUS_MASK_DOWN)) status = 0x01;
        if (!(io_input_state & IO_STATUS_MASK_UP)) status |= 0x02;
        if (!(io_input_state & IO_STATUS_MASK_RIGHT)) status |= 0x04;
        if (!(io_input_state & IO_STATUS_MASK_LEFT)) status |= 0x08;
        if (!(io_input_state & IO_STATUS_MASK_CHARGER)) status |= 0x10;
        if (adc_vdd >= 138) status |= 0x20;
        uplink_set_b0(status);
        REPEAT_BLOCK_BEGIN(EVERY500MS, 200)
        LED_Toggle();
        REPEAT_BLOCK_END(EVERY500MS)
    }
}

#endif


#if 0
void main(void)
{
    DECLARE_REPEAT_BLOCK(EVERY5MS);
    DECLARE_REPEAT_BLOCK(EVERY200MS);
    
    clock_config();
    
    io_prepare_sleep();
    
    VREGCONbits.VREGPM = 1;
    SLEEP();    // 7uA
    
    // -- Wakeup --
    
    io_init();
    
    tick_init();
    
    bms_start();
    uplink_start();
    
    io_main_power_on();
    
    // Enable interrupts
    enable_peripheral_int();
    enable_global_int();
    
    while (1)
    {
        tick_update();
        REPEAT_BLOCK_BEGIN(EVERY5MS, 5)
        io_debounce_inputs();
        uplink_set_b1(io_input_state);
        bms_update();
        uplink_set_b0(bms_battery_voltage);
        REPEAT_BLOCK_END(EVERY5MS)
                
        REPEAT_BLOCK_BEGIN(EVERY200MS, 200)
        LED_Toggle();
        REPEAT_BLOCK_END(EVERY200MS)
#if 0                
        if (io_input_state & 0x02)  // Up
            LED_Off();
        else
            LED_On();               // Press
#endif
        
        if (!(io_input_state & 0x04))       // Down pressed
            bms_stop_charge();      
        else if (!(io_input_state & 0x08))  // Right pressed
            bms_set_fast_charge();  
        else
            bms_set_slow_charge();
    }
    
}

#endif

#if 0

#define LOW_BATT_THRESHOLD 155  // around 3.4v

void main(void)
{
    DECLARE_REPEAT_BLOCK(EVERY5MS);
    DECLARE_REPEAT_BLOCK(EVERY200MS);
    DECLARE_REPEAT_BLOCK(EVERY1S);
    
    clock_config();
    io_init();
    tick_init();
    
    bms_start();
    
     // Enable interrupts
    enable_peripheral_int();
    enable_global_int();
    
    // prevent startup if battery is low
    while (1)
    {
        tick_update();
        io_main_power_off();

        bms_set_fast_charge();
        bms_update();
        if (bms_battery_voltage < LOW_BATT_THRESHOLD)
            break;

        REPEAT_BLOCK_BEGIN(EVERY1S, 1000)
        LED_Toggle();
        REPEAT_BLOCK_END(EVERY1S)
    }
    
    // Have enough battery to continue
    
    bms_set_slow_charge();
    io_main_power_on();
    uplink_start();
    
    while (1)
    {
        tick_update();
        
        REPEAT_BLOCK_BEGIN(EVERY5MS, 5)
        io_debounce_inputs();
        uplink_set_b1(io_input_state);
        bms_update();
        uplink_set_b0(bms_battery_voltage);
        REPEAT_BLOCK_END(EVERY5MS)
                
        REPEAT_BLOCK_BEGIN(EVERY200MS, 200)
        LED_Toggle();
        REPEAT_BLOCK_END(EVERY200MS)

        if (!(io_input_state & 0x01))   // Left pressed, control main power
            io_main_power_off();
        else
            io_main_power_on();
        
        if (!(io_input_state & 0x08))  // Right pressed, control bootsel
            io_set_bootsel_low();
        else
            io_set_bootsel_high();
    }
}
#endif