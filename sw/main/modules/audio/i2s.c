#include <string.h>
#include <pico/stdlib.h>
#include <hardware/clocks.h>
#include <hardware/pio.h>
#include <hardware/dma.h>
#include <hardware/irq.h>
#include "sw_conf.h"
#include "hw_conf.h"
#include "audio_buffer.h"
#include "i2s.pio.h"
#include "i2s.h"



// Notififcation callbacks

static i2s_notification_cb_t sample_requested_cb = 0;
static void * sample_requested_cb_param = 0;
static i2s_notification_cb_t xmit_finished_cb = 0;
static void* xmit_finished_cb_param = 0;


static void _i2s_dma_isr()
{
    if (dma_hw->I2S_DMA_INTS & (1u << DMA_CHANNEL_I2S_TX))  // If I2S TX finish
    {   
        // Clear the interrupt request.
        dma_hw->I2S_DMA_INTS |= 1u << DMA_CHANNEL_I2S_TX;
        if (cur_tx_buf)
        {
            // was transmitting buffer 1, switch to buffer 0
            if (tx_buf0_len > 0)
            {
                dma_channel_transfer_from_buffer_now(DMA_CHANNEL_I2S_TX, tx_buf0, tx_buf0_len);
                cur_tx_buf = false;
                // tx will stop if no further samples
                tx_buf1_len = 0;
                // Notify samples requested
                if (sample_requested_cb)
                    sample_requested_cb(sample_requested_cb_param);
            }
            else
            {
                // play finished
                if (xmit_finished_cb)
                    xmit_finished_cb(xmit_finished_cb_param);
            }
        }
        else
        {
            // was transmitting buffer 0, switch to buffer 1
            if (tx_buf1_len > 0)
            {
                dma_channel_transfer_from_buffer_now(DMA_CHANNEL_I2S_TX, tx_buf1, tx_buf1_len);
                cur_tx_buf = true;
                // tx will stop if no further samples
                tx_buf0_len = 0;
                // Notify samples requested
                if (sample_requested_cb)
                    sample_requested_cb(sample_requested_cb_param);
            }
            else
            {
                // play finished
                if (xmit_finished_cb)
                    xmit_finished_cb(xmit_finished_cb_param);
            }
        }
    }
}


void i2s_init()
{
    // Set up I2S PIO program
    pio_sm_claim(I2S_PIO, 0);
    uint offset = pio_add_program(I2S_PIO, &pio_i2s_program);
    pio_i2s_program_init(I2S_PIO, 0, offset, I2S_DATAOUT_PIN, I2S_BCLK_PIN);

    // Setup I2S PIO clock
    uint32_t system_clock_frequency = clock_get_hz(clk_sys);
    // 2 PIO cycles = 1 BCLK
    // PIO_CLK = BCLK * 2
    // BCLK = AUDIO_SAMPLE_RATE * 32
    // PIO_CLK = AUDIO_SAMPLE_RATE * 64
    // divider is fraction number in 16:8 fixed point format,
    // divider = 256 * SYS_CLK / PIO_CLK * 256 
    //         = 256 * SYS_CLK / (AUDIO_SAMPLE_RATE * 64)
    //         = 4 * SYS_CLK / AUDIO_SAMPLE_RATE
    uint32_t divider = system_clock_frequency * 4 / AUDIO_SAMPLE_RATE;
    assert(divider < 0x1000000);
    pio_sm_set_clkdiv_int_frac(I2S_PIO, 0, divider >> 8u, divider & 0xffu);

    // Configure DMA channel to send audio samples to I2S
    dma_channel_claim(DMA_CHANNEL_I2S_TX);
    dma_channel_config i2s_tx_dma_cfg = dma_channel_get_default_config(DMA_CHANNEL_I2S_TX);
    channel_config_set_transfer_data_size(&i2s_tx_dma_cfg, DMA_SIZE_32); // 32bit transfer
    channel_config_set_read_increment(&i2s_tx_dma_cfg, true);            // auto increase pointer
    channel_config_set_dreq(&i2s_tx_dma_cfg, I2S_PIO_TX_DREQ);           // I2S TX transfer request signal
    dma_channel_configure(
        DMA_CHANNEL_I2S_TX, // DMA channel
        &i2s_tx_dma_cfg,
        &I2S_PIO->txf[0],   // Write address (only need to set this once)
        NULL,               // Don't provide a read address yet
        0,                  // Don't give transfer count yet
        false               // Don't start yet
    );
    // Tell the DMA to raise IRQ when the channel finishes a block
#if (I2S_DMA_IRQ == DMA_IRQ_0)
    dma_channel_set_irq0_enabled(DMA_CHANNEL_I2S_TX, true);
#else
    dma_channel_set_irq1_enabled(DMA_CHANNEL_I2S_TX, true);
#endif    
    irq_add_shared_handler(I2S_DMA_IRQ, _i2s_dma_isr, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    irq_set_enabled(I2S_DMA_IRQ, true);
}


void i2s_kickstart()
{
    // prepare an silent buffer 0 and let i2c start sending * with interrupt disabled * (for timing purpose)
    cur_tx_buf = false;
    memset(tx_buf0, 0, AUDIO_MAX_BUFFER_LENGTH * sizeof(uint32_t));
    tx_buf0_len = AUDIO_MAX_BUFFER_LENGTH;
    irq_set_enabled(I2S_DMA_IRQ, false);
    dma_channel_transfer_from_buffer_now(DMA_CHANNEL_I2S_TX, tx_buf0, tx_buf0_len);
    pio_sm_set_enabled(pio0, 0, true);
    // prepare first lot of samples in buffer 1
    sample_requested_cb(sample_requested_cb_param);
    dma_channel_wait_for_finish_blocking(DMA_CHANNEL_I2S_TX);
    // When buffer 0 finishes, we should have buffer 1 with actual sample.
    // Send a "gradient" buffer of {samples} length
    int16_t l = (int16_t)(tx_buf1[0] >> 16);
    int16_t r = (int16_t)(tx_buf1[0] & 0x0000ffff);
    const int samples = 480;
    for (int i = 0; i < samples; ++i)
    {
        uint32_t data = ( (uint32_t)(l * i / samples) << 16) + (uint32_t)((r * i) / samples);
        pio_sm_put_blocking(I2S_PIO, 0, data);
    }
    // follow by buffer 1 the let isr handle the remaining
    irq_set_enabled(I2S_DMA_IRQ, true);
    cur_tx_buf = true;
    dma_channel_transfer_from_buffer_now(DMA_CHANNEL_I2S_TX, tx_buf1, tx_buf1_len);
}


void i2s_setup_notifications(i2s_notification_t* n)
{
    sample_requested_cb = n->sample_requested_cb;
    sample_requested_cb_param = n->sample_requested_cb_param;
    xmit_finished_cb = n->xmit_finished_cb;
    xmit_finished_cb_param = n->xmit_finished_cb_param;
}