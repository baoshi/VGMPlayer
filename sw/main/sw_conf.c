#include "sw_conf.h"


int8_t backlight_brigntness_normal = 90;
int8_t backlight_brignthess_dimmed = 20;


#define CONFIG_VER 100

union nvram_t config;


static bool config_dirty;

static void config_load_default()
{
    config.ver = CONFIG_VER;
    config.backlight_brigntness_normal = 90;
    config.backlight_brignthess_dimmed = 20;
    config.volume_headphone = 20;
    config.volume_speaker = 50;
}


void config_load()
{
    config_dirty = false;
    if (EEPROM_NOT_INITIALIZED == eeprom_read(config.data))
    {
        config_load_default();
        return;
    }
    if (CONFIG_VER != config.ver)
    {
        config_load_default();
        return;
    }
}


void config_save()
{
    eeprom_write(config.data);
    config_dirty = false;
}


void config_set_dirty()
{
    config_dirty = true;
}


bool config_is_dirty()
{
    return config_dirty;
}


const char * const config_supported_file_exts[] =
{
    "vgm",
    "s16",
    "\0"
};