#include <xc.h>
#include <stdint.h>
#include "tick.h"


// Use TMR0 as sys tick, each tick is 1ms
// TMR0 config:
// PSA not used, clock source is FOSC/4 (1MHz/4 = 250kHz)
// Count upwards from 6 -> 255 = 250 cycles -> 1kHz

uint16_t systick;

volatile static uint8_t _tick_update;

#define TMR0_PRELOAD 0x06
  
void tick_init(void)
{
    systick = 0;
    _tick_update = 0;
    // OPTION_REG [~WPUEN INTEDG TMR0CS TMR0SE PSA PS<2:0>]
    //                X     X      0      1     1    000
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.TMR0SE = 1;
    OPTION_REGbits.PSA = 1;
    OPTION_REGbits.PS = 0b000;
    // Preload TMR0
    TMR0 = TMR0_PRELOAD;
    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;
    // Enabling TMR0 interrupt
    INTCONbits.TMR0IE = 1;
}


// Call this in main loop
void tick_update(void)
{
    if (_tick_update)
    {
        ++systick;
        _tick_update = 0;
    }
}


void tick_tmr0_isr(void)
{
    // Clear the TMR0 interrupt flag
    INTCONbits.TMR0IF = 0;
    TMR0 = TMR0_PRELOAD;
    _tick_update = 1;
}


void tick_waste_ms(uint16_t ms)
{
    uint16_t start = systick;
    while (systick - start < ms)
    {
        tick_update();
    }
}