#pragma once
// Some commonly used LVGL styles

#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif

#define COLOR_BACKGROUND            lv_color_make(0, 0, 0)
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
extern lv_style_t lvs_player_progress_bar_bg;
extern lv_style_t lvs_player_progress_bar_ind;

void lvs_init();

#ifdef __cplusplus
}
#endif
