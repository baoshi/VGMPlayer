#ifndef MY_MEM_H
#define MY_MEM_H

#include <stdlib.h>
#include <umm_malloc_cfg.h>
#include <umm_malloc.h>


#ifndef DBG_MEM
#  define DBG_MEM 0
#endif


#if (DBG_MEM)
#  define my_mem_init umm_init
#  define my_malloc umm_malloc
#  define my_calloc umm_calloc
#  define my_realloc umm_realloc
#  define my_free umm_free
#else
#  define my_mem_init()
#  define my_malloc malloc
#  define my_calloc calloc
#  define my_realloc realloc
#  define my_free free
#endif


#endif