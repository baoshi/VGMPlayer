#include <pico/critical_section.h>
#include "my_mem.h"
#include "audio_buffer.h"

//audio_frame_t __attribute__((section(".core1ramdata"))) buffer[3];
audio_frame_t buffer[3];

struct audio_cbuf_s
{
	audio_frame_t *buffer;
	unsigned int head;
	unsigned int tail;
	unsigned int max;
	bool full;
	critical_section_t lock;
};


audio_cbuf_t * audio_cbuf_create(unsigned int blocks)
{
	audio_cbuf_t *cbuf = (audio_cbuf_t *)MY_MALLOC(sizeof(audio_cbuf_t));
	if (NULL == cbuf)
		return NULL;
	cbuf->buffer = buffer; // MY_MALLOC(sizeof(audio_frame_t) * blocks);
	if (NULL == cbuf->buffer)
	{
		MY_FREE(cbuf);
		return NULL;
	}
	cbuf->max = blocks;
	critical_section_init(&(cbuf->lock));
	audio_cbuf_reset(cbuf);
	return cbuf;
}


void audio_cbuf_destroy(audio_cbuf_t *me)
{
	if (me)
	{
		critical_section_deinit(&(me->lock));
		//if (me->buffer) MY_FREE(me->buffer);
		MY_FREE(me);
	}
}


void audio_cbuf_reset(audio_cbuf_t *me)
{
	me->head = 0;
	me->tail = 0;
	me->full = false;
}


unsigned int audio_cbuf_size(audio_cbuf_t *me)
{
	unsigned int size = me->max;
	if (!(me->full))
	{
		if (me->head >= me->tail)
		{
			size = (me->head - me->tail);
		}
		else
		{
			size = (me->max + me->head - me->tail);
		}
	}
	return size;
}


audio_frame_t * audio_cbuf_get_write_buffer(audio_cbuf_t *me)
{
	audio_frame_t *r = NULL;
	if (!me->full)
	{
		r = &(me->buffer[me->head]);
	}
	return r;
}


void audio_cbuf_finish_write(audio_cbuf_t *me)
{
	critical_section_enter_blocking(&(me->lock));
	if (++(me->head) == me->max) me->head = 0;
	me->full = (me->head == me->tail);
	critical_section_exit(&(me->lock));
}


audio_frame_t * audio_cbuf_get_read_buffer(audio_cbuf_t *me)
{
	audio_frame_t *r = NULL;
	if (me->full || (me->head != me->tail))	// not empty
	{
		r = &(me->buffer[me->tail]);
	}
	return r;
}


void audio_cbuf_finish_read(audio_cbuf_t *me)
{
	critical_section_enter_blocking(&(me->lock));
	if (++(me->tail) == me->max) me->tail = 0;
	me->full = false;
	critical_section_exit(&(me->lock));
}
