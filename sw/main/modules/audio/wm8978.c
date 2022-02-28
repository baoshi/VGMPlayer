#include <stdint.h>
#include <stdbool.h>
#include <hardware/gpio.h>
#include <hardware/clocks.h>
#include <hardware/i2c.h>
#include "hw_conf.h"
#include "my_debug.h"
#include "wm8978_reg.h"
#include "wm8978.h"


static uint16_t wmc_regs[WMC_NUM_REGISTERS] =
{
    /* Initialized with post-reset default values - the 2-wire interface
     * cannot be read. Or-in additional bits desired for some registers. */
    [0 ... WMC_NUM_REGISTERS-1]   = 0x8000, /* To ID invalids in gaps */
    [WMC_SOFTWARE_RESET]          = 0x000,
    [WMC_POWER_MANAGEMENT1]       = 0x000,
    [WMC_POWER_MANAGEMENT2]       = 0x000,
    [WMC_POWER_MANAGEMENT3]       = 0x000,
    [WMC_AUDIO_INTERFACE]         = 0x050,
    [WMC_COMPANDING_CTRL]         = 0x000,
    [WMC_CLOCK_GEN_CTRL]          = 0x140,
    [WMC_ADDITIONAL_CTRL]         = 0x000,
    [WMC_GPIO]                    = 0x000,
    [WMC_JACK_DETECT_CONTROL1]    = 0x000,
    [WMC_DAC_CONTROL]             = 0x000,
    [WMC_LEFT_DAC_DIGITAL_VOL]    = 0x0ff, /* Latch left first */
    [WMC_RIGHT_DAC_DIGITAL_VOL]   = 0x0ff | WMC_VU,
    [WMC_JACK_DETECT_CONTROL2]    = 0x000,
    [WMC_ADC_CONTROL]             = 0x100,
    [WMC_LEFT_ADC_DIGITAL_VOL]    = 0x0ff, /* Latch left first */
    [WMC_RIGHT_ADC_DIGITAL_VOL]   = 0x0ff | WMC_VU,
    [WMC_EQ1_LOW_SHELF]           = 0x12c,
    [WMC_EQ2_PEAK1]               = 0x02c,
    [WMC_EQ3_PEAK2]               = 0x02c,
    [WMC_EQ4_PEAK3]               = 0x02c,
    [WMC_EQ5_HIGH_SHELF]          = 0x02c,
    [WMC_DAC_LIMITER1]            = 0x032,
    [WMC_DAC_LIMITER2]            = 0x000,
    [WMC_NOTCH_FILTER1]           = 0x000,
    [WMC_NOTCH_FILTER2]           = 0x000,
    [WMC_NOTCH_FILTER3]           = 0x000,
    [WMC_NOTCH_FILTER4]           = 0x000,
    [WMC_ALC_CONTROL1]            = 0x038,
    [WMC_ALC_CONTROL2]            = 0x00b,
    [WMC_ALC_CONTROL3]            = 0x032,
    [WMC_NOISE_GATE]              = 0x000,
    [WMC_PLL_N]                   = 0x008,
    [WMC_PLL_K1]                  = 0x00c,
    [WMC_PLL_K2]                  = 0x093,
    [WMC_PLL_K3]                  = 0x0e9,
    [WMC_3D_CONTROL]              = 0x000,
    [WMC_BEEP_CONTROL]            = 0x000,
    [WMC_INPUT_CTRL]              = 0x033,
    [WMC_LEFT_INP_PGA_GAIN_CTRL]  = 0x010 | WMC_ZC, /* Latch left first */
    [WMC_RIGHT_INP_PGA_GAIN_CTRL] = 0x010 | WMC_VU | WMC_ZC,
    [WMC_LEFT_ADC_BOOST_CTRL]     = 0x100,
    [WMC_RIGHT_ADC_BOOST_CTRL]    = 0x100,
    [WMC_OUTPUT_CTRL]             = 0x002,
    [WMC_LEFT_MIXER_CTRL]         = 0x001,
    [WMC_RIGHT_MIXER_CTRL]        = 0x001,
    [WMC_LOUT1_HP_VOLUME_CTRL]    = 0x039 | WMC_ZC, /* Latch left first */
    [WMC_ROUT1_HP_VOLUME_CTRL]    = 0x039 | WMC_VU | WMC_ZC,
    [WMC_LOUT2_SPK_VOLUME_CTRL]   = 0x039 | WMC_ZC, /* Latch left first */
    [WMC_ROUT2_SPK_VOLUME_CTRL]   = 0x039 | WMC_VU | WMC_ZC,
    [WMC_OUT3_MIXER_CTRL]         = 0x001,
    [WMC_OUT4_MONO_MIXER_CTRL]    = 0x001,
};


