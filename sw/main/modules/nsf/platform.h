#pragma once


#ifdef __GNUC__
# define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
# define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#if defined(WIN32)
# include<stdio.h>
#include <stdlib.h>
# define NSF_PRINTF(...) printf(__VA_ARGS__)
# define NSF_PRINTERR(...) fprintf(stderr, __VA_ARGS__)
# define NSF_MALLOC malloc
# define NSF_FREE free
#elif defined(linux)
# include<stdio.h>
#include <stdlib.h>
# define NSF_PRINTF(...) printf(__VA_ARGS__)
# define NSF_PRINTERR(...) fprintf(stderr, __VA_ARGS__)
# define NSF_MALLOC malloc
# define NSF_FREE free
#elif defined(__APPLE__)
# include<stdio.h>
#include <stdlib.h>
# define NSF_PRINTF(...) printf(__VA_ARGS__)
# define NSF_PRINTERR(...) fprintf(stderr, __VA_ARGS__)
# define NSF_MALLOC malloc
# define NSF_FREE free
#elif defined(PICO_BOARD)
#include "my_debug.h"
#include "my_mem.h"
# define NSF_PRINTF(...) MY_LOGD(__VA_ARGS__)
# define NSF_PRINTERR(...) MY_LOGE(__VA_ARGS__)
# define NSF_MALLOC MY_MALLOC
# define NSF_FREE MY_FREE
#endif


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif