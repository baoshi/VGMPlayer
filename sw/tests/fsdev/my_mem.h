#ifndef MY_MEM_H
#define MY_MEM_H

#include <stdlib.h>
#include <umm_malloc_cfg.h>
#include <umm_malloc.h>


#ifndef DBG_MEM
#  define DBG_MEM 0
#endif


#if (DBG_MEM)
#  define MY_MEM_INIT umm_init
#  define MY_MALLOC umm_malloc
#  define MY_CALLOC umm_calloc
#  define MY_REALLOC umm_realloc
#  define MY_FREE umm_free
#else
#  define MY_MEM_INIT()
#  define MY_MALLOC malloc
#  define MY_CALLOC calloc
#  define MY_REALLOC realloc
#  define MY_FREE free
#endif


#endif