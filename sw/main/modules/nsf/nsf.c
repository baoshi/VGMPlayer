#include <stdlib.h>
#include <memory.h>
#include "nesfloat.h"
#include "nsf.h"


#define EMU_RAM1_BASE 0x0000
#define EMU_RAM1_SIZE 2048
#define EMU_RAM2_BASE 0x6000
#define EMU_RAM2_SIZE 8192

#define APU_CLOCK_NTSC 1789773
#define APU_CLOCK_PAL  1662607

// Default slient detection time (2000ms)
#define SILENT_DETECTION_MS  2000


//
// Emulator RAM read/write handlers
//

static bool ram1_read(uint16_t addr, uint8_t* rval, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    if (0 == c)
        return false;
    if (0 == c->ram1)
        return false;
    *rval = c->ram1[addr - EMU_RAM1_BASE];
    return true;
}


static bool ram1_write(uint16_t addr, uint8_t val, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    if (0 == c)
        return false;
    if (0 == c->ram1)
        return false;
    c->ram1[addr - EMU_RAM1_BASE] = val;
    return true;
}


static bool ram2_read(uint16_t addr, uint8_t* rval, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    if (0 == c)
        return false;
    if (0 == c->ram2)
        return false;
    *rval = c->ram2[addr - EMU_RAM2_BASE];
    return true;
}


static bool ram2_write(uint16_t addr, uint8_t val, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    if (0 == c)
        return false;
    if (0 == c->ram2)
        return false;
    c->ram2[addr - EMU_RAM2_BASE] = val;
    return true;
}


//
// Emulator ROM (NSF) read handlers
//

static bool rom_read_nonbankswitched(uint16_t addr, uint8_t* rval, void* cookie)
{
    // Non bank-switched NSF rom, music data from c->music_offset is loaded to c->header->load_addr
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    if (c->reader)
    {
        int32_t offset = addr - c->header->load_addr; // offset in music data
        // address range should be gauranteed by nesbus module
        if (1 == c->reader->read(c->reader->self, rval, c->music_offset + offset, 1))
            return true;
    }
    return false;
}


static bool ram_write_bankswitch_reg(uint16_t addr, uint8_t val, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    // addr is bank switch register 5FF8-5FFF and val is bank number
    // https://wiki.nesdev.com/w/index.php/NSF
    // Register  Address       Page
    // --------  -----------   ----
    // 5FF8      8000 - 8FFF   8
    // 5FF9      9000 - 9FFF   9
    // 5FFA      A000 - AFFF   A
    // 5FFB      B000 - BFFF   B
    // 5FFC      C000 - CFFF   C
    // 5FFD      D000 - DFFF   D
    // 5FFE      E000 - EFFF   E
    // 5FFF      F000 - FFFF   F
    // Below:
    // (c->header->load_addr & 0x0fff) is the number of pad bytes 
    // (val << 12) converts bank number into bank starting address
    // c->bank[0] is the offset to music data that will to be loaded to page 8 (0x8000), etc...
    int page = addr & 0x0F; // 5FF8 -> Page 8, ... etc
    c->bank[page - 8] = ((int32_t)val << 12) - (c->header->load_addr & 0x0fff) ;
    return true;
}


static bool rom_read_bankswitched(uint16_t addr, uint8_t* rval, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    int bank = (addr & 0xf000) >> 12;           // 8000-ffff -> bank 8 - bank f
    int offset = addr & 0x0fff;                 // offset in bank
    int32_t loc = c->bank[bank - 8] + offset;   // actual address in the music data
    if (loc >= 0)
    {
        if (1 == c->reader->read(c->reader->self, rval, c->music_offset + loc, 1))
        {
            return true;
        }
    }
    // Not valid address
    return false;
}


