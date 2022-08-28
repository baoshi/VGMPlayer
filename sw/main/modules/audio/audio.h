#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <umm_malloc_cfg.h>
#include <umm_malloc.h>
#include "my_debug.h"
#include "file_reader.h"
#include "decoder.h"


#ifndef AUDIO_DEBUG
#  define AUDIO_DEBUG 1
#endif

// Debug log
#if AUDIO_DEBUG
#define AUD_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define AUD_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define AUD_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define AUD_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define AUD_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define AUD_LOGD(x, ...)
#define AUD_LOGW(x, ...)
#define AUD_LOGE(x, ...)
#define AUD_LOGI(x, ...)
#define AUD_DEBUGF(x, ...)
#endif


// audio frame length. make it 2's power for easy FFT. also NESAPU_MAX_SAMPLE_SIZE in vgm_conf.h need to be larger than this.
#define AUDIO_FRAME_LENGTH              2048
// audio circular buffer length, we will statically allocate AUDIO_CBUF_LENGTH or AUDIO_FRAME_LENGTH samples
#define AUDIO_CBUF_LENGTH               3
// audio sample rate
#define AUDIO_SAMPLE_RATE               44100
#define AUDIO_SONG_GAP_MS               2000


#ifdef __cplusplus
extern "C" {
#endif

void audio_preinit();
void audio_postinit();
void audio_close();

void audio_start_playback(decoder_t *decoder);
void audio_pause_playback();
void audio_resume_playback();
void audio_stop_playback();
void audio_finish_playback();
void audio_set_speaker_volume(int8_t vol);
void audio_set_headphone_volume(int8_t vol);

int  audio_jack_detect(uint32_t now);
bool audio_get_jack_state();
void audio_jack_enable(bool enable);


//
// Audio memory management
//
void audio_mem_init();
#define audio_malloc    umm_malloc
#define audio_calloc    umm_calloc
#define audio_realloc   umm_realloc
#define audio_free      umm_free
// use this to define variable in audio heap. e.g
// static uint32_t __audio_ram("my_group_name") array[3];
#define __audio_ram(group) __attribute__((section(".core1mem." group)))


//
// Audio circular buffer
//
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


#ifdef __cplusplus
}
#endif