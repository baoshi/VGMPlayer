#include <stdint.h>
#include <stdbool.h>
#include <hardware/gpio.h>
#include <hardware/clocks.h>
#include <hardware/i2c.h>
#include "hw_conf.h"
#include "wm8978.h"


#define WM8978_ADDR 0x1a
#define WM8978_VMID_STABLIZE_MS  500

//
// WM8978 low level operations
//

// WM8978 does not support register reading. Here are the cache of WM8978 registers, initialized to default values.
// Reading of register is taking directly from this array. Writing of register is to both this table and hardware.
static uint16_t _wm8978_reg[58] =
{
/*       |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   A   |   B   |   C   |   D   |   E   |   F   |   */   
/* 0 */    0x0000, 0x0000, 0x0000, 0x0000, 0x0050, 0x0000, 0x0140, 0x0000, 0x0000, 0x0000, 0x0000, 0x00ff, 0x00ff, 0x0000, 0x0100, 0x00ff,
/* 1 */    0x00ff, 0x0000, 0x012c, 0x002c, 0X002c, 0x002c, 0x002c, 0x0000, 0x0032, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* 2 */    0x0038, 0x000b, 0x0032, 0x0000, 0x0008, 0x000c, 0x0093, 0x00e9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0033, 0x0010, 0x0010, 0x0100,
/* 3 */    0x0100, 0x0002, 0x0001, 0x0001, 0x0039, 0x0039, 0x0039, 0x0039, 0x0001, 0x0001
};



static int _wm8978_write_reg(uint8_t reg, uint16_t val)
{
    int ret;
    uint8_t buf[2];
    val = val & 0x01ff;
    buf[0] = (reg << 1) | ((val >> 8) & 0x01);  // First byte is register address and bit 8 of value
    buf[1] = val & 0xff;                        // 2nd byte is bit[7:0] of value
    i2c_lock();
    ret = i2c_write_timeout_us(I2C_INST, WM8978_ADDR, buf, 2, false, 1000);
    i2c_unlock();
    if (ret != 2)   // FAIL
        return -1;
    // Cache writing
    _wm8978_reg[reg] = val;
    return 0;
}


// Power up stages
// Refer to WM8978_v4.5.pdf, page 83
// == Stage 1 ==
//  Mute all analog outputs
//  Set L/RMIXEN = 1 and DACENL/R = 1 in register R3
//  Set BUFIOEN = 1 and VMIDSEL[1:0] to required valud in R1
// ====
//  Wait for VMID supply to settle (500ms)
// == Stage 2 ==
//  Set BIASEN=1 in R1
//  Set L/ROUT1EN=1 in R2
//  Enable other mixers as required
//  Enable other outputs as required
//  Set remaining registers

