#ifndef SW_CONFIG_H
#define SW_CONFIG_H

#include <stdint.h>

#define SUPERLOOP_UPDATE_INTERVAL_MS    5
#define BACKLIGHT_IDLE_DIM_MS           10000

#define TICK_GRANULARITY_MS             1


// Maximun files listed per page.
#define CATALOG_PAGER_SIZE              32
// Remember # levels of history in file navigation
#define CATALOG_HISTORY_DEPTH           10

#define AUDIO_SAMPLE_RATE               48000
#define AUDIO_POWER_SAVER_MS            5000

extern int8_t backlight_brigntness_normal;
extern int8_t backlight_brignthess_dimmed;

#endif
