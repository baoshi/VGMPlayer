#include <pico/critical_section.h>
#include "audio.h"


struct audio_cbuf_s
{
	audio_frame_t buffer[AUDIO_CBUF_LENGTH];
	unsigned int head;
	unsigned int tail;
	bool full;
	critical_section_t lock;
};


static audio_cbuf_t __audio_ram("audio") cbuf;


void audio_cbuf_init()
{
	critical_section_init(&(cbuf.lock));
	audio_cbuf_reset();
}


void audio_cbuf_reset()
{
	critical_section_enter_blocking(&(cbuf.lock));
	cbuf.head = 0;
	cbuf.tail = 0;
	cbuf.full = false;
	critical_section_exit(&(cbuf.lock));
}


unsigned int audio_cbuf_size()
{
	unsigned int size = AUDIO_CBUF_LENGTH;
	if (!(cbuf.full))
	{
		if (cbuf.head >= cbuf.tail)
		{
			size = (cbuf.head - cbuf.tail);
		}
		else
		{
			size = (AUDIO_CBUF_LENGTH + cbuf.head - cbuf.tail);
		}
	}
	return size;
}


audio_frame_t * audio_cbuf_get_write_buffer()
{
	audio_frame_t *r = NULL;
	if (!cbuf.full)
	{
		r = &(cbuf.buffer[cbuf.head]);
	}
	return r;
}


void audio_cbuf_finish_write()
{
	critical_section_enter_blocking(&(cbuf.lock));
	if (++(cbuf.head) == AUDIO_CBUF_LENGTH) cbuf.head = 0;
	cbuf.full = (cbuf.head == cbuf.tail);
	critical_section_exit(&(cbuf.lock));
}


audio_frame_t * audio_cbuf_get_read_buffer()
{
	audio_frame_t *r = NULL;
	if (cbuf.full || (cbuf.head != cbuf.tail))	// not empty
	{
		r = &(cbuf.buffer[cbuf.tail]);
	}
	return r;
}


void audio_cbuf_finish_read()
{
	critical_section_enter_blocking(&(cbuf.lock));
	if (++(cbuf.tail) == AUDIO_CBUF_LENGTH) cbuf.tail = 0;
	cbuf.full = false;
	critical_section_exit(&(cbuf.lock));
}
