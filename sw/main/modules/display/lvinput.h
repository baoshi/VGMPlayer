#pragma once

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif


extern lv_indev_t* lvi_button;
extern lv_indev_t* lvi_keypad;
extern lv_group_t* lvi_keypad_group;


#define LVI_BUTTON_NW   0
#define LVI_BUTTON_SW   1
#define LVI_BUTTON_PLAY 2
#define LVI_BUTTON_NE   3
#define LVI_BUTTON_SE   4


void lvi_init();

void lvi_disable_button();
void lvi_enable_button();
void lvi_disable_keypad();
void lvi_enable_keypad();
void lvi_pos_button(uint8_t button, lv_coord_t x, lv_coord_t y);
void lvi_map_keypad(uint8_t button, uint32_t lvkey);

#ifdef __cplusplus
}
#endif