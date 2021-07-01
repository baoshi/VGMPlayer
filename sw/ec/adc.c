/*
 * File:   adc.c
 * Author: Baoshi
 *
 * Created on 3 May, 2021, 11:33 AM
 */

#include <xc.h>
#include <stdint.h>
#include "global.h"
#include "tick.h"
#include "adc.h"


/**
 * Battery Management System
 * Battery is connected to RC3 (AN7) with 15/16 voltage divider
 * To measure battery:
 * 1. Use VDD as reference, measure FVR 2.048V (M1). VDD = 2.048 * 1023 / M1
 * 2. Use VDD as reference, measure AN7 (M2). VDD = VBAT * (15 / 16) * 1023 / M2
 * 3. Calculate VBAT = M2 / M1 * 2.048 * 16 / 15 = 2.185 * M2 / M1
 * 4. To scale result into 8 bit
 *    adc_vdd = VDD * 30
 *    adc_bat = VBAT * 30
 */

uint8_t volatile adc_vdd;
uint8_t volatile adc_bat;


enum 
{
    ADC_STATE_STOPPED = 0,
    ADC_STATE_SETUP_M1,
    ADC_STATE_ACQUISITION_M1,
    ADC_STATE_READ_M1_SETUP_M2,
    ADC_STATE_ACQUISITION_M2,
    ADC_STATE_READ_M2_CALC_VOLT
};
static uint8_t state = 0;

static union u16_u
{
    uint16_t word;
    struct
    {
        uint8_t lo;
        uint8_t hi;
    };
} m1, m2;


void adc_start(void)
{
    adc_vdd = 0;
    adc_bat = 0;
    // 
    // Setup FVR
    // FVRCON [RVREN RVRRDY TSEN TSRNG CDAFVR<1:0> ADFVR<1:0>]
    //        [  1     x     0     0     0            10     ]
    // FVREN enabled; TSEN disabled; ADFVR 2x (2.048V);
    FVRCON = 0x82;
    
    // Setup ADC (Common initialization for M1 and M2)
    // ADCON1 [ADFM ADCS<2:0> u u ADPREF<1:0>] = 1000 0000
    // ADFM=1 (right adjusted); ADCS=000 (FOSC/2); ADPREF=00 (VDD)
    ADCON1 = 0x80;
    // ADCON2 [ TRGSEL<3:0> u u u u] = 0000 0000
    // TRGSEL=0000 no auto trigger
    ADCON2 = 0x00;
    // Start from M1 state
    state = ADC_STATE_SETUP_M1;
}


bool adc_update(void)
{
    switch (state)
    {
    case ADC_STATE_STOPPED:
        break;
    case ADC_STATE_SETUP_M1: // Setup and start M1
        // ADCON0 [ u CHS<4:0> GO_nDONE ADON] = 0111 1100
        // CHS=11111 FVR Buffer output; GO=0; ADON=0
        ADCON0 = 0x7C;
        // Set result 0
        ADRESL = 0x00; ADRESH = 0x00;
        // Start acquisition
        ADCON0bits.ADON = 1;
        // Record start acquisition time, using m1 temporary
        m1.word = systick;
        state = ADC_STATE_ACQUISITION_M1;
        break;
    case ADC_STATE_ACQUISITION_M1:
        if (systick - m1.word > 1)  // give acquisition 2ms or less
        {
            ADCON0bits.GO_nDONE = 1;
            state = ADC_STATE_READ_M1_SETUP_M2;
        }
        break;
    case ADC_STATE_READ_M1_SETUP_M2:
        if (!ADCON0bits.GO_nDONE)   // If conversion is done
        {
            // Read M1 result
            m1.hi = ADRESH;
            m1.lo = ADRESL;
            // Setup M2
            // ADCON0 [ u CHS<4:0> GO_nDONE ADON] = 0001 1100
            // CHS=00111 AN7; GO=0; ADON=0
            ADCON0 = 0x1C;
            // Set result 0
            ADRESL = 0x00; ADRESH = 0x00;
            // Start conversion
            ADCON0bits.ADON = 1;
            // Record start acquisition time, using m2 temporary
            m2.word = systick;
            state = ADC_STATE_ACQUISITION_M2;
        }
        break;
    case ADC_STATE_ACQUISITION_M2:
        if (systick - m2.word > 1) // give acquisition 2ms or less
        {
            ADCON0bits.GO_nDONE = 1;
            state = ADC_STATE_READ_M2_CALC_VOLT;
        }
        break;
    case ADC_STATE_READ_M2_CALC_VOLT:
        if (!ADCON0bits.GO_nDONE)   // If conversion is done
        {
            // Read M2 result
            m2.hi = ADRESH;
            m2.lo = ADRESL;
            // VDD = 2.048 * 1023 / M1
            // Scale x30 at both sides, 30VDD = 62853 / M1
            adc_vdd = (uint8_t)(62853u / m1.word);   // guarantee m1 > 245 (fails if VDD > 8.5v)
            // VBAT = 2.185 * M2 / M1, scale 30 at both sides
            adc_bat = (uint8_t)(66u * m2.word / m1.word); // m2 will max around 960 (1023 * 15 / 16), 66 * m2 won't overflow
            state = ADC_STATE_SETUP_M1;  // restart
            return true;
        }
        break;
    }
    return false;
}


void adc_stop(void)
{
    ADCON0bits.GO_nDONE = 0;
    ADCON0bits.ADON = 0;
    state = 0;
}


void adc_prepare_sleep(void)
{
    // Stop ADC
    ADCON0bits.GO_nDONE = 0;
    ADCON0bits.ADON = 0;
    // Stop FVR
    // FVRCON [RVREN RVRRDY TSEN TSRNG CDAFVR<1:0> ADFVR<1:0>]
    //        [  0     x     0     0     00            00
    // FVREN disable; TSEN disabled; ADFVR off;
    FVRCON = 0x00;
}