static inline void wmc_write_reg(uint8_t reg, uint16_t val)
{
    uint8_t buf[2];
    val = val & 0x01ff;
    buf[0] = (reg << 1) | ((val & 0x100) >> 8);  // First byte is register address and bit 8 of value
    buf[1] = val & 0xff;                        // 2nd byte is bit[7:0] of value
    i2c_lock();
    i2c_write_timeout_us(I2C_INST, WMC_I2C_ADDR, buf, 2, false, 1000);
    i2c_unlock();
}


static void wmc_write(uint8_t reg, uint16_t val)
{
    MY_ASSERT(reg < WMC_NUM_REGISTERS);
    MY_ASSERT((wmc_regs[reg] & 0x8000) == 0);
    wmc_regs[reg] = val & ~0x8000;
    wmc_write_reg(reg, val);
}


static void wmc_set(uint8_t reg, uint16_t bits)
{
    wmc_write(reg, wmc_regs[reg] | bits);
}


static void wmc_clear(uint8_t reg, uint16_t bits)
{
    wmc_write(reg, wmc_regs[reg] & ~bits);
}


static void wmc_write_masked(uint8_t reg, uint16_t bits, uint16_t mask)
{
    wmc_write(reg, (wmc_regs[reg] & ~mask) | (bits & mask));
}


void wm8978_preinit()
{
    wmc_write(WMC_SOFTWARE_RESET, 0x00);    // For pop debugger, not needed if only does once after power up
    // Mute all analog outputs
    wmc_set(WMC_LOUT1_HP_VOLUME_CTRL, WMC_MUTE);
    wmc_set(WMC_ROUT1_HP_VOLUME_CTRL, WMC_MUTE);
    wmc_set(WMC_LOUT2_SPK_VOLUME_CTRL, WMC_MUTE);
    wmc_set(WMC_ROUT2_SPK_VOLUME_CTRL, WMC_MUTE);
    wmc_set(WMC_OUT3_MIXER_CTRL, WMC_MUTE);
    wmc_set(WMC_OUT4_MONO_MIXER_CTRL, WMC_MUTE);
    // Set L/RMIXEN = 1 in register R3
    wmc_write(WMC_POWER_MANAGEMENT3, WMC_RMIXEN | WMC_LMIXEN);
    // EQ and 3D applied to DAC (Set before DAC enable!)
    wmc_set(WMC_EQ1_LOW_SHELF, WMC_EQ3DMODE);
    // Set DACENL/R = 1 in register R3
    wmc_set(WMC_POWER_MANAGEMENT3, WMC_DACENR | WMC_DACENL);
    // Set BUFIOEN = 1 and VMIDSEL[1:0] to required value in register R1. Wait for VMID supply to settle
    wmc_write(WMC_POWER_MANAGEMENT1, WMC_BUFIOEN | WMC_VMIDSEL_5K);
    // Should wait for sufficient time before turning on outputs, we take this time to setup digital interface
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
    wmc_write(WMC_PLL_N, 8);
    // 3126E9 = 0011 0001 0010 0110 1110 1001
    //          001100 010010011 011101001
    //              0c        93        e9     
    wmc_write(WMC_PLL_K1, 0x0c);
    wmc_write(WMC_PLL_K2, 0x93);
    wmc_write(WMC_PLL_K3, 0xe9);
    wmc_write(WMC_CLOCK_GEN_CTRL, WMC_CLKSEL | WMC_MCLKDIV_2);
    wmc_set(WMC_POWER_MANAGEMENT1, WMC_PLLEN);
    // Audio Format
    wmc_write(WMC_AUDIO_INTERFACE, WMC_WL_16 | WMC_FMT_I2S);
    // DACOSR
    wmc_write(WMC_DAC_CONTROL, WMC_DACOSR_128);
    // L/ROUT2 to drive BTL speaker
    wmc_set(WMC_BEEP_CONTROL, WMC_INVROUT2);
    // Slow clock and SR
    wmc_write(WMC_ADDITIONAL_CTRL, WMC_SR_48KHZ | WMC_SLOWCLKEN);
    // Jack detection is on GPIO2
    wmc_write(WMC_JACK_DETECT_CONTROL1, WMC_JD_EN | WMC_JD_SEL_GPIO2);
    // When earpiece not present, GPIO2 = 0, enable LOUT2/ROUT2 (Speaker), WMC_OUT2_EN0
    // When earpiece present, GPIO2 = 1, enable LOUT1/ROUT1, WMC_OUT1_EN1
    wmc_write(WMC_JACK_DETECT_CONTROL2, WMC_OUT1_EN1 | WMC_OUT2_EN0);
    // Initial volume (still muted)
    wmc_write_masked(WMC_LOUT1_HP_VOLUME_CTRL, 20, WMC_AVOL);
    wmc_write_masked(WMC_ROUT1_HP_VOLUME_CTRL, 20, WMC_AVOL);
    wmc_write_masked(WMC_LOUT2_SPK_VOLUME_CTRL, 50, WMC_AVOL);
    wmc_write_masked(WMC_ROUT2_SPK_VOLUME_CTRL, 50, WMC_AVOL);

}


void wm8978_postinit()
{
    // Enable Thermal Shutdown
    wmc_set(WMC_OUTPUT_CTRL, WMC_TSDEN);
    // No ADC, no HP filter, no popping
    wmc_clear(WMC_ADC_CONTROL, WMC_HPFEN);
    wmc_clear(WMC_LEFT_ADC_BOOST_CTRL, WMC_PGABOOSTL);
    wmc_clear(WMC_RIGHT_ADC_BOOST_CTRL, WMC_PGABOOSTR);
    // Set BIASEN = 1 in register R1
    wmc_set(WMC_POWER_MANAGEMENT1, WMC_BIASEN);
    // Set L/ROUT1EN = 1 in register R2
    wmc_set(WMC_POWER_MANAGEMENT2, WMC_LOUT1EN | WMC_ROUT1EN);
    // Set L/ROUT2 = 1 in register R3
    wmc_set(WMC_POWER_MANAGEMENT3, WMC_LOUT2EN | WMC_ROUT2EN);
}


void wm8978_powerdown()
{
    wm8978_mute(true);
    wmc_write(WMC_POWER_MANAGEMENT1, 0x000);
    wmc_write(WMC_POWER_MANAGEMENT2, 0x000);
    wmc_write(WMC_POWER_MANAGEMENT3, 0x000);
    // remove MCLK output
    gpio_init(I2S_MCLK_PIN);
}


void wm8978_mute(bool mute)
{
    if (mute)
    {
        wmc_set(WMC_DAC_CONTROL, WMC_SOFT_MUTE);
        wmc_set(WMC_LOUT1_HP_VOLUME_CTRL, WMC_MUTE);
        wmc_set(WMC_ROUT1_HP_VOLUME_CTRL, WMC_MUTE);
        wmc_set(WMC_LOUT2_SPK_VOLUME_CTRL, WMC_MUTE);
        wmc_set(WMC_ROUT2_SPK_VOLUME_CTRL, WMC_MUTE);
    }
    else
    {
        wmc_clear(WMC_LOUT1_HP_VOLUME_CTRL, WMC_MUTE);
        wmc_clear(WMC_ROUT1_HP_VOLUME_CTRL, WMC_MUTE);
        wmc_clear(WMC_LOUT2_SPK_VOLUME_CTRL, WMC_MUTE);
        wmc_clear(WMC_ROUT2_SPK_VOLUME_CTRL, WMC_MUTE);
        wmc_clear(WMC_DAC_CONTROL, WMC_SOFT_MUTE);
    }
}