void wm8978_powerup_stage1()
{
    _wm8978_write_reg(0x00, 0x00);  // software reset
    // MCLK
    // From RP2040, output 12MHz MCLK to WM8978
    clock_gpio_init(I2S_MCLK_PIN, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_XOSC_CLKSRC, 1);
    // WM8978 internal clock setup
    // Note: The following value are calculated using 48000Hz sample rate and 12MHz MCLK
    // Refer to WM8978_v4.5.pdf, page 77
    // MCLK   |  SAMPLE  |  DESIRED  |    F2    |  PRESCALE  | POSTSCALE |    R    |   N   |    K     |
    // (MHZ)  |   RATE   |  OUTPUT   |   (MHZ)  |   DIVIDE   |  DIVIDE   |  (HEX)  | (HEX) |          |
    // (F1)   |   (HZ)   |   (MHZ)   |          |            |           |         |       |          |
    //--------+----------+-----------+----------+------------+-----------+---------+-------+----------|
    //  12    |   44100  |   11.29   |  90.3168 |     1      |    2      | 7.5264  |   7   |  86c226  |
    //  12    |   48000  |  12.288   |  98.304  |     1      |    2      | 8.192   |   8   |  3126E9  |
    //
    // Clock PLLPRESCALE = 1, POSTSCALE = 2, PLLN = 8, PLLK = 3126E9
    _wm8978_write_reg(36, 0x08);    // R36 [4]   PLLPRESCALE = 0 (MCLK input not divided)
                                    // R36 [3:0] PLLN = 1000 ( / 8)
                                    // 0 0000 1000      
    // Set K                                    
    _wm8978_write_reg(37, 0x0c);    // R37 [5:0] PLLK[23:18]
    _wm8978_write_reg(38, 0x93);    // R38 [8:0] PLLK[17:9]
    _wm8978_write_reg(39, 0xe9);    // R39 [8:0] PLLK[8:0]
                                    // 3126E9 = 0011 0001 0010 0110 1110 1001
                                    //          001100 010010011 011101001
                                    //              0c        93        e9     
    // Set Mode
    _wm8978_write_reg(6, 0x140);    // R6  [8]   CLKSEL = 1 (Clock from PLL output)
                                    // R6  [7:5] MCLKDIV = 010 (POSTSCALE = 2)
                                    // R6  [4:2] BCLKDIV = 000 (divide by 1) (BCLK = SYSCLK, for use when in master mode)
                                    // R6  [0]   MS = 0  (Chip in slave mode, BCLK and LRC clock are inputs)
                                    // 1 0100 0000
    // Setup digital interface
    _wm8978_write_reg(4, 0x10);     // R4  [8]   BCP = 0 normal BCLK polarity
                                    // R4  [7]   LRP = 0 normal LRCLK polarity
                                    // R4  [6:5] WL = 00 16 bits
                                    // R4  [4:3] FMT = 10 I2S format
                                    // R4  [2]   DACLRSWAP = 0 normal LR phase for DAC
                                    // R4  [1]   ADCLRSWAP = 0 normal LR phase for ADC
                                    // R4  [0]   MONO = 0 Stereo operation
                                    // 0 0001 0000                                    
    // Enable PLL
    _wm8978_write_reg(1, 0x20);     // R1  [8]   BUFDCOPEN = 0 (1 for SPKBOOST)
                                    // R1  [7]   OUT4MIXEN = 0
                                    // R1  [6]   OUT3MIXEN = 0
                                    // R1  [5]   PLLEN = 1
                                    // R1  [4]   MICBEN = 0
                                    // R1  [3]   BIASEN = 0
                                    // R1  [2]   BUFIOEN = 0
                                    // R1  [1:0] VMIDSEL = 00
                                    // 0 0010 0000
    // Mute all analog output
    _wm8978_write_reg(52, 0x40);    // R52 [6]   LOUT1MUTE = 1
    _wm8978_write_reg(53, 0x40);    // R53 [6]   ROUT1MUTE = 1
    _wm8978_write_reg(54, 0x40);    // R54 [6]   LOUT2MUTE = 1
    _wm8978_write_reg(55, 0x40);    // R55 [6]   ROUT2MUTE = 1
    _wm8978_write_reg(56, 0x40);    // R56 [6]   OUT3MUTE  = 1
    _wm8978_write_reg(57, 0x40);    // R57 [6]   OUT4MUTE  = 1                                    
    // Set L/RMIXEN = 1 and DACENL/R = 1 in R3
    _wm8978_write_reg(3, 0x0f);     // R3  [8]   OUT4EN = 0
                                    // R3  [7]   OUT3EN = 0
                                    // R3  [6]   LOUT2EN = 0
                                    // R3  [5]   ROUT2EN = 0
                                    // R3  [3]   RMIXEN = 1 
                                    // R3  [2]   LMIXEN = 1
                                    // R3  [1]   DACENR = 1
                                    // R3  [0]   DACENL = 1
                                    // 0 0000 1111
    // Enable VMID and BUFIOEN
    _wm8978_write_reg(1, 0x26);     // R1  [8]   BUFDCOPEN = 0 (1 for SPKBOOST)
                                    // R1  [7]   OUT4MIXEN = 0
                                    // R1  [6]   OUT3MIXEN = 0
                                    // R1  [5]   PLLEN = 1
                                    // R1  [4]   MICBEN = 0
                                    // R1  [3]   BIASEN = 0
                                    // R1  [2]   BUFIOEN = 1 (To tie off all unused in/out)
                                    // R1  [1:0] VMIDSEL = 10 (300K, slowest startup)
                                    // 0 0010 0110
}


