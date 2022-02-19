#pragma once

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif


extern lv_indev_t* lvi_button;
extern lv_indev_t* lvi_keypad;
extern lv_group_t* lvi_keypad_group;

#define LVI_BUTTON_MODE 0x01
#define LVI_BUTTON_PLAY 0x02
#define LVI_BUTTON_UP   0x04
#define LVI_BUTTON_DOWN 0x08


void lvi_init();

void lvi_disable_button();
void lvi_disable_keypad();
void lvi_clear_keypad_group();
void lvi_pos_button(uint8_t button, lv_coord_t x, lv_coord_t y);
void lvi_map_keypad(uint8_t button, uint32_t lvkey);

#ifdef __cplusplus
}
#endif