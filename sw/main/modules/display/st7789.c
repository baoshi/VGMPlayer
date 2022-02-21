/**
 * @file st7789.c
 *
 * Mostly taken from lvgl/lv_port_esp32
 */

#include <pico.h>
#include <pico/types.h>
#include <pico/sem.h>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/dma.h>
#include <hardware/irq.h>
#include <hardware/spi.h>

#include <lvgl.h>
#include "my_debug.h"
#include "hw_conf.h"
#include "st7789.h"


#ifndef ST7789_DEBUG
# define ST7789_DEBUG 0
#endif

// Debug log
#if ST7789_DEBUG
#define ST_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define ST_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define ST_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define ST_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define ST_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define ST_LOGD(x, ...)
#define ST_LOGI(x, ...)
#define ST_LOGW(x, ...)
#define ST_LOGE(x, ...)
#define ST_DEBUGF(x, ...)
#endif


// Screen resolution
#define LV_HOR_RES_MAX      240
#define LV_VER_RES_MAX      240


// ST7789 commands
#define ST7789_NOP          0x00
#define ST7789_SWRESET      0x01
#define ST7789_RDDID        0x04
#define ST7789_RDDST        0x09

#define ST7789_RDDPM        0x0A    // Read display power mode
#define ST7789_RDD_MADCTL   0x0B    // Read display MADCTL
#define ST7789_RDD_COLMOD   0x0C    // Read display pixel format
#define ST7789_RDDIM        0x0D    // Read display image mode
#define ST7789_RDDSM        0x0E    // Read display signal mode
#define ST7789_RDDSR        0x0F    // Read display self-diagnostic result (ST7789V)

#define ST7789_SLPIN        0x10
#define ST7789_SLPOUT       0x11
#define ST7789_PTLON        0x12
#define ST7789_NORON        0x13

#define ST7789_INVOFF       0x20
#define ST7789_INVON        0x21
#define ST7789_GAMSET       0x26    // Gamma set
#define ST7789_DISPOFF      0x28
#define ST7789_DISPON       0x29
#define ST7789_CASET        0x2A
#define ST7789_RASET        0x2B
#define ST7789_RAMWR        0x2C
#define ST7789_RGBSET       0x2D    // Color setting for 4096, 64K and 262K colors
#define ST7789_RAMRD        0x2E

#define ST7789_PTLAR        0x30
#define ST7789_VSCRDEF      0x33    // Vertical scrolling definition (ST7789V)
#define ST7789_TEOFF        0x34    // Tearing effect line off
#define ST7789_TEON         0x35    // Tearing effect line on
#define ST7789_MADCTL       0x36    // Memory data access control
#define ST7789_IDMOFF       0x38    // Idle mode off
#define ST7789_IDMON        0x39    // Idle mode on
#define ST7789_RAMWRC       0x3C    // Memory write continue (ST7789V)
#define ST7789_RAMRDC       0x3E    // Memory read continue (ST7789V)
#define ST7789_COLMOD       0x3A

#define ST7789_RAMCTRL      0xB0    // RAM control
#define ST7789_RGBCTRL      0xB1    // RGB control
#define ST7789_PORCTRL      0xB2    // Porch control
#define ST7789_FRCTRL1      0xB3    // Frame rate control
#define ST7789_PARCTRL      0xB5    // Partial mode control
#define ST7789_GCTRL        0xB7    // Gate control
#define ST7789_GTADJ        0xB8    // Gate on timing adjustment
#define ST7789_DGMEN        0xBA    // Digital gamma enable
#define ST7789_VCOMS        0xBB    // VCOMS setting
#define ST7789_LCMCTRL      0xC0    // LCM control
#define ST7789_IDSET        0xC1    // ID setting
#define ST7789_VDVVRHEN     0xC2    // VDV and VRH command enable
#define ST7789_VRHS         0xC3    // VRH set
#define ST7789_VDVSET       0xC4    // VDV setting
#define ST7789_VCMOFSET     0xC5    // VCOMS offset set
#define ST7789_FRCTR2       0xC6    // FR Control 2
#define ST7789_CABCCTRL     0xC7    // CABC control
#define ST7789_REGSEL1      0xC8    // Register value section 1
#define ST7789_REGSEL2      0xCA    // Register value section 2
#define ST7789_PWMFRSEL     0xCC    // PWM frequency selection
#define ST7789_PWCTRL1      0xD0    // Power control 1
#define ST7789_VAPVANEN     0xD2    // Enable VAP/VAN signal output
#define ST7789_CMD2EN       0xDF    // Command 2 enable
#define ST7789_PVGAMCTRL    0xE0    // Positive voltage gamma control
#define ST7789_NVGAMCTRL    0xE1    // Negative voltage gamma control
#define ST7789_DGMLUTR      0xE2    // Digital gamma look-up table for red
#define ST7789_DGMLUTB      0xE3    // Digital gamma look-up table for blue
#define ST7789_GATECTRL     0xE4    // Gate control
#define ST7789_SPI2EN       0xE7    // SPI2 enable
#define ST7789_PWCTRL2      0xE8    // Power control 2
#define ST7789_EQCTRL       0xE9    // Equalize time control
#define ST7789_PROMCTRL     0xEC    // Program control
#define ST7789_PROMEN       0xFA    // Program mode enable
#define ST7789_NVMSET       0xFC    // NVM setting
#define ST7789_PROMACT      0xFE    // Program action


