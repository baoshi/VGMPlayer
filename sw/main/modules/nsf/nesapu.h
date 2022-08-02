#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "nesbus.h"
#include "nesfloat.h"


#ifdef __cplusplus
extern "C" {
#endif

//
// Envelope (Pulse1, Pulse2, Noise)
// http://wiki.nesdev.org/w/index.php/APU_Envelope
//
typedef struct envelope_s
{
    bool enabled;
    bool loop;
    bool start;
    uint8_t period;
    uint8_t divider;
    uint8_t decay;
} envelope_t;


//
// Length Counter (Pulse1, Pulse2, Triangle, Noise)
// http://wiki.nesdev.org/w/index.php/APU_Length_Counter
//
typedef struct length_counter_s
{
    bool halt;
    uint8_t value;
} length_counter_t;


//
// Pulse channel
// http://wiki.nesdev.org/w/index.php/APU_Pulse
//
typedef struct pulse_s
{
    bool enabled;
    envelope_t envelope;
    length_counter_t length_counter;
    uint8_t duty;
    uint8_t duty_index;
    uint16_t timer_period;
    uint16_t timer_value;
    struct sweep_s
    {
        bool enabled;
        uint8_t period;
        bool negate;
        uint8_t shift;
        bool reload;
        uint8_t value;
    } sweep;
    
} pulse_t;


//
// Triangle channel
// https://wiki.nesdev.org/w/index.php/APU_Triangle
//
typedef struct triangle_s
{
    bool enabled;
    length_counter_t length_counter;
    bool linear_counter_on;
    uint8_t linear_counter_period;
    bool linear_counter_reset;
    uint8_t linear_counter_value;
    uint8_t waveform_index;
    uint16_t timer_period;
    uint16_t timer_value;
    bool timer_period_bad;
} triangle_t;


//
// Noise channel
// https://wiki.nesdev.org/w/index.php/APU_Noise
//
typedef struct noise_s
{
    bool enabled;
    envelope_t envelope;
    length_counter_t length_counter;
    bool mode;
    uint16_t timer_period;
    uint16_t timer_value;
    uint16_t shift_reg;
} noise_t;

//
// DMC channel 
// https://wiki.nesdev.org/w/index.php/APU_DMC
//
typedef struct dmc_s
{
    bool enabled;
    nesbus_ctx_t* bus;  // DMC need bus access
    bool irq_enabled;
    bool loop;
    uint8_t output_value;
    uint16_t sample_addr;
    uint16_t sample_len;
    uint16_t timer_period;
    uint16_t timer_value;
    uint8_t read_buffer;
    bool read_buffer_empty;
    uint16_t read_addr;
    unsigned int read_remaining;
    uint8_t output_shift_reg;
    unsigned int output_bits_remaining;
    bool output_silence;
    bool stall_cpu;
    bool irq_requested;
} dmc_t;

// Frame Counter
// http://wiki.nesdev.org/w/index.php/APU_Frame_Counter
//
typedef struct frame_counter_s
{
    bool mode;  // false: 4 step sequence; true: 5 step sequence
    bool inhibit_irq;
    bool irq_requested;
} frame_counter_t;


typedef struct nesapu_ctx_s
{
    bool format;        // true: PAL, false: NTSC
    // APU configs
    uint32_t cycles;
    uint32_t clock_rate;
    uint32_t sample_rate;
    // sequencer step
    uint8_t seq_step;               
    // frame counter timing
    uint32_t cycles_per_frame;
    uint32_t next_frame_cycle;
    int32_t frame_cycle_error;
    int32_t accumulated_frame_cycle_error;
    // registers
    uint8_t reg4000, reg4001, reg4002, reg4003; // Pulse1
    uint8_t reg4004, reg4005, reg4006, reg4007; // Pulse 2
    uint8_t reg4008, reg4009, reg400a, reg400b; // Triangle
    uint8_t reg400c, reg400d, reg400e, reg400f; // Noise
    uint8_t reg4010, reg4011, reg4012, reg4013; // DMC
    uint8_t reg4017;    // All 
    // Pulse channel
    pulse_t pulse1, pulse2;
    // Triangle channel
    triangle_t triangle;
    // Noise channel
    noise_t noise;
    // DMC channel
    dmc_t dmc;
    // Frame counter
    frame_counter_t frame_counter;
} nesapu_ctx_t;


nesapu_ctx_t * nesapu_create(bool format, uint32_t clock, uint32_t srate);
void nesapu_destroy(nesapu_ctx_t* ctx);
bool nesapu_attach_bus(nesapu_ctx_t* apu, nesbus_ctx_t* bus);
void nesapu_reset(nesapu_ctx_t* ctx);
void nesapu_clock(nesapu_ctx_t* ctx);
nesfloat_t nesapu_sample(nesapu_ctx_t* ctx);
bool nesapu_irq_requested(nesapu_ctx_t* ctx);
bool nesapu_dmc_stall_cpu(nesapu_ctx_t* ctx);


#ifdef __cplusplus
}
#endif
