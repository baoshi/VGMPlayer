#include <xc.h>
#include <stdint.h>
#include "tick.h"


// Use TMR0 as sys tick, each tick is 20ms (50Hz)
// TMR0 config:
// clock source is FOSC/4 (1MHz/4 = 250kHz)
// Prescaler 1:32, PSA = 0, count 156
// Count upwards from 100 -> 255 = 156 cycles -> 50Hz

volatile uint8_t systick;

#define TMR0_PRELOAD 100
  
void tick_init(void)
{
    systick = 0;
    // OPTION_REG [~WPUEN INTEDG TMR0CS TMR0SE PSA PS<2:0>]
    //                X     X      0      1     0    100
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.TMR0SE = 1;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b100;
    // Preload TMR0
    TMR0 = TMR0_PRELOAD;
    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;
    // Enabling TMR0 interrupt
    INTCONbits.TMR0IE = 1;
}


void tick_tmr0_isr(void)
{
    // Reload
    TMR0 = TMR0_PRELOAD;
    // Clear the TMR0 interrupt flag
    INTCONbits.TMR0IF = 0;
    ++systick;
}
