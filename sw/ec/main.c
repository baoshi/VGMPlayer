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

void main(void)
{
    uint8_t status;
    DECLARE_REPEAT_BLOCK(EVERY500MS);
    clock_config();
    io_config();
    tick_config();
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
    
    io_config();
    
    tick_config();
    
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
    io_config();
    tick_config();
    
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