//
// NSF INIT wrap
//
//
// Set 4015 and call NSF init routine
// From: https://wiki.nesdev.com/w/index.php/Talk:NSF
// Regarding 4015h, well... it's empirical. My experience says that setting 4015h to 0Fh
// is required in order to get *a lot of* tunes starting playing. I don't remember of *any* broken
// tune by setting such value. So, it's recommended *to follow* such thing. --Zepper 14:25, 29 March 2012 (PDT)
// NSF init routine address should be placed in byte NSF_EMU_INIT_WRAP_BASE + 1 and NSF_EMU_INIT_WRAP_BASE + 2
#define NSF_EMU_INIT_WRAP_BASE  0x5000
#define NSF_EMU_INIT_WRAP_SIZE  11
static const uint8_t rom_init_wrap[NSF_EMU_INIT_WRAP_SIZE] =
{
    0x48,               // PHA
    0xA9, 0x0F,         // LDA #$0F
    0x8D, 0x15, 0x40,   // STA $4015
    0x68,               // PLA
    0x20, 0x00, 0x00,   // JSR $xxxx (+8 LL) (+9 HH)
    0xF2                // JAM
};
static bool rom_read_init_wrap(uint16_t addr, uint8_t* rval, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    if (addr == NSF_EMU_INIT_WRAP_BASE + 8)
    {   
        // low byte of init address
        *rval = (uint8_t)(c->header->init_addr & 0xFF);
        return true;
    }
    if (addr == NSF_EMU_INIT_WRAP_BASE + 9)
    {
        // high byte of init address
        *rval = (uint8_t)((c->header->init_addr >> 8) & 0xFF);
        return true;
    }
    // address range should be gauranteed by nesbus module
    *rval = rom_init_wrap[addr - NSF_EMU_INIT_WRAP_BASE];
    return true;
}


//
// NSF PLAY wrap
//
#define NSF_EMU_PLAY_WRAP_BASE  0x5020
#define NSF_EMU_PLAY_WRAP_SIZE  4
//static const uint8_t rom_play_wrap[NSF_EMU_PLAY_WRAP_SIZE] =
//{
//    0x20, 0x00, 0x00,   // JSR $xxxx
//    0xF2                // JAM
//};
static bool rom_read_play_wrap(uint16_t addr, uint8_t* rval, void* cookie)
{
    nsf_ctx_t* c = (nsf_ctx_t*)cookie;
    switch (addr)
    {
    case NSF_EMU_PLAY_WRAP_BASE:        // JSR
        *rval = 0x20;
        break;
    case NSF_EMU_PLAY_WRAP_BASE + 1:    // low byte of init address
        *rval = (uint8_t)(c->header->play_addr & 0xFF);
        break;
    case NSF_EMU_PLAY_WRAP_BASE + 2:    // high byte of init address
        *rval = (uint8_t)((c->header->play_addr >> 8) & 0xFF);
        break;
    case NSF_EMU_PLAY_WRAP_BASE + 3:    // JAM
        *rval = 0xF2;
        break;
    }
    return true;
}


//
// NSF exported functions
//

nsf_ctx_t* nsf_create()
{
    nsf_ctx_t* c = (nsf_ctx_t*)malloc(sizeof(nsf_ctx_t));
    if (0 == c)
        return 0;
    memset(c, 0, sizeof(nsf_ctx_t));
    return c;
}


void nsf_destroy(nsf_ctx_t* c)
{
    if (c != 0)
    {
        nsf_stop_emu(c);
        free(c);
    }
}


