#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <hardware/clocks.h>
#include <hardware/pio.h>
#include <hardware/dma.h>
#include <hardware/irq.h>
#include "my_debug.h"
#include "sw_conf.h"
#include "hw_conf.h"
#include "audio.h"
#include "i2s.pio.h"
#include "i2s.h"


// flag if buffer underrun happened
bool i2s_buffer_underrun = false;

// defined in audio.c
extern audio_cbuf_t *audio_buffer;

// i2s PIO program offset for cleanup
static uint pio_i2s_offset = 0;


static void i2s_dma_channel_irq_enable()
{
#if (I2S_DMA_IRQ == DMA_IRQ_0)
    dma_channel_set_irq0_enabled(DMA_CHANNEL_I2S_TX, true);
#else
    dma_channel_set_irq1_enabled(DMA_CHANNEL_I2S_TX, true);
#endif    
}


static void i2s_dma_channel_irq_disable()
{
#if (I2S_DMA_IRQ == DMA_IRQ_0)
    dma_channel_set_irq0_enabled(DMA_CHANNEL_I2S_TX, false);
#else
    dma_channel_set_irq1_enabled(DMA_CHANNEL_I2S_TX, false);
#endif    
}


static void i2s_dma_isr()
{
    if (dma_hw->I2S_DMA_INTS & (1u << DMA_CHANNEL_I2S_TX))  // interrupt because I2S TX finish
    {   
        // clear the interrupt request
        dma_hw->I2S_DMA_INTS |= 1u << DMA_CHANNEL_I2S_TX;
        // acknowledge buffer sent
        audio_cbuf_finish_read();
        // get new buffer to send
        audio_frame_t *frame = audio_cbuf_get_read_buffer();
        if (frame != NULL)
        {
            if (frame->length > 0)
            {
                dma_channel_transfer_from_buffer_now(DMA_CHANNEL_I2S_TX, frame->data, frame->length);
                i2s_buffer_underrun = false;
            }
            else
            {
                // No more data, no need DMA irq anymore
                AUD_LOGD("Audio/i2s: no more samples\n");
                i2s_dma_channel_irq_disable();
            }
        }
        else
        {
            AUD_LOGW("Audio/i2s: buffer underrun\n");
            i2s_buffer_underrun = true; // audio module will detect this flag and resume playback
        }
    }
}


static void pio_i2s_start()
{   
    // set init pin state, can only be done when state machine is diabled
    pio_sm_set_enabled(I2S_PIO, I2S_PIO_SM, false);
    uint pin_mask = (1u << I2S_DATAOUT_PIN) | (3u << I2S_BCLK_PIN);
    pio_sm_set_pins_with_mask(I2S_PIO, I2S_PIO_SM, pin_mask, pin_mask); // set all 3 pins to 1 intially
    // drain fifo and OSR
    pio_sm_drain_tx_fifo(I2S_PIO, I2S_PIO_SM);
    // set number of bits in y register
    pio_sm_exec(I2S_PIO, I2S_PIO_SM, pio_encode_set(pio_y, 14));
    // immediate jump to entry point, statemachine not enabled yet
    pio_sm_exec(I2S_PIO, I2S_PIO_SM, pio_encode_jmp(pio_i2s_offset + pio_i2s_offset_entry_point));
    // start statem achine
    pio_sm_set_enabled(I2S_PIO, I2S_PIO_SM, true);
}


static void pio_i2s_flush()
{
    static const uint32_t SM_STALL_MASK = 1u << (PIO_FDEBUG_TXSTALL_LSB + I2S_PIO_SM);
    // Disable IRQ
    i2s_dma_channel_irq_disable();
    // Let PIO execute until stalled
    pio_sm_set_enabled(I2S_PIO, I2S_PIO_SM, true); 
    // Clear the sticky stall status
    I2S_PIO->fdebug = SM_STALL_MASK;
    // Wait until the stall flag is up again.
    while (!(I2S_PIO->fdebug & SM_STALL_MASK))
    {
        tight_loop_contents();
    }
}


