#include <stdlib.h>
#include <memory.h>
#include "nesbus.h"
#include "nesapu.h"
#include "nesfloat.h"

//
// Envelope (Pulse1, Pulse2, Noise)
// http://wiki.nesdev.org/w/index.php/APU_Envelope
//

static void envelope_reset(envelope_t* e)
{
    e->enabled = false;
    e->loop = false;
    e->period = 0;
    e->start = false;
    e->divider = 0;
    e->decay = 0;
}


static void envelope_clock(envelope_t* e)
{
    if (e->start)
    {
        e->decay = 15;
        e->divider = e->period;
        e->start = false;
        return;
    }
    if (e->divider != 0) 
    {
        --e->divider;
    }
    else
    {
        e->divider = e->period;
        if (e->decay)
            --(e->decay);
        else if (e->loop)
            e->decay = 15;
    }
}


//
// Length Counter (Pulse1, Pulse2, Triangle, Noise)
// http://wiki.nesdev.org/w/index.php/APU_Length_Counter
//
static const uint8_t length_counter_table[32] = 
{
    10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
    12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};


static void length_counter_reset(length_counter_t* l)
{
    l->halt = true;
    l->value = 0;
}


static void length_counter_clock(length_counter_t* l)
{
    if ((!l->halt) && (l->value != 0))
        --(l->value);
}


//
// Pulse channel
// http://wiki.nesdev.org/w/index.php/APU_Pulse
//

// Duty lookup table, see wiki "Implementation details"
static const bool pulse_waveform_table[][8] = 
{
    { 0, 0, 0, 0, 0, 0, 0, 1 }, // 12.5%
    { 0, 0, 0, 0, 0, 0, 1, 1 }, // 25%
    { 0, 0, 0, 0, 1, 1, 1, 1 }, // 50%
    { 1, 1, 1, 1, 1, 1, 0, 0 }  // 25% negated
};


static void pulse_reset(pulse_t* p)
{
    p->enabled = false;
    envelope_reset(&(p->envelope));
    length_counter_reset(&(p->length_counter));
    p->duty = 0;
    p->duty_index = 0;
    p->timer_period = 0;
    p->timer_value = 0;
    p->sweep.enabled = false;
    p->sweep.period = 0;
    p->sweep.negate = false;
    p->sweep.shift = 0;
    p->sweep.reload = false;
    p->sweep.value = 0;
}


static void pulse_timer_clock(pulse_t* p)
{
    if (p->timer_value)
    {
        --(p->timer_value);
    }
    else
    {
        p->timer_value = p->timer_period;
        if (0 == p->duty_index)     // duty_index counting 0,7,6,5,4,3,2,1,0,7,6,5,4...
            p->duty_index = 7;
        else
            --(p->duty_index);
    }
}


// http://wiki.nesdev.org/w/index.php/APU_Sweep
static void pulse_sweep_clock(pulse_t* p, bool p1)
{
    if (p->sweep.reload)
    {
        p->sweep.value = p->sweep.period + 1;
        p->sweep.reload = false;
        return;
    }
    if (p->sweep.value)
    {
        --(p->sweep.value);
    }
    else
    {
        p->sweep.value = p->sweep.period + 1;
        if ((p->sweep.enabled) && (p->sweep.shift != 0))
        {
            // Shift channel's 11-bit timer period right by shift count, producing the change amount
            uint16_t c = p->timer_period >> p->sweep.shift;
            // If the negate flag is true, the change amount is made negative.
            // The target period is the sum of the current period and the change amount.
            if (p->sweep.negate)
            {
                if (p1)
                {
                    // Pulse channel 1,  (−c-1)
                    p->timer_period -= c + 1;
                }
                else
                {
                    // Pulse channel 2, (-c)
                    p->timer_period -= c;
                }
            }
            else
            {
                p->timer_period += c;
            }
        }
    }
}


static uint8_t pulse_output(pulse_t* p)
{
    if (!p->enabled) return 0;
    if (p->length_counter.value == 0) return 0;
    if (p->timer_period < 8) return 0;        // If the current period is less than 8, the sweep unit mutes the channel
    if (p->timer_period > 0x7ff) return 0;    // If at any time the target period is greater than $7FF, the sweep unit mutes the channel
    if (!pulse_waveform_table[p->duty][p->duty_index]) return 0;
    return (p->envelope.enabled) ? (p->envelope.decay) : (p->envelope.period);
}


//
// Triangle channel
// https://wiki.nesdev.org/w/index.php/APU_Triangle
//

