#include <stdint.h>
#include "audio.h"


// "Heap" used by audio function in core1mem region

#define AUDIO_HEAP_SIZE 0x9F00
static uint8_t __audio_ram("heap") audio_heap[AUDIO_HEAP_SIZE];


void audio_mem_init()
{
    umm_init_heap((void *)audio_heap, AUDIO_HEAP_SIZE);
}