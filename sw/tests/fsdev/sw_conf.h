#ifndef SW_CONFIG_H
#define SW_CONFIG_H

#include <stdint.h>

#define SUPERLOOP_UPDATE_INTERVAL_MS    5
#define BACKLIGHT_IDLE_DIM_MS           10000

#define TICK_GRANULARITY_MS         1


// Maximun files list per screen. Set this to conserve memory
// Set to -1 to disable limit
#define BROWSER_MAX_LIST_FILES      64

#define AUDIO_SAMPLE_RATE           48000
#define AUDIO_POWER_SAVER_MS        5000

extern int8_t backlight_brigntness_normal;
extern int8_t backlight_brignthess_dimmed;

#endif
