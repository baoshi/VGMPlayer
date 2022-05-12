/*
 * File:   i2c.c
 * Author: Baoshi
 *
 * Created on 9 May, 2022, 12:33 PM
 */


#include <xc.h>
#include <stdbool.h>
#include "global.h"
#include "tick.h"
#include "adc.h"
#include "io.h"
#include "i2c.h"


#define SLAVE_ADDRESS 0x13u

volatile uint8_t i2c_recent_activity;

static bool _activity;

void i2c_start(void)
{
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
    _activity = false;
    i2c_recent_activity = systick;
}


void i2c_stop(void)
{
    SSP1CON1bits.SSPEN = 0;         // Disable MSSP
    PIR2bits.BCL1IF = 0;            // Clear Bus Collision interrupt flag
    PIR1bits.SSP1IF = 0;            // Clear the SSP interrupt flag
    PIE2bits.BCL1IE = 0;            // Clear BCLIF
    PIE1bits.SSP1IE = 0;            // Clear SSPIF
}


void i2c_track_activity(void)
{
    if (_activity)
    {
        i2c_recent_activity = systick;
        _activity = false;
    }
}


void i2c_slave_ssp_isr(void)
{
    static uint8_t idx = 0;
    uint8_t temp;
    // Global ISR guarantees PIR1bits.SSP1IF == 1
    if (SSP1STATbits.R_nW == 1) // Master wants to read
    {
        temp = SSP1BUF; // read buf
        if (SSP1STATbits.D_nA == 0) // Last byte was address, sending first data byte
        {
            // data0 is io_input_state
            SSP1BUF = (uint8_t)(~io_input_state) & 0x3F;
            idx = 0;
        }
        else  // Already transmitted first data byte, sending next
        {
            ++idx;
            if (idx == 1)
            {
                // data1 is adc_bat
                SSP1BUF = adc_bat;
            }
            else // All data are sent
            {
                if (SSP1CON2bits.ACKSTAT == 0)  // Master ACKed the last byte, expect more
                {
                    SSP1BUF = 0xFF;
                }
                // Master NACKed last byte, nothing needed.
                _activity = true;
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
