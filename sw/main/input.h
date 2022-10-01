#pragma once

#include <lvgl.h>

// Button index. Note these are also the bit index to ec_key
#define KEY_INDEX_MIN   0
#define KEY_INDEX_NW    0
#define KEY_INDEX_SW    1
#define KEY_INDEX_PLAY  2
#define KEY_INDEX_NE    3
#define KEY_INDEX_SE    4
#define KEY_INDEX_MAX   4

// Function to button index mapping
#define INPUT_KEY_SETTING   KEY_INDEX_SW
#define INPUT_KEY_BACK      KEY_INDEX_NW
#define INPUT_KEY_PLAY      KEY_INDEX_PLAY
#define INPUT_KEY_UP        KEY_INDEX_NE
#define INPUT_KEY_DOWN      KEY_INDEX_SE
#define INPUT_KEYS          5

extern lv_indev_t *indev_button;
extern lv_obj_t *input_button_setting;
extern lv_obj_t *input_button_back;
extern lv_obj_t *input_button_play;
extern lv_obj_t *input_button_up;
extern lv_obj_t *input_button_down;
void input_enable_button_dev();
void input_disable_button_dev();
void input_create_buttons(lv_obj_t *screen);
void input_delete_buttons();
void input_map_button(int id, int lv_event, int app_event);

extern lv_indev_t* indev_keypad;
void input_enable_keypad_dev();
void input_disable_keypad_dev();
void input_map_keypad(int id, uint32_t keycode);

void input_init();
void * input_save();
void input_load(void *config);