#pragma once

#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif

lv_theme_t * lvt_init(lv_disp_t * disp);

LV_IMG_DECLARE(browser_icon_folder);
LV_IMG_DECLARE(browser_icon_file);
LV_IMG_DECLARE(browser_icon_nsf);
LV_IMG_DECLARE(settings_icon_brightness);

#ifdef __cplusplus
} /*extern "C"*/
#endif

