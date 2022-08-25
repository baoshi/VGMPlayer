#ifndef SW_CONFIG_H
#define SW_CONFIG_H

#include <stdint.h>
#include "eeprom.h"

#define SUPERLOOP_UPDATE_INTERVAL_MS    40
#define BACKLIGHT_IDLE_DIM_MS           10000

#define TICK_GRANULARITY_MS             10


// Maximun files listed per page.
#define CATALOG_PAGER_SIZE              32
// Remember # levels of history in file navigation
#define CATALOG_HISTORY_DEPTH           10

#define POPUP_AUTO_CLOSE_MS             5000

#define VGM_CACHE_SIZE                  2048


union nvram_t
{
    struct 
    {
        uint8_t ver;
        int8_t backlight_brigntness_normal;
        int8_t backlight_brignthess_dimmed;
        int8_t volume_speaker;
        int8_t volume_headphone;    
    };
    uint8_t data[EEPROM_SIZE];
};

extern union nvram_t config;

void config_load();
void config_save();
void config_set_dirty();
bool config_is_dirty();

#endif
