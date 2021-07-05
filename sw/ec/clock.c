#include <xc.h>

#include "clock.h"

void clock_config(void)
{
    // IRCF[3:0] = 1011, 1MHz
    OSCCON = 0b01011000; // [x IRCF<3:0> x SCS<1:0>]
    // Wait for HFINTOSC to stable
    while (OSCSTATbits.HFIOFS != 1) {};
}
