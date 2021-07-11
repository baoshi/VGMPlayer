/*
 * File:   uplink.c
 * Author: Baoshi
 *
 * Created on 2 May, 2021, 12:33 PM
 */


#include <xc.h>
#include "global.h"
#include "tick.h"
#include "led.h"
#include "uplink.h"


#define SLAVE_ADDRESS 0x13u

volatile uint16_t uplink_activity;

#define UPLINK_DATA_COUNT 2u
static uint8_t data[UPLINK_DATA_COUNT];


void uplink_start(void)
{
    // dummy sample data
    data[0] = 0x00;
    data[1] = 0x00;
    
    // Init I2C slave
    // MSSP config
    SSP1STATbits.SMP = 1;           // Slew rate control disabled
    SSP1STATbits.CKE = 1;           // SMBus compatible input (2.8V high)
    SSP1CON1bits.SSPM = 0b0110;     // I2C slave mode, 7-bit
    SSP1CON2bits.SEN = 1;           // Clock stretching enabled
    SSP1CON3bits.SBCDE = 1;         // Enable slave bus collision interrupts
    SSP1ADD = (uint8_t)(SLAVE_ADDRESS << 1); // Load slave address
    SSP1CON1bits.SSPEN = 1;         // Enable MSSP
    // PIR2 config
    PIR2bits.BCL1IF = 0;            // Clear Bus Collision interrupt flag
    PIR1bits.SSP1IF = 0;            // Clear the SSP interrupt flag
    PIE2bits.BCL1IE = 1;            // Enable BCLIF
    PIE1bits.SSP1IE = 1;            // Enable SSPIF
    uplink_activity = systick;
}


void uplink_set_b0(uint8_t val)
{
    data[0] = val;
}


void uplink_set_b1(uint8_t val)
{
    data[1] = val;
}


void i2c_slave_ssp_isr(void)
{
    static uint8_t idx = 0;
    uint8_t temp;
    // Global ISR guarantees PIR1bits.SSP1IF == 1
    if (SSP1STATbits.R_nW == 1) // Master wants to read
    {
        temp = SSP1BUF;
        if (SSP1STATbits.D_nA == 0) // Last byte was address, sending first data byte
        {
            SSP1BUF = data[0];
            idx = 1;
        }
        else  // Already transmitted first data byte, sending next
        {
            if (idx < UPLINK_DATA_COUNT) // Still have data?
            {
                SSP1BUF = data[idx];
                ++idx;
            }
            else // All data are sent
            {
                if (SSP1CON2bits.ACKSTAT == 0)  // Master ACKed the last byte, expect more
                {
                    SSP1BUF = 0xFF;
                }
                // Master NACKed last byte, nothing needed.
                uplink_activity = systick;
            }
        }
    }
    else // Master wants to write
    {
        // Nothing to do here
        temp = SSP1BUF;
    }
    PIR1bits.SSP1IF = 0;    // Clear SSP1IF   
    SSP1CON1bits.CKP = 1;   // Release clock stretch
}


void i2c_slave_bcl_isr(void)
{
    uint8_t temp = SSP1BUF;  // Clear BUF
    PIR2bits.BCL1IF = 0;     // Clear BCLIF
    SSP1CON1bits.CKP = 1;    // Release bus
}
