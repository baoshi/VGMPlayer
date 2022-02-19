#ifndef HW_CONFIG_H
#define HW_CONFIG_H

#include <stdbool.h>

//
// Pin Config
//

// SD Card
#define SDCARD_SPI_INST         spi1
#define SDCARD_CLK_PIN          10
#define SDCARD_CMD_PIN          11
#define SDCARD_DAT0_PIN         12
#define SDCARD_DAT1_PIN         13
#define SDCARD_DAT2_PIN         14
#define SDCARD_DAT3_PIN         15
#define SDCARD_CD_PIN           9
// SD Card in SPI mode
#define SDCARD_SPI_MOSI_PIN     SDCARD_CMD_PIN
#define SDCARD_SPI_MISO_PIN     SDCARD_DAT0_PIN
#define SDCARD_SPI_CS_PIN       SDCARD_DAT3_PIN
#define SDCARD_SPI_CLK_PIN      SDCARD_CLK_PIN

// ST7789 LCD
#define ST7789_SPI_INST         spi0
#define ST7789_CLK_PIN          6
#define ST7789_MOSI_PIN         7
#define ST7789_CS_PIN           4
#define ST7789_DC_PIN           3
#define ST7789_RST_PIN          5
#define ST7789_DMA_IRQ          DMA_IRQ_0
#define ST7789_DMA_INTS         ints0
// Check RP2040 Datasheet for slice and channel for PWM
#define ST7789_BCKL_PIN         8
#define ST7789_BCKL_PWM_SLICE   4
#define ST7789_BCKL_PWM_CHANNEL PWM_CHAN_A

// I2C bus shared with embedded controller (ec) and WM8978
#define I2C_INST        i2c0
#define I2C_SDA_PIN     16
#define I2C_SCL_PIN     17
#define I2C_TIMEOUT_US  1000
#define I2C_BAUD_HZ     (400 * 1000)

// AUDIO
#define I2S_DATAOUT_PIN         22
#define I2S_BCLK_PIN            23
#define I2S_LRC_PIN             24
#define I2S_MCLK_PIN            21
#define JACK_DETECTION_PIN      25
#define I2S_PIO                 pio0            // Use pio0 for I2S
#define I2S_PIO_TX_DREQ         DREQ_PIO0_TX0   // I2S TX DREQ on DREQ_PIO0_TX0
#define I2S_DMA_IRQ             DMA_IRQ_1
#define I2S_DMA_INTS            ints1

//
// DMA channel allocation
//
#define DMA_CHANNEL_SDCARD_SPI_TX   0
#define DMA_CHANNEL_SDCARD_SPI_RX   1
#define DMA_CHANNEL_ST7789_SPI_TX   2
#define DMA_CHANNEL_ST7789_SPI_RX   3
#define DMA_CHANNEL_I2S_TX          4


#ifdef __cplusplus
extern "C"
{
#endif

bool i2c_lock();
void i2c_unlock();

void hw_init();


#ifdef __cplusplus
}
#endif

#endif
