#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "decoder.h"


#ifdef __cplusplus
extern "C" {
#endif

// audio frame length. 1470 is 1/30s in 44100 sample rate
#define AUDIO_FRAME_LENGTH              1470
// audio circular buffer length, we will statically allocate AUDIO_CBUF_LENGTH or AUDIO_FRAME_LENGTH samples
#define AUDIO_CBUF_LENGTH               3
// audio sample rate
#define AUDIO_SAMPLE_RATE               44100
#define AUDIO_SONG_GAP_MS               2000


void audio_preinit();
void audio_postinit();
void audio_close();

void audio_setup_playback(decoder_t *decoder);
void audio_start_playback();
void audio_pause_playback();
void audio_resume_playback();
void audio_stop_playback();
void audio_finish_playback();
void audio_set_speaker_volume(int8_t vol);
void audio_set_headphone_volume(int8_t vol);

int  audio_jack_detect(uint32_t now);
bool audio_get_jack_state();
void audio_jack_enable(bool enable);


/* find in audio_memory.c */

void audio_mem_init();
#define audio_malloc    umm_malloc
#define audio_calloc    umm_calloc
#define audio_realloc   umm_realloc
#define audio_free      umm_free

/* use this to define variable in audio heap. e.g
 * static uint32_t __audio_ram("my_group_name") array[3];
 */
#define __audio_ram(group) __attribute__((section(".core1mem." group)))

/* find in audio_buffer.c */

typedef struct audio_frame_s
{
    uint32_t data[AUDIO_FRAME_LENGTH];
    unsigned int length;
} audio_frame_t;

/// Opaque circular buffer structure
typedef struct audio_cbuf_s audio_cbuf_t;

void audio_cbuf_init();
void audio_cbuf_reset();
unsigned int audio_cbuf_size();
audio_frame_t * audio_cbuf_get_write_buffer();
void audio_cbuf_finish_write();
audio_frame_t * audio_cbuf_get_read_buffer();
void audio_cbuf_finish_read();

extern audio_cbuf_t *audio_buffer;



#ifdef __cplusplus
}
#endif