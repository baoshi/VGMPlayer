#pragma once
// Some commonly used LVGL styles

#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif


#define COLOR_BACKGROUND            lv_color_make(0, 0, 0)
#define COLOR_TEXT                  lv_color_make(225, 217, 210)
#define COLOR_BORDER                lv_color_make(60, 169, 227)
#define COLOR_BUTTON_FACE_INACTIVE  lv_color_make(0, 0, 0)
#define COLOR_BUTTON_TEXT_INACTIVE  lv_color_make(225, 217, 210)
#define COLOR_BUTTON_FACE_FOCUSED   lv_color_make(44, 102, 132)
#define COLOR_BUTTON_TEXT_FOCUSED   lv_color_make(225, 217, 210)
#define COLOR_BUTTON_FACE_PRESSED   lv_color_make(61, 174, 233)
#define COLOR_BUTTON_TEXT_PRESSED   lv_color_make(225, 217, 210)

#define CORNER_RADIUS               4



extern lv_style_t lvs_recolored_icon;

extern lv_style_t lvs_default;
extern lv_style_t lvs_scrollbar;
extern lv_style_t lvs_list;
extern lv_style_t lvs_list_btn, lvs_list_btn_focused, lvs_list_btn_pressed;
extern lv_style_t lvs_msgbox;

// Application specific styles
extern lv_style_t lvs_browser_file_list;
extern lv_style_t lvs_invisible_button;

void lvs_init();

#ifdef __cplusplus
}
#endif