void wm8978_powerup_stage2()
{   
    // Enable BIASEN
    _wm8978_write_reg(1, 0x2e);     // R1  [8]   BUFDCOPEN = 0 (1 for SPKBOOST)
                                    // R1  [7]   OUT4MIXEN = 0
                                    // R1  [6]   OUT3MIXEN = 0
                                    // R1  [5]   PLLEN = 1
                                    // R1  [4]   MICBEN = 0
                                    // R1  [3]   BIASEN = 1
                                    // R1  [2]   BUFIOEN = 1
                                    // R1  [1:0] VMIDSEL = 10 (300K, slowest startup)
                                    // 0 0010 0110
    // Set VROI
    _wm8978_write_reg(49, 0x03);    // R49 [8:7] --
                                    // R49 [6]   DACL2RMIX = 0
                                    // R49 [5]   DACR2LMIX = 0
                                    // R49 [4]   OUT4BOOST = 0
                                    // R49 [3]   OUT3BOOST = 0
                                    // R49 [2]   SPKBOOST = 0
                                    // R49 [1]   TSDEN = 1
                                    // R49 [0]   VROI = 1 (30kohm)
                                    // 0 0000 0011
    _wm8978_write_reg(52, 0xc0);    // R52 [8]   HPVU = 0
                                    // R52 [7]   LOUT1ZC = 1
                                    // R52 [6]   LOUT1MUTE = 1
                                    // R52 [5:0] LOUT1VOL = 0
                                    // 0 1100 0000
    _wm8978_write_reg(52, 0x1c0);   // R53 [8]   HPVU = 1
                                    // R53 [7]   ROUT1ZC = 1
                                    // R53 [6]   ROUT1MUTE = 1
                                    // R53 [5:0] ROUT1VOL = 0
                                    // 1 1100 0000   
    // Enable L/ROUT1
    _wm8978_write_reg(2, 0x180);    // R2  [8]   ROUT1EN = 1
                                    // R2  [7]   LOUT1EN = 1
                                    // R2  [6]   SLEEP = 0
                                    // R2  [5]   BOOSTENR = 0
                                    // R2  [4]   BOOSTENL = 0
                                    // R2  [3]   INPPGAENR = 0
                                    // R2  [2]   INPPGAENL = 0
                                    // R2  [1]   ADCENR = 0
                                    // R2  [0]   ADCENL = 0
                                    // 1 1000 0000
    //_wm8978_write_reg(10, 0x08);    // R10 [8:7] 00
                                    // R10 [6]   SOFTMUTE = 0 (disable)
                                    // R10 [5:4] 00  
                                    // R10 [3]   DACOSR128 = 1 (best SNR)
                                    // R10 [2]   AMUTE = 0 (disable)
                                    // R10 [1]   DACPOLR = 0
                                    // R10 [0]   DACPOLL = 0
                                    // 0 0000 1000
    // Additional Ctrl                  
    //_wm8978_write_reg(7, 0x01);     // R7  [8:4] -
                                    // R7  [3:1] SR = 000 ~ 48kHz SR
                                    // R7  [0]   SLOWCLKEN = 1 Slow clock enable for jack detection and zero-cross timeout
                                    // 0 0000 0001
    // Jack detection setup
    // Jack detection is on GPIO2
    // When earpiece not present, GPIO2 = 0, enable LOUT2/ROUT2 (Speaker), disable LOUT1/ROUT1
    // When earpiece present, GPIO2 = 1, enable LOUT1/ROUT1, disable LOUT2/ROUT2

    //_wm8978_write_reg(13, 0x12);    // R13 [8]   -
                                    // R13 [7]   OUT4_EN_1 = 0
                                    // R13 [6]   OUT3_EN_1 = 0
                                    // R13 [5]   OUT2_EN_1 = 0
                                    // R13 [4]   OUT1_EN_1 = 1
                                    // R13 [3]   OUT4_EN_0 = 0
                                    // R13 [2]   OUT3_EN_0 = 0
                                    // R13 [1]   OUT2_EN_0 = 1
                                    // R13 [0]   OUT1_EN_0 = 0
                                    // 0 0001 0010
    //_wm8978_write_reg(9, 0x50);     // R9  [8]   VMID_EN_1 = 0 jack detection does not affect VMID
                                    // R9  [7]   VMID_EN_0 = 0
                                    // R9  [6]   JD_EN = 1 jack detection enable
                                    // R9  [5:4] JD_SEL = 01 jack detection on GPIO2
                                    // R9  [3:0] -
                                    // 0 0101 0000

    //_wm8978_write_reg(3, 0x6f);     // R3  [8]   OUT4EN = 0
                                    // R3  [7]   OUT3EN = 0
                                    // R3  [6]   LOUT2EN = 1
                                    // R3  [5]   ROUT2EN = 1
                                    // R3  [3]   RMIXEN = 1 
                                    // R3  [2]   LMIXEN = 1
                                    // R3  [1]   DACENR = 1
                                    // R3  [0]   DACENL = 1
                                    // 0 0110 1111
                                        
    //_wm8978_write_reg(52, 0x90);    // LOUT1VOL  0 1001 0000
    //_wm8978_write_reg(53, 0x190);   // ROUT1VOL, HPVU=1  1 1001 0000

    //_wm8978_write_reg(49, 0x02);    // TSDEN[1]=1 SPKBOOST = 0 0 00000010
    
    //_wm8978_write_reg(43, 0x10);    // 0 0001 0000 INVROUT2[4] = 1

    //_wm8978_write_reg(54, 0xb8);    // LOUT2VOL 0 1011 1000
    //_wm8978_write_reg(54, 0x1b8);   // ROUT2VOL 0 1011 1000
 
    
    //_wm8978_write_reg(50, 0x01);       // DACL2LMIX=1
    //_wm8978_write_reg(51, 0x01);       // DACR2RMIX=1
    
    //_wm8978_write_reg(52, 0x90);       // LOUT1VOL=0dB
    //_wm8978_write_reg(53, 0x190);      // ROUT1VOL=0dB, HPVU=1 to write both volume together
    //_wm8978_write_reg(52, 0x10);       // LOUT1VOL
    //_wm8978_write_reg(53, 0x110);      // ROUT1VOL, HPVU=1 to write both volume together
    //_wm8978_write_reg(0x2b, 0x10);     // INVROUT2=1, Mute input to INVROUT2
    //_wm8978_write_reg(0x2f, 0x100);    // PGABOOSTL, L Ch PGA output +20dB through input BOOST stage
    //_wm8978_write_reg(0x30, 0x100);    // PGABOOSTR, R Ch PGA output +20dB through input BOOST stage
}



