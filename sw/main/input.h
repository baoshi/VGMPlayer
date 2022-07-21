#pragma once

// Button index. Note these are also the bit index to ec_key
#define KEY_INDEX_NW     0
#define KEY_INDEX_SW     1
#define KEY_INDEX_PLAY   2
#define KEY_INDEX_NE     3
#define KEY_INDEX_SE     4

// Function to button index mapping
#define INPUT_KEY_SETTING    KEY_INDEX_NW
#define INPUT_KEY_BACK       KEY_INDEX_SW
#define INPUT_KEY_PLAY       KEY_INDEX_PLAY
#define INPUT_KEY_UP         KEY_INDEX_NE
#define INPUT_KEY_DOWN       KEY_INDEX_SE


extern lv_indev_t *indev_button;
extern lv_obj_t *input_button_setting;
extern lv_obj_t *input_button_back;
extern lv_obj_t *input_button_play;
extern lv_obj_t *input_button_up;
extern lv_obj_t *input_button_down;

void input_enable_button_dev(bool enable);
void input_create_virtual_buttons(lv_obj_t *screen);
void input_delete_virtual_buttons();
void input_enable_virtual_button(int id, bool enable);
void input_restore_default_virtual_button_event(int id);
void input_add_virtual_button_event(int id, lv_event_code_t filter, int event);


extern lv_indev_t* indev_keypad;
extern lv_group_t* input_keypad_group;
void input_enable_keypad_dev(bool enable);
void input_map_keypad(int id, uint32_t keycode);

void input_init();