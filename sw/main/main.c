#include <stdio.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <hardware/clocks.h>
#include "sw_conf.h"
#include "hw_conf.h"
#include "my_debug.h"
#include "my_mem.h"



int main()
{
    // Calculated using vcocalc.py, set sys clock to 120MHz
    set_sys_clock_pll(1440 * MHZ, 6, 2);

    stdio_init_all();
    printf("\033[2J\033[H");  // Clear terminal
    stdio_flush();

    my_mem_init();

    hw_init();

    for (;;)
    {
    }

    return 0;
}