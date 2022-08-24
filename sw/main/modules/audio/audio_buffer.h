#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sw_conf.h"

typedef struct audio_frame_s
{
    uint32_t data[AUDIO_FRAME_LENGTH];
    unsigned int length;
} audio_frame_t;

/// Opaque circular buffer structure
typedef struct audio_cbuf_s audio_cbuf_t;

audio_cbuf_t * audio_cbuf_create(unsigned int blocks);
void audio_cbuf_destroy(audio_cbuf_t *me);
void audio_cbuf_reset(audio_cbuf_t *me);
unsigned int audio_cbuf_size(audio_cbuf_t *me);
audio_frame_t * audio_cbuf_get_write_buffer(audio_cbuf_t *me);
void audio_cbuf_finish_write(audio_cbuf_t *me);
audio_frame_t * audio_cbuf_get_read_buffer(audio_cbuf_t *me);
void audio_cbuf_finish_read(audio_cbuf_t *me);