int nsf_start_emu(nsf_ctx_t* c, nsfreader_ctx_t* reader, uint16_t max_sample_count, uint32_t sample_rate, uint8_t oversample)
{
    int ret, i;
    if (0 == c)
    {
        ret = NSF_ERR_INVALIDPARAM;
        goto start_exit;
    }
    if (0 == reader)
    {
        ret = NSF_ERR_INVALIDPARAM;
        goto start_exit;
    }
    c->header = (nsf_header_t*)malloc(sizeof(nsf_header_t));
    if (0 == c->header)
    {
        ret = NSF_ERR_OUTOFMEMORY;
        goto start_exit;
    }
    // Load header
    if (sizeof(nsf_header_t) != reader->read(reader->self, (uint8_t*)(c->header), 0, sizeof(nsf_header_t)))
    {
        ret = NSF_ERR_UNSUPPORTED;
        goto start_exit;
    }
    // Check signature
    if (memcmp(&(c->header->id), "NESM\x1A", 5))
    {
        ret = NSF_ERR_UNSUPPORTED;
        goto start_exit;
    }
    // Check version
    if (c->header->version != 0x01)
    {
        ret = NSF_ERR_UNSUPPORTED;
        goto start_exit;
    }
    // Not support any extension yet
    if (0 != c->header->ext_sound_type)
    {
        ret = NSF_ERR_UNSUPPORTED;
        goto start_exit;
    }
    // NSF seems valid
    c->music_offset = sizeof(nsf_header_t);
    c->music_length = reader->size(reader->self) - c->music_offset;
    c->reader = reader;
    // Format
    if (c->header->pal_ntsc_bits & 0x02)
    {
        c->format = false;  // Dual NTSC/PAL format, we will use NTSC
    }
    else
    {
        if (c->header->pal_ntsc_bits & 0x01)
        {
            c->format = true;   // PAL
        }
        else
        {
            c->format = false;  // NTSC
        }
    }
    // Playback rate
    if (c->format)
    {
        c->playback_rate = c->header->pal_speed;
        c->clock_rate = APU_CLOCK_PAL;
    }
    else
    {
        c->playback_rate = c->header->ntsc_speed;
        c->clock_rate = APU_CLOCK_NTSC;
    }
    // Setup sampling clocks
    c->output_sample_rate = sample_rate;
    c->oversample = oversample;
    c->apu_sample_rate = c->output_sample_rate * c->oversample;
    //
    // APU Sampling timing
    // Ideal APU sample cycles = (double)clock_rate / apu_sample_rate
    // Actual APU sample cycles = clock_rate / apu_sample_rate = cycles_per_apu_sample
    c->cycles_per_apu_sample = c->clock_rate / c->apu_sample_rate;
    // Due to truncation, cycles_per_apu_sample is less than actual required cycles
    // Sample cycle error = (double)clock_rate / apu_sample_rate - cycles_per_apu_sample
    //                    = (clock_rate - (apu_sample_rate * cycles_per_apu_sample)) /  apu_sample_rate
    // We use Q16_16 fixed point (<< 16)
    // When sample_cycle_error accumulated >= 65536 an extra cycle must be inserted
    c->apu_sample_cycle_error = ((int32_t)(c->clock_rate - c->cycles_per_apu_sample * c->apu_sample_rate) << 16) / c->apu_sample_rate;
    c->accumulated_apu_sample_cycle_error = 0;
    c->next_apu_sample_cycle = 0;
    //
    // Playback timing
    // Playback clock ensures that we call "PLAY" routine at playback_rate
    // Ideal Playback rate  = (double)clock_rate / (1000000 / playback_rate)
    // Actual playback rate = clock_rate / (1000000 / playback_rate) = clock_rate * playback_rate / 1000000 
    c->cycles_per_playback = (uint32_t)((uint64_t)c->clock_rate * c->playback_rate / 1000000);    // consider 32-bit overflow
    // Due to truncation, cycles_per_playback is les than actual required cycles
    // Playback cycle error = (double)clock_rate / (1000000 / playback_rate) - cycles_per_playback
    //                      = (clock_rate * playback_rate / 1000000) - cycles_per_playback
    //                      = (clock_rate * playback_rate - cycles_per_playback) / 1000000
    // We use Q16_16 fixed point (<< 16)
    // When playback_cycle_error accumulated >= 65536 an extra cycle must be inserted
    c->playback_cycle_error = (uint32_t)((((uint64_t)c->clock_rate * c->playback_rate - (uint64_t)c->cycles_per_playback * 1000000) << 16) / 1000000);
    c->accumulated_playback_cycle_error = 0;
    c->next_playback_cycle = 0;
    // Construct emulator
    // Read:  APU 3, RAM 1, NSF 1, INIT 1, PLAY 1, total 6
    // Write: APU 3, RAM 1, BANK REG 1, total 4
    c->bus = nesbus_create(8, 6);
    if (0 == c->bus)
    {
        ret = NSF_ERR_OUTOFMEMORY;
        goto start_exit;
    }
    c->cpu = nescpu_create();
    if (0 == c->cpu)
    {
        ret = NSF_ERR_OUTOFMEMORY;
        goto start_exit;
    }
    nescpu_attach_bus(c->cpu, c->bus);
    c->apu = nesapu_create(c->format, c->format ? APU_CLOCK_PAL : APU_CLOCK_NTSC, sample_rate);
    if (0 == c->apu)
    {
        ret = NSF_ERR_OUTOFMEMORY;
        goto start_exit;
    }
    nesapu_attach_bus(c->apu, c->bus);
    // RAM
    c->ram1 = (uint8_t*)malloc(EMU_RAM1_SIZE);
    if (0 == c->ram1)
    {
        ret = NSF_ERR_OUTOFMEMORY;
        goto start_exit;
    }
    memset(c->ram1, 0, EMU_RAM1_SIZE);
    nesbus_add_read_handler(c->bus, "RAM1", EMU_RAM1_BASE, EMU_RAM1_BASE + EMU_RAM1_SIZE - 1, ram1_read, (void*)c);
    nesbus_add_write_handler(c->bus, "RAM1", EMU_RAM1_BASE, EMU_RAM1_BASE + EMU_RAM1_SIZE - 1, ram1_write, (void*)c);
    c->ram2 = (uint8_t*)malloc(EMU_RAM2_SIZE);
    if (0 == c->ram2)
    {
        ret = NSF_ERR_OUTOFMEMORY;
        goto start_exit;
    }
    memset(c->ram2, 0, EMU_RAM2_SIZE);
    nesbus_add_read_handler(c->bus, "RAM2", EMU_RAM2_BASE, EMU_RAM2_BASE + EMU_RAM2_SIZE - 1, ram2_read, (void*)c);
    nesbus_add_write_handler(c->bus, "RAM2", EMU_RAM2_BASE, EMU_RAM2_BASE + EMU_RAM2_SIZE - 1, ram2_write, (void*)c);
    // ROM from .nsf file
    c->bank_switched = false;   // check if NSF file uses bank switch
    for (i = 0; i < 8; ++i)
    {
        if (c->header->bankswitch_info[i])
        {
            c->bank_switched = true;
            break;
        }
    }
    if (!c->bank_switched)
    {
        // Non bank-switched NSF rom, music data from c->music is loaded to c->header->load_addr
        nesbus_add_read_handler(c->bus, "NSF_ROM", c->header->load_addr, c->header->load_addr + c->music_length - 1, rom_read_nonbankswitched, (void*)c);
    }
    else
    {
        // Use bankswitch_info in the header to initialize bank switch register
        for (i = 0; i < 8; ++i)
        {
            ram_write_bankswitch_reg(0x5ff8 + i, c->header->bankswitch_info[i], (void*)c);
        }
        // Bank switched NSF rom can extend to full address range
        nesbus_add_read_handler(c->bus, "NSF_BANK", 0x8000, 0xFFFF, rom_read_bankswitched, (void*)c);
        // Bank switch register
        nesbus_add_write_handler(c->bus, "NSF_BANK_REG", 0x5FF8, 0x5FFF, ram_write_bankswitch_reg, (void*)c);
    }
    // INIT wrap
    nesbus_add_read_handler(c->bus, "NSF_INIT", NSF_EMU_INIT_WRAP_BASE, NSF_EMU_INIT_WRAP_BASE + NSF_EMU_INIT_WRAP_SIZE - 1, rom_read_init_wrap, (void*)c);
    // PLAY wrap
    nesbus_add_read_handler(c->bus, "NSF_PLAY", NSF_EMU_PLAY_WRAP_BASE, NSF_EMU_PLAY_WRAP_BASE + NSF_EMU_PLAY_WRAP_SIZE - 1, rom_read_play_wrap, (void*)c);
    ret = NSF_ERR_SUCCESS;
    // BLIP buffer
    c->blip_buffer_size = max_sample_count;
    c->blip = blip_new(max_sample_count);
    blip_set_rates(c->blip, c->clock_rate, c->output_sample_rate);
    c->blip_last_sample = 0;
    // Silent detection
    c->slient_cycles_target = c->clock_rate * (SILENT_DETECTION_MS / 100) / 10;
    c->slient_cycles_count = 0;
start_exit:
    if ((ret != NSF_ERR_SUCCESS) && (c != 0))
        nsf_stop_emu(c);
    return ret;
}


