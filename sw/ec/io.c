/*
 * File:   io.c
 * Author: Baoshi
 *
 * Created on May 1, 2021, 6:37 PM
 */


#include <xc.h>
#include <stdint.h>
#include "io.h"


#define MAX_DEBOUNCE_READS  8  // # of reads before a state is determined
uint8_t io_input_state = 0x37;
uint8_t io_prev_input_state = 0x37;

void io_debounce_inputs(void)
{
    // RA0 - IO_SW_LEFT
    // RA1 - IO_SW_UP
    // RA2 - IO_CHARGER_STATUS
    // RA3 - 0
    // RA4 - IO_SW_RIGHT
    // RA5 - IO_SW_DOWN
    // Idle: 0 0 1 1 1 1 1 1 (No button pressed, Not charging)
    static uint8_t reading[MAX_DEBOUNCE_READS];// = { 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37 };
    static uint8_t idx = 0;
    uint8_t i, a, b, c;
    
    // Read new inputs
    reading[idx] = PORTA;    
    ++idx;
    if (idx >= MAX_DEBOUNCE_READS) idx = 0;
    // Debounce Algorithm:
    // Refer to http://www.ganssle.com/debouncing.htm
    // Add debounce both edges
    //                       a   b   c
    // reading[] all 1       1   0   1
    // reading[] all 0       0   1   1 
    // reading[] changing    0   0   0
    a = 0xFF; b = 0xFF;
    for (i = 0; i < MAX_DEBOUNCE_READS; ++i)
    {
        a = a & reading[i];
        b = b & ~reading[i];
    }
    c = a ^ b;
    b = io_input_state;    // b no longer needed
    io_input_state = (a & c) | (io_prev_input_state & (~c));
    io_prev_input_state = b;
}


void io_init(void)
{
    // PORTA:
    // RA0 - IO_SW_LEFT         Input, WPU
    // RA1 - IO_SW_UP           Input, WPU
    // RA2 - IO_CHARGER_STATUS  Input, WPU
    // RA4 - IO_SW_RIGHT        Input, WPU
    // RA5 - IO_SW_DOWN         Input, WPU
    // PORTA registers
    LATA = 0x00;    // LATA  [u u 5 4 3 2 1 0]
                    //        0 0 0 0 0 0 0 0 -> All 0
    TRISA = 0x3F;   // TRISA [u u 5 4 U 2 1 0]
                    //        0 0 1 1 1 1 1 1 -> All input
    ANSELA = 0x00;  // ANSELA[u u u 4 u 2 1 0]
                    //        0 0 0 0 0 0 0 0 -> All digital
    WPUA = 0x3F;    // WPUA  [u u 5 4 3 2 1 0]
                    //        0 0 1 1 1 1 1 1 -> Pull-ups
    // All weak pull-up enabled
    OPTION_REGbits.nWPUEN = 0;
    // APFCON not using alternate pin
    APFCON = 0x00;

    // PORTC:
    // RC0 - IO_SCL         Input
    // RC1 - IO_SDA         Input
    // RC2 - IO_PWR_EN      Init input, when enable, output Low
    // RC3 - IO_ADC_BATT    Input, AN7
    // RC4 - IO_LED         Output, init Low
    // RC5 - IO_BOOTSEL     Output, init Low
    // PORTC registers
    LATC = 0x04;    // LATC  [u u 5 4 3 2 1 0]
                    //        0 0 0 0 0 1 0 0 -> RC2 initial to high
    TRISC = 0x0F;   // TRISC [u u 5 4 3 2 1 0]
                    //        0 0 0 0 1 1 1 1 -> RC0, RC1, RC2, RC3 input
    ANSELC = 0x08;  // ANSELC[u u u u 3 2 1 0]
                    //        0 0 0 0 1 0 0 0 -> RC3 is AN7 ADC input
    
    // Disable IOCI interrupt 
    INTCONbits.IOCIE = 0; 
}


void io_main_power_on(void)
{
    LATCbits.LATC2 = 0;
    TRISCbits.TRISC2 = 0;   // Output Low at RC2
}


void io_main_power_off(void)
{
    LATCbits.LATC2 = 1;
    TRISCbits.TRISC2 = 1;   // Set RC2 to input (HiZ)
}


void io_set_bootsel_low(void)
{
    LATCbits.LATC5 = 1;
}


void io_set_bootsel_high(void)
{
    LATCbits.LATC5 = 0;
}


void io_led_on(void)
{
    LATCbits.LATC4 = 1;
}


void io_led_off(void)
{
    LATCbits.LATC4 = 0;
}


void io_led_toggle(void)
{
    LATCbits.LATC4 = ~LATCbits.LATC4;
}


// Preparing sleep
// Set proper levels and enable IOC
void io_prepare_sleep(void)
{
    // IOC
    // IOC PortA flag
    IOCAF = 0x00;   // IOCAF  [u u 5 4 3 2 1 0]
                    //         0 0 0 0 0 0 0 0
    // Negative Edge IOC register
    IOCAN = 0x37;   // IOCAN  [u u 5 4 3 2 1 0]
                    //         0 0 1 1 0 1 1 1  (RA0,1,2,4,5)
    // Positive Edge IOC register
    IOCAP = 0x00;   // IOCAP  [u u 5 4 3 2 1 0]
                    //         0 0 0 0 0 0 0 0  (None)
    // Enable IOCI interrupt 
    INTCONbits.IOCIE = 1; 
}



void io_exit_sleep(void)
{
    IOCAF = 0x00;
    INTCONbits.IOCIE = 0;
}