// SPI interface related variables
static dma_channel_config _spi_tx_dma_cfg;
static dma_channel_config _spi_rx_dma_cfg;
static semaphore_t _spi_sem;
static const uint32_t _spi_timeout_ms = 10;
static uint8_t _spi_dummy;


// LVGL related variables
static lv_color_t _buf1[LV_HOR_RES_MAX * 8];
static lv_color_t _buf2[LV_HOR_RES_MAX * 8];
static lv_disp_draw_buf_t _buf;
static lv_disp_drv_t _disp_drv;


static void _send_cmd(uint8_t cmd)
{
    if (sem_acquire_timeout_ms(&_spi_sem, _spi_timeout_ms))
    {
        gpio_put(ST7789_DC_PIN, false);
        gpio_put(ST7789_CS_PIN, false);
        spi_write_blocking(ST7789_SPI_INST, &cmd, 1);
        gpio_put(ST7789_CS_PIN, true);
        sem_release(&_spi_sem);
    }
}


static void _send_data(const void *data, uint16_t length)
{
    if (sem_acquire_timeout_ms(&_spi_sem, _spi_timeout_ms))
    {
        gpio_put(ST7789_DC_PIN, true);
        gpio_put(ST7789_CS_PIN, false);
        spi_write_blocking(ST7789_SPI_INST, data, length);
        gpio_put(ST7789_CS_PIN, true);
        sem_release(&_spi_sem);
    }
}


static void _send_color(void *data, uint16_t length)
{
    if (sem_acquire_timeout_ms(&_spi_sem, _spi_timeout_ms))
    {
        gpio_put(ST7789_DC_PIN, true);
        gpio_put(ST7789_CS_PIN, false);
        dma_channel_configure(
            DMA_CHANNEL_ST7789_SPI_TX,          // DMA channel
            &_spi_tx_dma_cfg,                   // Pointer to channel config
            &spi_get_hw(ST7789_SPI_INST)->dr,   // write address
            data,                               // read address
            length,                             // element count (each element is of size transfer_data_size (DMA_SIZE_8))
            false);                             // don't start yet
        dma_channel_configure(
            DMA_CHANNEL_ST7789_SPI_RX,          // DMA channel
            &_spi_rx_dma_cfg,                   // Pointer to channel config
            &_spi_dummy,                        // write address
            &spi_get_hw(ST7789_SPI_INST)->dr,   // read address
            length,                             // element count (each element is of size transfer_data_size (DMA_SIZE_8))
            false);                             // don't start yet
        // start them exactly simultaneously to avoid races (in extreme cases the FIFO could overflow)
        dma_start_channel_mask((1u << DMA_CHANNEL_ST7789_SPI_RX) | (1u << DMA_CHANNEL_ST7789_SPI_TX));
    }
}


static void _spi_dma_isr()
{
    if (dma_hw->ST7789_DMA_INTS & (1u << DMA_CHANNEL_ST7789_SPI_RX))    // If SPI DMA finish
    {
        // Clear interrupt flag
        dma_hw->ST7789_DMA_INTS |= (1u << DMA_CHANNEL_ST7789_SPI_RX);
        // CS was asserted when starting DMA transfer, dessert here
        gpio_put(ST7789_CS_PIN, true);
        lv_disp_flush_ready(&_disp_drv);
        sem_release(&_spi_sem); // acquired in _send_color
    }
}


