#include <stdio.h>
#include <stdarg.h>
#include <pico.h>

#include "my_debug.h"

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DBG_LEVEL_DBG
#endif


uint8_t debug_level = DEBUG_LEVEL;


void my_debugf(const char *format, ...)
{
    char buf[256] = { 0 };
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    printf("%s", buf);
    fflush(stdout); 
}


void my_assert(const char *file, int line, const char *func, const char *pred)
{
    printf("assertion \"%s\" failed: file \"%s\", line %d, function: %s\n", pred, file, line, func);
    fflush(stdout);
    __asm volatile("cpsid i" : : : "memory"); // Disable global interrupts
    while (1) 
    {
        __asm("bkpt #0");
    };
}