static const uint8_t triangle_waveform_table[32] = 
{
    15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
};


static void triangle_reset(triangle_t* t)
{
    t->enabled = false;
    length_counter_reset(&(t->length_counter));
    t->linear_counter_on = false;
    t->linear_counter_period = 0;
    t->linear_counter_reset = false;
    t->linear_counter_value = 0;
    t->waveform_index = 0;
    t->timer_period = 0;
    t->timer_value = 0;
    t->timer_period_bad = true;
}


static void triangle_timer_clock(triangle_t* t)
{
    // Do not clock if the channel should be silenced
    if (!t->enabled) return;
    if (t->timer_period_bad) return;
    if (t->length_counter.value == 0) return;
    if (t->linear_counter_value == 0) return;
    // Clock triangle channel, produce next sample if timer counted down to zero, reload timer
    if (t->timer_value > 0)
    {
        --t->timer_value;
    }
    else
    {
        t->timer_value = t->timer_period;
        t->waveform_index = (t->waveform_index + 1) % 32;
    }
}


static void triangle_linear_counter_clock(triangle_t* t)
{
    if (t->linear_counter_reset)
    {
        t->linear_counter_value = t->linear_counter_period;
    }
    else if (t->linear_counter_value > 0)
    {
        --t->linear_counter_value;
    }
    if (t->linear_counter_on)
        t->linear_counter_reset = false;
}


static uint8_t triangle_output(triangle_t* t)
{
    // Retain last sample even if not clocked
    return triangle_waveform_table[t->waveform_index];
}