void nsf_stop_emu(nsf_ctx_t* c)
{
    if (0 == c)
    {
        return;
    }
    if (c->blip)
    {
        blip_delete(c->blip);
        c->blip = 0;
    }
    if (c->ram2)
    {
        free(c->ram2);
        c->ram2 = 0;
    }
    if (c->ram1)
    {
        free(c->ram1);
        c->ram1 = 0;
    }
    if (c->apu)
    {
        nesapu_destroy(c->apu);
        c->apu = 0;
    }
    if (c->cpu)
    {
        nescpu_destroy(c->cpu);
        c->cpu = 0;
    }
    if (c->bus)
    {
        nesbus_destroy(c->bus);
        c->bus = 0;
    }
    if (c->header)
    {
        free(c->header);
        c->header = 0;
    }
}


int nsf_init_song(nsf_ctx_t* c, uint8_t song)
{
    if (0 == c)
    {
        return NSF_ERR_INVALIDPARAM;
    }
    if (0 == c->cpu || 0 == c->apu || 0 == c->bus || 0 == c->ram1 || 0 == c->ram2 || 0 == c->blip)
    {
        return NSF_ERR_NOT_INITIALIZED;
    }
    // clear ram
    memset(c->ram1, 0, sizeof(EMU_RAM1_SIZE));
    memset(c->ram2, 0, sizeof(EMU_RAM2_SIZE));
    nescpu_reset(c->cpu, false);
    nescpu_set_pc(c->cpu, NSF_EMU_INIT_WRAP_BASE);
    nescpu_set_a(c->cpu, song);                        // desired song #
    nescpu_set_x(c->cpu, c->format ? 0x01 : 0x00);     // PAL=1, NTSC=0
    nescpu_set_y(c->cpu, 0x00);
    do
    {
        if (nescpu_clock(c->cpu))    // nescpu_clock returns true if JAMed
            break;
    } while (1);
    blip_clear(c->blip);
    c->cycles = 0;
    c->slient_cycles_count = 0;
    c->silent = false;
    return NSF_ERR_SUCCESS;
}