static void _send_init_cmds(void)
{
    /*The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct. */
    static const struct 
    {
        uint8_t cmd;
        uint8_t data[16];
        uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
    } init_cmds[] = 
    {
        {0xCF, {0x00, 0x83, 0X30}, 3},
        {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
        {ST7789_PWCTRL2, {0x85, 0x01, 0x79}, 3},
        {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
        {0xF7, {0x20}, 1},
        {0xEA, {0x00, 0x00}, 2},
        {ST7789_LCMCTRL, {0x26}, 1},
        {ST7789_IDSET, {0x11}, 1},
        {ST7789_VCMOFSET, {0x35, 0x3E}, 2},
        {ST7789_CABCCTRL, {0xBE}, 1},
        {ST7789_MADCTL, {0x00}, 1}, // Set to 0x28 if your display is flipped
        {ST7789_COLMOD, {0x55}, 1},
        {ST7789_INVON, {0}, 0},
        {ST7789_RGBCTRL, {0x00, 0x1B}, 2},
        {0xF2, {0x08}, 1},
        {ST7789_GAMSET, {0x01}, 1},
        {ST7789_PVGAMCTRL, {0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17}, 14},
        {ST7789_NVGAMCTRL, {0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E}, 14},
        {ST7789_CASET, {0x00, 0x00, 0x00, 0xEF}, 4},
        {ST7789_RASET, {0x00, 0x00, 0x01, 0x3f}, 4},
        {ST7789_RAMWR, {0}, 0},
        {ST7789_GCTRL, {0x07}, 1},
        {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
        {ST7789_MADCTL, {0xA0}, 1}, // Portrait: C0, Portrait_Inverted: 00, Landscape: 60, Landscape_Inverted: A0
        {ST7789_SLPOUT, {0}, 0x80},
        {ST7789_DISPON, {0}, 0x80},
        {0, {0}, 0xff},
    };
    //Reset the display
    gpio_put(ST7789_RST_PIN, false);
    sleep_ms(100);
    gpio_put(ST7789_RST_PIN, true);
    sleep_ms(100);
    //Send all the commands
    uint16_t cmd = 0;
    while (init_cmds[cmd].databytes != 0xff) 
    {
        _send_cmd(init_cmds[cmd].cmd);
        _send_data(init_cmds[cmd].data, init_cmds[cmd].databytes & 0x1f);
        if (init_cmds[cmd].databytes & 0x80) 
        {
            sleep_ms(100);
        }
        cmd++;
    }
}


static void _st7789_init(void)
{
    //Initialize non-SPI GPIOs
    gpio_init(ST7789_CS_PIN);
    gpio_put(ST7789_CS_PIN, true);
    gpio_set_dir(ST7789_CS_PIN, GPIO_OUT);
    gpio_put(ST7789_CS_PIN, true);  // In case glitch
    gpio_init(ST7789_DC_PIN);
    gpio_set_dir(ST7789_DC_PIN, GPIO_OUT);
    gpio_init(ST7789_RST_PIN);
    gpio_set_dir(ST7789_RST_PIN, GPIO_OUT);
    // Initialize semaphore for SPI exclusive access
    sem_init(&_spi_sem, 1, 1);  // initial 1, max 1
    // Initial SPI bus
    spi_init(ST7789_SPI_INST, 62500000);   // 62.5MHz, maximum pico clock
    spi_set_format(ST7789_SPI_INST, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(ST7789_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ST7789_CLK_PIN, GPIO_FUNC_SPI);
    dma_channel_claim(DMA_CHANNEL_ST7789_SPI_TX);
    dma_channel_claim(DMA_CHANNEL_ST7789_SPI_RX);
    _spi_tx_dma_cfg = dma_channel_get_default_config(DMA_CHANNEL_ST7789_SPI_TX);
    _spi_rx_dma_cfg = dma_channel_get_default_config(DMA_CHANNEL_ST7789_SPI_RX);
    channel_config_set_transfer_data_size(&_spi_tx_dma_cfg, DMA_SIZE_8);
    channel_config_set_transfer_data_size(&_spi_rx_dma_cfg, DMA_SIZE_8);
    // TX DMA to transfer from a memory buffer to the SPI TX FIFO
    // paced by the SPI TX FIFO DREQ. Read address auto increase and
    // write address is fixed TX FIFO
    channel_config_set_dreq(&_spi_tx_dma_cfg, (spi_get_index(ST7789_SPI_INST) == 1) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
    channel_config_set_read_increment(&_spi_tx_dma_cfg, true);
    channel_config_set_write_increment(&_spi_tx_dma_cfg, false);
    // RX DMA to transfer from the SPI RX FIFO to a memory buffer
    // paced by SPI RX FIFO DREQ. Read address is fixed and write
    // address is dummy
    channel_config_set_dreq(&_spi_rx_dma_cfg, (spi_get_index(ST7789_SPI_INST) == 1) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
    channel_config_set_read_increment(&_spi_rx_dma_cfg, false);
    channel_config_set_write_increment(&_spi_rx_dma_cfg, false);
    /*
        * The LCD does not require RX. However DMA interrupt is fired when data transfer is finished at *register* level.
        * If we dessert CS on TX DMA interrupt, the last byte is lost because it has not been clocked out yet. The
        * walkaround is to set up a dummy RX DMA with TX. When RX DMA finishes, RX data has been "clocked in" and hence
        * TX data has already been clocked out.
        */
    // Setup DMA interrupt on DMA_IRQ_0
#if (ST7789_DMA_IRQ == DMA_IRQ_0)    
    dma_channel_set_irq0_enabled(DMA_CHANNEL_ST7789_SPI_RX, true);
#else
    dma_channel_set_irq1_enabled(DMA_CHANNEL_ST7789_SPI_RX, true);
#endif
    irq_add_shared_handler(ST7789_DMA_IRQ, _spi_dma_isr, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    irq_set_enabled(ST7789_DMA_IRQ, true);
    // Send ST7789 initialize sequence
    _send_init_cmds();
}


static void _lv_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map)
{
    //ST_LOGD("Flush (%d,%d)-(%d,%d)\n", area->x1, area->y1, area->x2, area->y2);
    uint8_t data[4] = {0};
    uint16_t offsetx1 = area->x1;
    uint16_t offsetx2 = area->x2;
    uint16_t offsety1 = area->y1;
    uint16_t offsety2 = area->y2;
    /* The ST7789 display controller can drive 320*240 displays, when using a 240*240
     * display there's a gap of 80px, we need to edit the coordinates to take into
     * account that gap, this is not necessary in all orientations. 
     * The following corresponding to "Landscape Inverted" orientation which is our case.
     */
    offsety1 += 80;
    offsety2 += 80;
    // Column addresses
    _send_cmd(ST7789_CASET);
    data[0] = (offsetx1 >> 8) & 0xFF;
    data[1] = offsetx1 & 0xFF;
    data[2] = (offsetx2 >> 8) & 0xFF;
    data[3] = offsetx2 & 0xFF;
    _send_data(data, 4);
    // Page addresses
    _send_cmd(ST7789_RASET);
    data[0] = (offsety1 >> 8) & 0xFF;
    data[1] = offsety1 & 0xFF;
    data[2] = (offsety2 >> 8) & 0xFF;
    data[3] = offsety2 & 0xFF;
    _send_data(data, 4);
    // Memory write
    _send_cmd(ST7789_RAMWR);
    uint32_t size = lv_area_get_width(area) * lv_area_get_height(area);
    _send_color((void*)color_map, size * 2);    // 2 bytes per pixel
}


void st7789_lvgl_init(void)
{
    ST_LOGD("Initialize ST7789\n");
    // ST7789 driver init
    _st7789_init();
    // LVGL init
	lv_disp_draw_buf_init(&_buf, _buf1, _buf2, LV_HOR_RES_MAX * 8);
	lv_disp_drv_init(&_disp_drv);
    _disp_drv.hor_res = LV_HOR_RES_MAX;
	_disp_drv.ver_res = LV_VER_RES_MAX;
    _disp_drv.draw_buf = &_buf;
	_disp_drv.flush_cb = _lv_flush;
	lv_disp_drv_register(&_disp_drv);
}