//
// Noise channel
// https://wiki.nesdev.org/w/index.php/APU_Noise
//
static const uint16_t noise_timer_period_ntsc[16] = 
{
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

static const uint16_t noise_timer_period_pal[16] =
{
    4, 8, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708,  944, 1890, 3778
};


static void noise_reset(noise_t* n)
{
    n->enabled = false;
    envelope_reset(&(n->envelope));
    length_counter_reset(&(n->length_counter));
    n->mode = false;
    n->timer_period = 0;
    n->timer_value = 0;
    n->shift_reg = 1;
}


static void noise_timer_clock(noise_t* n)
{
    if (n->timer_value > 0)
    {
        --n->timer_value;
    }
    else 
    {
        n->timer_value = n->timer_period;
        // When the timer clocks the shift register, the following occur in order:
        // 1) Feedback is calculated as the exclusive-OR of bit 0 and one other bit: bit 6 if Mode flag is set, otherwise bit 1.
        uint16_t feedback = (n->shift_reg & 0x0001) ^ (n->mode ? ((n->shift_reg >> 6) & 0x0001) : ((n->shift_reg >> 1) & 0x0001));
        // 2) The shift register is shifted right by one bit.
        n->shift_reg = n->shift_reg >> 1;
        // 3) Bit 14, the leftmost bit, is set to the feedback calculated earlier.
        n->shift_reg |= (feedback << 14);
    }
}


static uint8_t noise_output(noise_t* n)
{
    if (!n->enabled) return 0;
    // The mixer receives the current envelope volume except when
    // Bit 0 of the shift register is set, or
    // The length counter is zero
    if (n->length_counter.value == 0) return 0;
    if (n->shift_reg & 0x01) return 0;
    // Return current envelopment value or const volume (in envelope.period)
    return (n->envelope.enabled ? n->envelope.decay : n->envelope.period);
}


//
// DMC channel 
// https://wiki.nesdev.org/w/index.php/APU_DMC
//

// Note: The table on WiKi is refering to CPU cycles. DMC APU is clocking at half speed
static const uint16_t dmc_timer_period_ntsc[16] =
{
    //428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84, 72, 54
    214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27
};

static const uint16_t dmc_timer_period_pal[16] =
{
    //398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118,  98,  78,  66,  50
    199, 177, 158, 149, 138, 118, 105, 149, 138, 74, 66, 59, 49, 39, 33, 25
};


static void dmc_transfer(dmc_t* d)
{
    if (d->read_buffer_empty && d->read_remaining > 0)
    {
        // 1. The CPU is stalled for up to 4 CPU cycles to allow the longest
        //    possible write (the return address and write after an IRQ) to
        //    finish.
        d->stall_cpu = true;
        // 2. The sample buffer is filled with the next sample byte read from the current address
        d->read_buffer = nesbus_read(d->bus, d->read_addr);
        d->read_buffer_empty = false;
        // 3. The address is incremented. If it exceeds $FFFF, it wraps to $8000
        ++d->read_addr;
        if (d->read_addr == 0x0000)
            d->read_addr = 0x8000;
        // 4. The bytes remaining counter is decremented. If it becomes zero and the loop flag is set, the sample is
        //    restarted; otherwise, if the bytes remaining counter becomes zero and the IRQ enabled flag is set,
        //    the interrupt flag is set.
        --d->read_remaining;
        if (d->read_remaining == 0)
        {
            if (d->loop)
            {
                d->read_addr = d->sample_addr;
                d->read_remaining = d->sample_len;
            }
            else if (d->irq_enabled)
            {
                d->irq_requested = true;
            }
        }
    }
}


static void dmc_reset(dmc_t* d)
{
    d->enabled = false;
    d->irq_enabled = true;
    d->loop = false;
    d->output_value = 0;
    d->sample_addr = 0;
    d->sample_len = 0;
    d->timer_period = 0;
    d->timer_value = 0;
    d->read_buffer = 0;
    d->read_buffer_empty = true;
    d->read_addr = 0;
    d->read_remaining = 0;
    d->output_shift_reg = 0;
    d->output_bits_remaining = 0;
    d->output_silence = false;
    d->stall_cpu = false;
    d->irq_requested = false;
}


static void dmc_timer_clock(dmc_t* d)
{
    if (d->timer_value > 0)
    {
        --d->timer_value;
        return;
    }
    d->timer_value = d->timer_period;
    // When the timer outputs a clock, the following actions occur in order: 
    if (!d->output_silence)
    {
        // 1. If the silence flag is clear, the output level changes based on bit 0 of the shift register.
        //    If the bit is 1, add 2; otherwise, subtract 2.
        //    But if adding or subtracting 2 would cause the output level to leave the 0-127 range,
        //    leave the output level unchanged. This means subtract 2 only if the current level is at least 2,
        //    or add 2 only if the current level is at most 125.
        if (d->output_shift_reg & 0x01)
            d->output_value += (d->output_value <= 125) ? 2 : 0;
        else
            d->output_value -= (d->output_value >= 2) ? 2 : 0;
        // 2. The right shift register is clocked
        d->output_shift_reg >>= 1;
    }
    // 3. As stated above, the bits-remaining counter is decremented. If it becomes zero, a new output cycle is started.
    if (d->output_bits_remaining > 0)
    {
        --d->output_bits_remaining;
    }
    if (d->output_bits_remaining == 0)
    {
        // A new cycle is started as follows :
        // 1. The bits-remaining counter is loaded with 8.
        d->output_bits_remaining = 8;
        // 2. If the sample buffer is empty, then the silence flag is set; 
        //    otherwise, the silence flag is cleared and the sample buffer is emptied into the shift register.
        if (d->read_buffer_empty) 
        {
            d->output_silence = true;
        }
        else 
        {
            d->output_silence = false;
            d->output_shift_reg = d->read_buffer;
            d->read_buffer_empty = true;
            // Perform memory read
            dmc_transfer(d);
        }
    }
}


static uint8_t dmc_output(dmc_t* d)
{
    return d->output_value;
}


//
// Frame Counter
// http://wiki.nesdev.org/w/index.php/APU_Frame_Counter
//
static void frame_counter_reset(frame_counter_t* f)
{
    f->mode = false;
    f->inhibit_irq = true;
    f->irq_requested = false;
}


//
// Mixer
// https://wiki.nesdev.org/w/index.php/APU_Mixer#Emulation
//
// Generate table using make_const_tables.c


static const nesfloat_t mixer_pulse_table[31] =
{
             0,    6232609,   12315540,   18254120,   24053428,   29718306,   35253376,   40663044,
      45951532,   51122860,   56180880,   61129276,   65971580,   70711160,   75351248,   79894960,
      84345240,   88704968,   92976872,   97163576,  101267592,  105291368,  109237216,  113107392,
     116904048,  120629256,  124285008,  127873240,  131395816,  134854496,  138251008
};


static const nesfloat_t mixer_tnd_table[203] =
{
             0,    3596940,    7164553,   10703196,   14213217,   17694966,   21148782,   24574998,
      27973946,   31345950,   34691328,   38010392,   41303460,   44570820,   47812788,   51029652,
      54221704,   57389228,   60532508,   63651820,   66747436,   69819624,   72868656,   75894784,
      78898280,   81879376,   84838328,   87775384,   90690792,   93584784,   96457600,   99309472,
     102140624,  104951272,  107741656,  110511992,  113262480,  115993344,  118704800,  121397032,
     124070264,  126724680,  129360504,  131977904,  134577088,  137158224,  139721536,  142267184,
     144795360,  147306224,  149799968,  152276768,  154736784,  157180208,  159607168,  162017872,
     164412480,  166791120,  169153984,  171501200,  173832960,  176149376,  178450624,  180736848,
     183008176,  185264784,  187506800,  189734352,  191947600,  194146672,  196331728,  198502848,
     200660208,  202803920,  204934128,  207050944,  209154512,  211244944,  213322352,  215386864,
     217438624,  219477696,  221504256,  223518400,  225520224,  227509856,  229487408,  231452976,
     233406688,  235348624,  237278928,  239197680,  241104976,  243000944,  244885648,  246759232,
     248621760,  250473328,  252314064,  254144032,  255963376,  257772096,  259570384,  261358256,
     263135856,  264903216,  266660496,  268407712,  270144992,  271872416,  273590048,  275297984,
     276996320,  278685088,  280364448,  282034432,  283695072,  285346528,  286988832,  288622080,
     290246336,  291861696,  293468160,  295065888,  296654912,  298235264,  299807136,  301370464,
     302925376,  304471968,  306010240,  307540288,  309062208,  310576032,  312081824,  313579648,
     315069568,  316551680,  318025984,  319492608,  320951584,  322402944,  323846752,  325283104,
     326712064,  328133664,  329547904,  330954912,  332354784,  333747488,  335133088,  336511680,
     337883296,  339248000,  340605792,  341956800,  343301056,  344638560,  345969408,  347293664,
     348611328,  349922464,  351227136,  352525408,  353817280,  355102848,  356382112,  357655168,
     358922016,  360182720,  361437312,  362685856,  363928384,  365164960,  366395584,  367620320,
     368839232,  370052352,  371259680,  372461344,  373657280,  374847584,  376032320,  377211456,
     378385120,  379553280,  380716000,  381873312,  383025248,  384171872,  385313216,  386449280,
     387580128,  388705792,  389826272,  390941696,  392052032,  393157312,  394257568,  395352864,
     396443264,  397528704,  398609248
};


nesfloat_t mixer_sample(uint8_t p1, uint8_t p2, uint8_t tr, uint8_t ns, uint8_t dm)
{
    return mixer_pulse_table[p1 + p2] + mixer_tnd_table[3 * tr + 2 * ns + dm];
}


static void apu_clock_timers(nesapu_ctx_t* a)
{
    // Triangle channel clocks every CPU cycle
     triangle_timer_clock(&(a->triangle));
    // Pulse/Noise/DMC channels clock every second CPU cycles
    if (a->cycles % 2)
    {
        pulse_timer_clock(&(a->pulse1));
        pulse_timer_clock(&(a->pulse2));
        noise_timer_clock(&(a->noise));
        dmc_timer_clock(&(a->dmc));
    }
}


static void frame_counter_clock_length_counters(nesapu_ctx_t* a)
{
    length_counter_clock(&(a->pulse1.length_counter));
    length_counter_clock(&(a->pulse2.length_counter));
    length_counter_clock(&(a->triangle.length_counter));
    length_counter_clock(&(a->noise.length_counter));
}


static void frame_counter_clock_sweeps(nesapu_ctx_t* a)
{
    pulse_sweep_clock(&(a->pulse1), true);
    pulse_sweep_clock(&(a->pulse2), false);
}


static void frame_counter_clock_envelopes(nesapu_ctx_t* a)
{
    envelope_clock(&(a->pulse1.envelope));
    envelope_clock(&(a->pulse2.envelope));
    envelope_clock(&(a->noise.envelope));
}


static void frame_counter_clock_linear_counter(nesapu_ctx_t* a)
{
    // Linear counter only in triangle channel
    triangle_linear_counter_clock(&(a->triangle));
}

//
// APU
// https://wiki.nesdev.org/w/index.php/APU
//
static bool apu_write_reg(uint16_t addr, uint8_t val, void* cookie)
{
    nesapu_ctx_t* a = (nesapu_ctx_t*) cookie;
    if (0 == a)
        return false;
    switch (addr)
    {
    // Pulse1 regs
    case 0x4000: // DDLC VVVV
        a->reg4000 = val;
        a->pulse1.duty = (val & 0xc0) >> 6;         // Duty (DD), index into tbl_pulse_waveform
        a->pulse1.length_counter.halt = (val & 0x20) ? true : false;    // Length counter halt (L)
        a->pulse1.envelope.loop = (val & 0x20) ? true : false;          // or Envelope loop (L)
        a->pulse1.envelope.enabled = (val & 0x10) ? false : true;       // Constant volume or Envelope enabled (C)
        a->pulse1.envelope.period = val & 0x0f;     // Volume or Period of envelope (VVVV)
        a->pulse1.envelope.start = true;
        break;
    case 0x4001: // EPPP NSSS
        a->reg4001 = val;
        a->pulse1.sweep.enabled = (val & 0x80) ? true : false; // Sweep enabled (E)
        a->pulse1.sweep.period = (val & 0x70) >> 4;            // Sweep period (PPP)
        a->pulse1.sweep.negate = (val & 0x08) ? true : false;  // Negate (N)
        a->pulse1.sweep.shift = val & 0x07;                    // Shift (SSS)
        a->pulse1.sweep.reload = true;
        break;
    case 0x4002: // TTTT TTTT
        a->reg4002 = val;
        a->pulse1.timer_period &= 0xff00;
        a->pulse1.timer_period |= val;     // Timer period low (TTTT TTTT)
        break;
    case 0x4003: // LLLL LTTT
        a->reg4003 = val;
        a->pulse1.timer_period &= 0x00ff;
        a->pulse1.timer_period |= ((uint16_t)(val & 0x07) << 8);   // Timer period high 3 bits (TTT)
        a->pulse1.length_counter.value = length_counter_table[(val & 0xf8) >> 3];
        a->pulse1.envelope.start = true;
        a->pulse1.duty_index = 0;
        break;

    // Pulse2 regs
    case 0x4004: // DDLC VVVV
        a->reg4004 = val;
        a->pulse2.duty = (val & 0xc0) >> 6;     // Duty (DD), index into tbl_pulse_waveform
        a->pulse2.length_counter.halt = (val & 0x20) ? true : false;    // Length counter halt (L)
        a->pulse2.envelope.loop = (val & 0x20) ? true : false;          // or Envelope loop (L)
        a->pulse2.envelope.enabled = (val & 0x10) ? false : true;       // Constant volume or Envelope enabled (C)
        a->pulse2.envelope.period = val & 0x0f; // Volume or Period of envelope (VVVV)
        a->pulse2.envelope.start = true;
        break;
    case 0x4005: // EPPP NSSS
        a->reg4005 = val;
        a->pulse2.sweep.enabled = (val & 0x80) ? true : false; // Sweep enabled (E)
        a->pulse2.sweep.period = (val & 0x70) >> 4;            // Sweep period (PPP)
        a->pulse2.sweep.negate = (val & 0x08) ? true : false;  // Negate (N)
        a->pulse2.sweep.shift = val & 0x07;                    // Shift (SSS)
        a->pulse2.sweep.reload = true;
        break;
    case 0x4006: // TTTT TTTT
        a->reg4006 = val;
        a->pulse2.timer_period &= 0xff00;
        a->pulse2.timer_period |= val;     // Timer period low (TTTT TTTT)
        break;
    case 0x4007: // LLLL LTTT
        a->reg4007 = val;
        a->pulse2.timer_period &= 0x00ff;
        a->pulse2.timer_period |= ((uint16_t)(val & 0x07) << 8);   // Timer period high (TTT)
        a->pulse2.length_counter.value = length_counter_table[(val & 0xf8) >> 3];   // Length counter load
        a->pulse2.envelope.start = true;
        a->pulse2.duty_index = 0;
        break;
    // Triangle
    case 0x4008: // CRRR RRR
        a->reg4008 = val;
        a->triangle.length_counter.halt = (val & 0x80) ? true : false;  // Control flag (Length counter halt flag) (C)
        a->triangle.linear_counter_on = (val & 0x80) ? false : true;
        a->triangle.linear_counter_period = (val & 0x7F);   // Counter reload value
        break;
    case 0x4009:
        a->reg4009 = val;
        break;
    case 0x400a: // LLLL LLLL
        a->reg400a = val;
        a->triangle.timer_period &= 0xff00;
        a->triangle.timer_period |= val;    // Timer period low (LLLL LLLL)
        a->triangle.timer_period_bad = ((a->triangle.timer_period >= 0x7fe) || (a->triangle.timer_period <= 1)) ? true : false;
        break;
    case 0x400b: // llll lHHH
        a->reg400b = val;
        a->triangle.timer_period &= 0x00ff;
        a->triangle.timer_period |= ((uint16_t)(val & 0x07) << 8);   // Timer period high (HHH)
        a->triangle.timer_period_bad = ((a->triangle.timer_period >= 0x7fe) || (a->triangle.timer_period <= 1)) ? true : false;
        a->triangle.length_counter.value = length_counter_table[(val & 0xf8) >> 3]; // Length counter load
        a->triangle.linear_counter_reset = true;    // side effect: set linear counter reload
        a->triangle.timer_value = a->triangle.timer_period;   
        break;
    // Noise
    case 0x400c: // --lc vvvv
        a->reg400c = val;
        a->noise.length_counter.halt = (val & 0x20) ? true : false; // Length counter halt (l) 
        a->noise.envelope.loop = (val & 0x20) ? true : false;       // Envelope loop (l)
        a->noise.envelope.enabled = (val & 0x10) ? false : true;    // Constant volume or envelope (c)
        a->noise.envelope.period = (val & 0x0f);    // Volume or envelope divider period (vvvv)
        a->noise.envelope.start = true;
        break;
    case 0x400d:
        a->reg400d = val;
        break;
    case 0x400e: // M--- PPPP
        a->reg400e = val;
        a->noise.mode = (val & 0x80) ? true : false;    // Mode flag (M)
        if (!a->format)  // NTSC lookup table for timer period (PPPP)
        {
            a->noise.timer_period = noise_timer_period_ntsc[val & 0x0f];
        }
        else            // PAL lookup table for timer period (PPPP)
        {
            a->noise.timer_period = noise_timer_period_pal[val & 0x0f];
        }
        break;
    case 0x400f: // llll l---
        a->reg400f = val;
        a->noise.length_counter.value = length_counter_table[(val & 0xf8) >> 3];    // Length counter value (lllll)
        a->noise.envelope.start = true;
        break;
    // DMC
    case 0x4010: // IL-- RRRR
        a->reg4010 = val;
        a->dmc.irq_enabled = (val & 0x80) ? true : false;       // IRQ enable flag (I)
        a->dmc.loop = (val & 0x40) ? true : false;              // Loop flag (L)
        if (!a->dmc.irq_enabled) a->dmc.irq_requested = false;  // No IRQ if channel disabled
        if (!a->format)  // NTSC lookup table for timer period (RRRR)
        {
            a->dmc.timer_period = dmc_timer_period_ntsc[val & 0x0f];
        }
        else            // PAL lookup table for timer period (RRRR)
        {
            a->dmc.timer_period = dmc_timer_period_pal[val & 0x0f];
        }
        break;
    case 0x4011: // -DDD DDDD
        a->reg4011 = val;
        a->dmc.output_value = (val & 0x7f); // Output value (DDD DDDD)
        break;
    case 0x4012: // AAAA AAAA
        a->reg4012 = val;
        a->dmc.sample_addr = 0xc000 + ((uint16_t)val << 6); // Sample address = %11AAAAAA.AA000000 = $C000 + (A * 64)
        break;
    case 0x4013: // LLLL LLLL
        a->reg4013 = val;
        a->dmc.sample_len = ((uint16_t)val << 4) + 1;   // Sample length = %LLLL.LLLL0001 = (L * 16) + 1 bytes
        break;
    // Status
    case 0x4015: // ---D NT21
        a->pulse1.enabled = (val & 0x01) ? true : false;    // 1
        a->pulse2.enabled = (val & 0x02) ? true : false;    // 2
        a->triangle.enabled = (val & 0x04) ? true : false;  // T
        a->noise.enabled = (val & 0x08) ? true : false;     // N
        a->dmc.enabled = (val & 0x10) ? true : false;       // D
        if (!(a->pulse1.enabled)) a->pulse1.length_counter.value = 0;
        if (!(a->pulse2.enabled)) a->pulse2.length_counter.value = 0;
        if (!(a->triangle.enabled)) a->triangle.length_counter.value = 0;
        if (!(a->noise.enabled)) a->noise.length_counter.value = 0;
        // Writing to 0x4015 clears DMC interrupt flag
        a->dmc.irq_requested = false;
        if (a->dmc.enabled)
        {
            // If the DMC bit is set, the DMC sample will be restarted only if its bytes remaining is 0.
            if (a->dmc.read_remaining == 0)
            {
                a->dmc.read_addr = a->dmc.sample_addr;
                a->dmc.read_remaining = a->dmc.sample_len;
                if (!a->dmc.output_shift_reg) {
                    dmc_transfer(&(a->dmc));
                }
            }
            else
            {
                // If there are bits remaining in the 1-byte sample buffer, these will finish playing before the next sample is fetched.
            }
        }
        else
        {
            // If the DMC bit is clear, the DMC bytes remaining will be set to 0 and the DMC will silence when it empties
            a->dmc.read_remaining = 0;
        }
        break;
    // Frame counter
    case 0x4017:
        a->reg4017 = val;
        a->frame_counter.mode = (val & 0x80) ? true : false;
        a->frame_counter.inhibit_irq = (val & 0x40) ? true : false;
        if (a->frame_counter.mode) 
        {
            // write to 0x4017 with bit 7 set immediately clocks all units
            apu_clock_timers(a);
            frame_counter_clock_length_counters(a);
            frame_counter_clock_sweeps(a);
        }
        break;
    default:
        return false;
    }
    return true;
}


static bool apu_read_reg(uint16_t addr, uint8_t* rval, void* cookie)
{
    nesapu_ctx_t* a = (nesapu_ctx_t*)cookie;
    if (0 == a)
        return false;
    switch (addr)
    {
    case 0x4000:
        *rval = a->reg4000;
        break;
    case 0x4001:
        *rval = a->reg4001;
        break;
    case 0x4002:
        *rval = a->reg4002;
        break;
    case 0x4003:
        *rval = a->reg4003;
        break;
    case 0x4004:
        *rval = a->reg4004;
        break;
    case 0x4005:
        *rval = a->reg4005;
        break;
    case 0x4006:
        *rval = a->reg4006;
        break;
    case 0x4007:
        *rval = a->reg4007;
        break;
    case 0x4008:
        *rval = a->reg4008;
        break;
    case 0x4009:
        *rval = a->reg4009;
        break;
    case 0x400a:
        *rval = a->reg400a;
        break;
    case 0x400b:
        *rval = a->reg400b;
        break;
    case 0x400c:
        *rval = a->reg400c;
        break;
    case 0x400d:
        *rval = a->reg400d;
        break;
    case 0x400e:
        *rval = a->reg400e;
        break;
    case 0x400f:
        *rval = a->reg400f;
        break;
    case 0x4010:
        *rval = a->reg4010;
        break;
    case 0x4011:
        *rval = a->reg4011;
        break;
    case 0x4012:
        *rval = a->reg4012;
        break;
    case 0x4013:
        *rval = a->reg4013;
        break;
    case 0x4015:    // IF-D NT21
        // Read 0x4015 is from actual hardware
        *rval = 0x00;
        *rval |= (a->pulse1.length_counter.value > 0) ? 0x01 : 0x00;    
        *rval |= (a->pulse2.length_counter.value > 0) ? 0x02 : 0x00;
        *rval |= (a->triangle.length_counter.value > 0) ? 0x04 : 0x00;
        *rval |= (a->noise.length_counter.value > 0) ? 0x08 : 0x00;
        *rval |= (a->dmc.read_remaining > 0) ? 0x10 : 0x00;
        *rval |= (a->frame_counter.irq_requested) ? 0x40 : 0x00;
        *rval |= (a->dmc.irq_requested) ? 0x80 : 0x00;
        // Read 0x4015 clears frame interrupt flag
        a->frame_counter.irq_requested = false;
        break;
    case 0x4017:
        *rval = a->reg4017;
        break;
    default:
        return false;
    }
    return true;
}


nesapu_ctx_t* nesapu_create(bool format, uint32_t clock, uint32_t srate)
{
    nesapu_ctx_t* a = (nesapu_ctx_t*)malloc(sizeof(nesapu_ctx_t));
    if (0 == a)
        return 0;
    memset(a, 0, sizeof(nesapu_ctx_t));
    a->format = format;
    a->clock_rate = clock;
    a->sample_rate = srate;
    // Ideal frame counter cycles = clock_rate / 240.0f (frame counter clocked at 240Hz)
    // Actual frame counter cycles = clock_rate / 240 = cycles_per_frame
    // Frame cycle error = clock_rate / 240.0f - cycles_per_frame
    // Use Q16_16 fixed point float
    // When frame_cycle_error accumulated >= 65536 an extra cycle must be inserted
    a->cycles_per_frame = a->clock_rate / 240;
    a->frame_cycle_error = ((int32_t)(a->clock_rate - a->cycles_per_frame * 240) << 16) / 240;
    a->accumulated_frame_cycle_error = 0;
    nesapu_reset(a);
    return a;
}


void nesapu_destroy(nesapu_ctx_t* a)
{
    if (a != 0)
        free(a);
}


bool nesapu_attach_bus(nesapu_ctx_t* a, nesbus_ctx_t* b)
{
    bool r;
    if ((0 == a) || (0 == b))
        return false;
    a->dmc.bus = b; // DMC need access bus
    r = nesbus_add_read_handler(b, "APU", 0x4000, 0x4013, apu_read_reg, (void*)a);
    r &= nesbus_add_read_handler(b, "APU", 0x4015, 0x4015, apu_read_reg, (void*)a);
    r &= nesbus_add_read_handler(b, "APU", 0x4017, 0x4017, apu_read_reg, (void*)a);
    r &= nesbus_add_write_handler(b, "APU", 0x4000, 0x4013, apu_write_reg, (void*)a);
    r &= nesbus_add_write_handler(b, "APU", 0x4015, 0x4015, apu_write_reg, (void*)a);
    r &= nesbus_add_write_handler(b, "APU", 0x4017, 0x4017, apu_write_reg, (void*)a);
    return r;
}


void nesapu_reset(nesapu_ctx_t* a)
{
    a->reg4000 = a->reg4001 = a->reg4002 = a->reg4003 = 0x00;
    a->reg4004 = a->reg4005 = a->reg4006 = a->reg4007 = 0x00;
    a->reg4008 = a->reg4009 = a->reg400a = a->reg400b = 0x00;
    a->reg400c = a->reg400d = a->reg400e = a->reg400f = 0x00;
    a->reg4010 = a->reg4011 = a->reg4012 = a->reg4013 = 0x00;
    a->reg4017 = 0x00;
    a->cycles = 0;
    a->next_frame_cycle = 0;
    a->accumulated_frame_cycle_error = 32767;   // Euquivalent of 0.5 cycle initial error
    pulse_reset(&(a->pulse1));
    pulse_reset(&(a->pulse2));
    triangle_reset(&(a->triangle));
    noise_reset(&(a->noise));
    dmc_reset(&(a->dmc));
    frame_counter_reset(&(a->frame_counter));
    a->seq_step = 0;
}


void nesapu_clock(nesapu_ctx_t* a)
{
    apu_clock_timers(a);

    // Frame Counter
    if (a->cycles == a->next_frame_cycle)
    {
        // mode 0:    mode 1:       function
        // false      true
        // ---------  -----------  -----------------------------
        //  - - - f    - - - - -    IRQ (if bit 6 is clear)
        //  - l - l    - l - - l    Length counter and sweep
        //  e e e e    e e e - e    Envelope and linear counter
        if (a->frame_counter.mode)
        {
            // mode 1
            switch (a->seq_step % 5)
            {
            case 0:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                break;
            case 1:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                frame_counter_clock_length_counters(a);
                frame_counter_clock_sweeps(a);
                break;
            case 2:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                break;
            case 3:
                // Nothing
                break;
            case 4:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                frame_counter_clock_length_counters(a);
                frame_counter_clock_sweeps(a);
                break;
            }
        }
        else
        {
            // mode 0
            switch (a->seq_step % 4)
            {
            case 0:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                break;
            case 1:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                frame_counter_clock_length_counters(a);
                frame_counter_clock_sweeps(a);
                break;
            case 2:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                break;
            case 3:
                frame_counter_clock_envelopes(a);
                frame_counter_clock_linear_counter(a);
                frame_counter_clock_length_counters(a);
                frame_counter_clock_sweeps(a);
                if (a->frame_counter.inhibit_irq == false)
                {
                    a->frame_counter.irq_requested = true;
                }
                break;
            }
        }
        ++(a->seq_step);
        // Schedule next frame clock
        a->next_frame_cycle = a->cycles + a->cycles_per_frame;
        a->accumulated_frame_cycle_error += a->frame_cycle_error;
        if (a->accumulated_frame_cycle_error & 0xFFFF0000) // if error >= 65536
        {
            ++(a->next_frame_cycle);
            a->accumulated_frame_cycle_error &= 0xFFFF;
        }
    }
    ++(a->cycles);
}


nesfloat_t nesapu_sample(nesapu_ctx_t* a)
{
    nesfloat_t sample = mixer_sample(
        pulse_output(&(a->pulse1)),
        pulse_output(&(a->pulse2)),
        triangle_output(&(a->triangle)),
        noise_output(&(a->noise)),
        dmc_output(&(a->dmc)));
    return sample;
}


bool nesapu_irq_requested(nesapu_ctx_t* a)
{
    // Frame counter and DMC can raise IRQ
    return (a->frame_counter.irq_requested || a->dmc.irq_requested);
}


bool nesapu_dmc_stall_cpu(nesapu_ctx_t* a)
{
    if (a->dmc.stall_cpu)
    {
        a->dmc.stall_cpu = false;
        return true;
    }
    return false;
}
