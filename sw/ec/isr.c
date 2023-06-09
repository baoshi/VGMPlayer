/*
 * File:   interrupt.c
 * Author: Baoshi
 *
 * Created on May 1, 2021, 6:37 PM
 */


#include <xc.h>
#include "tick.h"
#include "i2c.h"
#include "isr.h"

void __interrupt() ISR(void)
{
    if ((INTCONbits.TMR0IE == 1) && (INTCONbits.TMR0IF == 1))       // Timer 0
    {
        tick_tmr0_isr();
    }
    else if ((INTCONbits.IOCIE == 1) && (INTCONbits.IOCIF == 1))    // IOC
    {
        // IOC is for wakeup, just clear interrupt flag
        IOCAF = 0x00;
    }
    else if(INTCONbits.PEIE == 1)
    {
        // Peripheral Interrupts
        if ((PIE2bits.BCL1IE == 1) && (PIR2bits.BCL1IF == 1))
        {
            i2c_slave_bcl_isr();
        } 
        else if ((PIE1bits.SSP1IE == 1) && (PIR1bits.SSP1IF == 1))
        {
            i2c_slave_ssp_isr();
        } 
        else
        {
            // Unhandled Peripheral Interrupt
        }
    }      
    else
    {
        // Unhandled Interrupt
    }
}