int nsf_get_samples(nsf_ctx_t* c, uint16_t count, int16_t* samples)
{
    if (0 == c)
    {
        return NSF_ERR_INVALIDPARAM;
    }
    if (0 == c->cpu || 0 == c->apu || 0 == c->bus || 0 == c->ram1 || 0 == c->ram2 || 0 == c->blip)
    {
        return NSF_ERR_NOT_INITIALIZED;
    }
    if (count > c->blip_buffer_size)
    {
        return NSF_ERR_INVALIDPARAM;
    }
    unsigned int needed_clocks = (unsigned int)blip_clocks_needed(c->blip, count);
    for (unsigned int i = 0; i < needed_clocks; ++i)
    {
        // Call PLAY ROUNTINE at playback rate
        if (c->cycles == c->next_playback_cycle)
        {
            if (nescpu_is_jammed(c->cpu))   // restart PLAY ROUTINE
            {
                nescpu_set_a(c->cpu, 0x00);
                nescpu_set_x(c->cpu, 0x00);
                nescpu_set_y(c->cpu, 0x00);
                nescpu_set_pc(c->cpu, NSF_EMU_PLAY_WRAP_BASE);
                nescpu_unjam(c->cpu);
            }
            // Schedule next sample
            c->next_playback_cycle = c->cycles + c->cycles_per_playback;
            c->accumulated_playback_cycle_error += c->playback_cycle_error;
            if (c->accumulated_playback_cycle_error & 0xffff0000) // if error >= 65536?
            {
                ++(c->next_playback_cycle);
                c->accumulated_playback_cycle_error &= 0x0000ffff;
            }
        }
        // Clock CPU/APU
        nescpu_clock(c->cpu);   // can clock CPU even if it is jammed
        nesapu_clock(c->apu);   // NES APU running same clock as CPU
        if (nesapu_dmc_stall_cpu(c->apu))
        {
            // Test if APU requested DMC transfer (stall CPU for 4 cycles)
            nescpu_skip_cycles(c->cpu, 4);
        }
        if (nesapu_irq_requested(c->apu))
        {
            // Test if APU asserted IRQ
            nescpu_irq(c->cpu);
        }

        // Sample APU if needed
        if (c->cycles == c->next_apu_sample_cycle)
        {
            // Take sample
            int16_t s = nesfloat_to_sample(nesapu_sample(c->apu));
            int16_t delta = s - c->blip_last_sample;
            c->blip_last_sample = s;
            blip_add_delta(c->blip, i, delta);
            if (0 == delta)
            {
                c->slient_cycles_count += c->cycles_per_apu_sample;
                if (c->slient_cycles_count >= c->slient_cycles_target)
                {
                    c->silent = true;
                }
            }
            else
            {
                c->slient_cycles_count = 0;
            }
            // Schedule next sample
            c->next_apu_sample_cycle = c->cycles + c->cycles_per_apu_sample;
            c->accumulated_apu_sample_cycle_error += c->apu_sample_cycle_error;
            if (c->accumulated_apu_sample_cycle_error & 0xffff0000) // if error >= 65536?
            {
                ++(c->next_apu_sample_cycle);
                c->accumulated_apu_sample_cycle_error &= 0x0000ffff;
            }
        }
        ++(c->cycles);
    }
    blip_end_frame(c->blip, needed_clocks);
    return blip_read_samples(c->blip, (short*)samples, count, 0);    // read out sample even if silent
}


int nsf_set_slience_detect(nsf_ctx_t* c, uint32_t ms)
{
    if (0 == c)
    {
        return NSF_ERR_INVALIDPARAM;
    }
    if (0 == c->cpu || 0 == c->apu || 0 == c->bus || 0 == c->ram1 || 0 == c->ram2 || 0 == c->blip)
    {
        return NSF_ERR_NOT_INITIALIZED;
    }
    if (ms > 60000) // to large
    {
        return NSF_ERR_INVALIDPARAM;
    }
    c->slient_cycles_target = c->clock_rate * (ms / 100) / 10;
    c->slient_cycles_count = 0;
    return NSF_ERR_SUCCESS;
}


bool nsf_silence_detected(nsf_ctx_t* c)
{
    if (0 == c)
    {
        // not initialized? silence of course
        return true;    
    }
    return (c->silent);
}
