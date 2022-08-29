#pragma once

#include <stdint.h>

#ifndef q15_t
#define q15_t int16_t
#endif

#ifndef q31_t
#define q31_t int32_t
#endif

#ifndef float32_t
#define float32_t float
#endif

#ifdef _MSC_VER
# define ALIGN4 
#else
# ifndef ALIGN4
#  define ALIGN4 __attribute__((aligned(4))) ///< align to 4 bytes
# endif
#endif


// "Workarea" for FFT, must be at least double the FFT points,
// i.e., FFT_WORKAREA = 4096 then we can do FFT on 2048 points
#define FFT_WORKAREA 4096

int fft_q15(q15_t* source, uint16_t length);
