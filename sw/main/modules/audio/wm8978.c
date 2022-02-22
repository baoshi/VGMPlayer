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
    MY_ASSERT(reg >= WMC_NUM_REGISTERS || (wmc_regs[reg] & 0x8000));
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

