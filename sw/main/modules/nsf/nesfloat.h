#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// All NES APU mathematics are dealing with float number within range 0.0-1.0
// We use 3:29 fixed point maths

typedef int32_t nesfloat_t;

// 000.0 0000 0000 0000 0000 0000 0000 0000
#define NESFLOAT_FRACTIONS 29

#define float_to_nesfloat(x) ((nesfloat_t)((x) * (float)(1 << NESFLOAT_FRACTIONS)))
#define nesfloat_to_float(x) ((x) / (float)(1 << NESFLOAT_FRACTIONS))
#define nesfloat_mul(x, y) (((x)>>15)*((y)>>14))


int16_t nesfloat_to_sample(nesfloat_t x);


#ifdef __cplusplus
}
#endif