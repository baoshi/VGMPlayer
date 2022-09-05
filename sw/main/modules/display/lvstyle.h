#pragma once
// Some commonly used LVGL styles

#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif

#define COLOR_BACKGROUND            lv_color_make(10, 10, 10)
#define COLOR_TEXT                  lv_color_make(225, 217, 210)
#define COLOR_DARK                  lv_color_make(44, 102, 132)
#define COLOR_LIGHT                 lv_color_make(61, 174, 233)

#define COLOR_BORDER                COLOR_LIGHT
#define COLOR_BUTTON_FACE_INACTIVE  COLOR_BACKGROUND
#define COLOR_BUTTON_FACE_FOCUSED   COLOR_DARK
#define COLOR_BUTTON_FACE_PRESSED   COLOR_LIGHT

#define CORNER_RADIUS               4


extern lv_style_t lvs_default;
extern lv_style_t lvs_scrollbar;
extern lv_style_t lvs_list;
extern lv_style_t lvs_list_btn, lvs_list_btn_focused, lvs_list_btn_pressed;
extern lv_style_t lvs_bar_bg, lvs_bar_ind;
extern lv_style_t lvs_slider_knob;
extern lv_style_t lvs_msgbox;

// Application specific styles
extern lv_style_t lvs_top_bar;
extern lv_style_t lvs_recolored_icon;
extern lv_style_t lvs_browser_file_list;
extern lv_style_t lvs_player_spectrum;

void lvs_init();

LV_IMG_DECLARE(img_browser_folder);
LV_IMG_DECLARE(img_browser_file);
LV_IMG_DECLARE(img_browser_pageup);
LV_IMG_DECLARE(img_browser_pagedown);
LV_IMG_DECLARE(img_popup_brightness);
LV_IMG_DECLARE(img_popup_speaker);
LV_IMG_DECLARE(img_popup_headphone);
LV_IMG_DECLARE(img_battery_0);
LV_IMG_DECLARE(img_battery_25);
LV_IMG_DECLARE(img_battery_50);
LV_IMG_DECLARE(img_battery_75);
LV_IMG_DECLARE(img_battery_100);
LV_IMG_DECLARE(img_battery_charging);
LV_IMG_DECLARE(img_battery_unknown);
LV_IMG_DECLARE(img_microsd);
LV_IMG_DECLARE(img_microsd_bad);
LV_IMG_DECLARE(img_microsd_empty);
LV_IMG_DECLARE(img_vgm_nes);

#ifdef __cplusplus
}
#endif
