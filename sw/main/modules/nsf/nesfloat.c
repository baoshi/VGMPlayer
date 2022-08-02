#include "nesfloat.h"


// Convert Fixed point number to Int16 samples
int16_t nesfloat_to_sample(nesfloat_t x)
{
    int32_t t = (x >> (NESFLOAT_FRACTIONS - 16)) - 32768;
    int16_t s;
    if (t > 32767) s = 32767;
    else if (t < -32768) s = -32768;
    else s = t;
    return s;
}