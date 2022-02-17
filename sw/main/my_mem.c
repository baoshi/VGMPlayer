#include "my_mem.h"


void my_mem_init()
{
#if DBG_MEM
    umm_init();
#endif
}


#if DBG_MEM
/* Start addresses and the size of the heap */
#define HEAP_BLOCKS (0x2000)
uint32_t UMM_MALLOC_CFG_HEAP_SIZE =  (HEAP_BLOCKS * UMM_BLOCK_BODY_SIZE);
static char umm_heap[HEAP_BLOCKS][UMM_BLOCK_BODY_SIZE];
void *UMM_MALLOC_CFG_HEAP_ADDR = &umm_heap;
#endif