void i2s_init()
{
    AUD_LOGD("Audio/i2s: init\n");
    // Set up I2S PIO program
    pio_sm_claim(I2S_PIO, I2S_PIO_SM);
    pio_i2s_offset = pio_add_program(I2S_PIO, &pio_i2s_program);
    pio_sm_config conf = pio_i2s_program_get_default_config(pio_i2s_offset);
    // DATAOUT pin
    pio_gpio_init(I2S_PIO, I2S_DATAOUT_PIN);
    pio_sm_set_consecutive_pindirs(I2S_PIO, I2S_PIO_SM, I2S_DATAOUT_PIN, 1, true);
    sm_config_set_out_pins(&conf, I2S_DATAOUT_PIN, 1);
    sm_config_set_out_shift(&conf, /* shift_right */ false, /* autopull */ true, 32);
    // BCLK and LRC are sideset pins
    pio_gpio_init(I2S_PIO, I2S_BCLK_PIN);
    pio_gpio_init(I2S_PIO, I2S_BCLK_PIN + 1);
    pio_sm_set_consecutive_pindirs(I2S_PIO, I2S_PIO_SM, I2S_BCLK_PIN, 2, true);
    sm_config_set_sideset_pins(&conf, I2S_BCLK_PIN);
    // Statemachine init
    pio_sm_init(I2S_PIO, I2S_PIO_SM, pio_i2s_offset, &conf);
    // Setup I2S PIO clock
    uint32_t system_clock_frequency = clock_get_hz(clk_sys);
    // Each BCLK cycles contains 2 PIO instruction (see i2s.pio)
    // PIO_CLK = BCLK * 2
    //  -> BCLK = AUDIO_SAMPLE_RATE * 32
    //  -> PIO_CLK = AUDIO_SAMPLE_RATE * 64
    // divider is fraction number in 16:8 fixed point format,
    // divider = 256 * SYS_CLK / PIO_CLK * 256 
    //         = 256 * SYS_CLK / (AUDIO_SAMPLE_RATE * 64)
    //         = 4 * SYS_CLK / AUDIO_SAMPLE_RATE
    uint32_t divider = system_clock_frequency * 4 / AUDIO_SAMPLE_RATE;
    assert(divider < 0x1000000);
    pio_sm_set_clkdiv_int_frac(I2S_PIO, I2S_PIO_SM, divider >> 8u, divider & 0xffu);
    // Configure DMA channel to send audio samples to I2S
    dma_channel_claim(DMA_CHANNEL_I2S_TX);
    dma_channel_config i2s_tx_dma_cfg = dma_channel_get_default_config(DMA_CHANNEL_I2S_TX);
    channel_config_set_transfer_data_size(&i2s_tx_dma_cfg, DMA_SIZE_32); // 32bit transfer
    channel_config_set_read_increment(&i2s_tx_dma_cfg, true);            // auto increase pointer
    channel_config_set_dreq(&i2s_tx_dma_cfg, pio_get_dreq(I2S_PIO, I2S_PIO_SM, true));  // I2S TX transfer request signal
    dma_channel_configure(
        DMA_CHANNEL_I2S_TX,         // DMA channel
        &i2s_tx_dma_cfg,
        &I2S_PIO->txf[I2S_PIO_SM],  // Write address (only need to set this once)
        NULL,                       // Don't provide a read address yet
        0,                          // Don't give transfer count yet
        false                       // Don't start yet
    );
    // Tell the DMA to raise IRQ when the channel finishes a block
    irq_set_exclusive_handler(I2S_DMA_IRQ, i2s_dma_isr);
    irq_set_priority(I2S_DMA_IRQ, 0x00);
    irq_set_enabled(I2S_DMA_IRQ, true);
}


void i2s_deinit()
{
    AUD_LOGD("Audio/i2s: deinit\n");
    // Disable DMA irq
    i2s_dma_channel_irq_disable();
    // remove IRQ handler (internally will disable IRQ before removing then restore)
    irq_remove_handler(I2S_DMA_IRQ, i2s_dma_isr);
    // let DMA finish
    dma_channel_wait_for_finish_blocking(DMA_CHANNEL_I2S_TX);
    dma_channel_unclaim(DMA_CHANNEL_I2S_TX);
    // cleanup PIO    
    pio_sm_set_enabled(I2S_PIO, I2S_PIO_SM, false);
    pio_sm_drain_tx_fifo(I2S_PIO, I2S_PIO_SM);
    pio_remove_program(I2S_PIO, &pio_i2s_program, pio_i2s_offset);
    pio_sm_unclaim(I2S_PIO, I2S_PIO_SM);
}


void i2s_start_playback()
{
    i2s_dma_channel_irq_enable();
    pio_i2s_start();
    i2s_resume_playback();
}


void i2s_resume_playback()
{
    audio_frame_t *frame = audio_cbuf_get_read_buffer();
    MY_ASSERT((frame != NULL) && (frame->length != 0));
    dma_channel_transfer_from_buffer_now(DMA_CHANNEL_I2S_TX, frame->data, frame->length);
    i2s_buffer_underrun = false;
}


void i2s_stop_playback()
{
    dma_channel_abort(DMA_CHANNEL_I2S_TX);
#if (I2S_DMA_IRQ == DMA_IRQ_0)
    dma_channel_acknowledge_irq0(DMA_CHANNEL_I2S_TX);
#else
    dma_channel_acknowledge_irq1(DMA_CHANNEL_I2S_TX);
#endif
    pio_i2s_flush();
    pio_sm_set_enabled(I2S_PIO, I2S_PIO_SM, false);
}


void i2s_send_buffer_blocking(uint32_t *buf, uint32_t len)
{
    i2s_dma_channel_irq_disable();
    pio_i2s_start();
    for (uint32_t i = 0; i < len; ++i)
    {
         pio_sm_put_blocking(I2S_PIO, I2S_PIO_SM, buf[i]);
    }
    pio_i2s_flush();
    pio_sm_set_enabled(I2S_PIO, I2S_PIO_SM, false);
}