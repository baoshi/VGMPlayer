#include <pico/time.h>
#include <lvgl.h>
#include "st7789.h"
#include "lvinput.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "display.h"


void display_tick_hook(void* p)
{
    lv_tick_inc((uint32_t)p);
}


void display_init()
{
    lv_init();
    st7789_lvgl_init();
    lv_disp_t* disp = lv_disp_get_default();
    lv_disp_set_theme(disp, lvt_init(disp));
    lvi_init(); // LVGL Input Device
    lvs_init(); // LVGL Style
}