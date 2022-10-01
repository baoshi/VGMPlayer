

#include <xc.h>
#include <stdint.h>
#include "state.h"

// OFF State
// Entry:
//   RESET
// Exit:
//   None
// Update:
//   Nothing
//   

static uint8_t _timestamp;

void state_off_entry(void)
{
    RESET();
}


uint8_t state_off_update(void)
{
    return MAIN_STATE_OFF;
}