// Power down steps
// Refer to WM8978_v4.5.pdf, page 84
// Mute all analog outputs
// R1 = 0x00
// R2 = 0x00
// R3 = 0x00
void wm8978_powerdown()
{
    // Mute all analog output
    _wm8978_write_reg(52, 0x40);    // R52 [6]   LOUT1MUTE = 1
    _wm8978_write_reg(53, 0x40);    // R53 [6]   ROUT1MUTE = 1
    _wm8978_write_reg(54, 0x40);    // R54 [6]   LOUT2MUTE = 1
    _wm8978_write_reg(55, 0x40);    // R55 [6]   ROUT2MUTE = 1
    _wm8978_write_reg(56, 0x40);    // R56 [6]   OUT3MUTE  = 1
    _wm8978_write_reg(57, 0x40);    // R57 [6]   OUT4MUTE  = 1
    _wm8978_write_reg(1, 0x00);
    _wm8978_write_reg(2, 0x00);
    _wm8978_write_reg(3, 0x00);
}


void wm8978_set_volume(wm8978_output_t chn, uint8_t vol)
{
    if (vol == 0)
    {
        _wm8978_write_reg(52, 0x80);    // LOUT1VOL mute 0 1000 0000
        _wm8978_write_reg(53, 0x180);   // ROUT1VOL mute 1 1000 0000  
    }
    else
    {
        _wm8978_write_reg(52, 0x90);    // LOUT1VOL  0 1001 0000
        _wm8978_write_reg(53, 0x190);   // ROUT1VOL, HPVU=1  1 1001 0000
    }
}
