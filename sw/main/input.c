#include <lvgl.h>
#include "ec.h"
#include "event_ids.h"
#include "event_queue.h"
#include "input.h"

//
// LVGL button device
// To use the 5 physical buttons as LVGL button device, we create 5 invisible buttons on the screen, and 
// associate the physical button to the coordinates where the invisible buttons are positioned. Invisible 
// buttons will send EVT_BUTTON_XXX_CLICKED when clicked.
// Buttons are disabled when created. Use input_enable_virtual_button to enable each button.
//

lv_indev_t *indev_button = NULL;
lv_obj_t *input_button_setting = NULL;
lv_obj_t *input_button_back = NULL;
lv_obj_t *input_button_play = NULL;
lv_obj_t *input_button_up = NULL;
lv_obj_t *input_button_down = NULL;

static lv_indev_drv_t _button_drv;
static uint8_t _button_mask;

// physical button to virtual (on-screen) button coordinate mapping
static lv_point_t _vbutton_coord[5] = 
{
    {0, 0}, // KEY_INDEX_NW
    {1, 0}, // KEY_INDEX_SW
    {2, 0}, // KEY_INDEX_PLAY
    {3, 0}, // KEY_INDEX_NE
    {4, 0}  // KEY_INDEX_NE
};
static lv_style_t _style_invisible_btn;
static bool _style_initialized = false;


static void _button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static uint32_t last_btn = 0;
    data->state = LV_INDEV_STATE_REL;    // Default state
    if (ec_keys & _button_mask & EC_KEY_MASK_NW)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_NW;
    }
    else if (ec_keys & _button_mask & EC_KEY_MASK_SW)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_SW;
    }
    else if (ec_keys & _button_mask & EC_KEY_MASK_PLAY)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_PLAY;
    }
    else if (ec_keys & _button_mask & EC_KEY_MASK_NE)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_NE;
    }
    else if (ec_keys & _button_mask & EC_KEY_MASK_SE)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_SE;
    }
    data->btn_id = last_btn;
}


static void button_init()
{
    _button_mask = 0;   // all buttons disabled
    lv_indev_drv_init(&_button_drv);
    _button_drv.type = LV_INDEV_TYPE_BUTTON;
    _button_drv.read_cb = _button_read;
    indev_button = lv_indev_drv_register(&_button_drv);
    lv_indev_set_button_points(indev_button, _vbutton_coord);
    
}


static void button_clicked_handler(lv_event_t *e)
{
    int event_id = (int)lv_event_get_user_data(e);
    EQ_QUICK_PUSH(event_id);
}


static void button_create_invisible_style()
{
     if (!_style_initialized)
    {
        lv_style_init(&_style_invisible_btn);
        lv_style_set_text_opa(&_style_invisible_btn, LV_OPA_TRANSP);
        lv_style_set_bg_opa(&_style_invisible_btn, LV_OPA_TRANSP);
        lv_style_set_border_width(&_style_invisible_btn, 0);
        lv_style_set_shadow_width(&_style_invisible_btn, 0);
        lv_style_set_outline_width(&_style_invisible_btn, 0);
        lv_style_set_outline_opa(&_style_invisible_btn, LV_OPA_TRANSP);
        _style_initialized = true;
    }
}


static lv_obj_t * button_create_virtual_button(lv_obj_t *screen, int btn_id, int event)
{
    lv_obj_t *btn;
    btn = lv_btn_create(screen);
    lv_obj_add_style(btn, &_style_invisible_btn, 0);
    lv_obj_add_style(btn, &_style_invisible_btn, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, _vbutton_coord[btn_id].x, _vbutton_coord[btn_id].y);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void*)event);
    lv_indev_enable(indev_button, false);
}


void input_enable_button_dev(bool enable)
{
    lv_indev_enable(indev_button, enable);
}


// Create invisible virtual buttons
void input_create_virtual_buttons(lv_obj_t *screen)
{
    button_create_invisible_style();
    input_button_setting = button_create_virtual_button(screen, INPUT_KEY_SETTING, EVT_BUTTON_SETTING_CLICKED);
    input_button_back = button_create_virtual_button(screen, INPUT_KEY_BACK, EVT_BUTTON_BACK_CLICKED);
    input_button_play = button_create_virtual_button(screen, INPUT_KEY_PLAY, EVT_BUTTON_PLAY_CLICKED);
    input_button_up = button_create_virtual_button(screen, INPUT_KEY_UP, EVT_BUTTON_UP_CLICKED);
    input_button_down = button_create_virtual_button(screen, INPUT_KEY_DOWN, EVT_BUTTON_DOWN_CLICKED);
}


void input_delete_virtual_buttons()
{
    lv_obj_del(input_button_down); input_button_down = NULL;
    lv_obj_del(input_button_up); input_button_up = NULL;
    lv_obj_del(input_button_play); input_button_play = NULL;
    lv_obj_del(input_button_back); input_button_back = NULL;
    lv_obj_del(input_button_setting); input_button_setting = NULL;
}


void input_enable_virtual_button(int id, bool enable)
{
    if ((id < 0) && (!enable))
    {
        _button_mask = 0;
    }
    else if ((id >= KEY_INDEX_NW) && (id <= KEY_INDEX_SE))
    {
        if (enable)
        {
            _button_mask |= (0x01 << id);
        }
        else
        {
            _button_mask &= ~(0x01 << id);
        }
    }
}


//
// LVGL Keypad device
//

lv_indev_t* indev_keypad = NULL;
lv_group_t* input_keypad_group = NULL;

static lv_indev_drv_t _keypad_drv;
static uint32_t _keypad_mapping[5] = {0, 0, 0, 0, 0};


static void _keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static uint32_t last_key = 0;
    data->state = LV_INDEV_STATE_REL;           // Default state
    if (ec_keys & EC_KEY_MASK_NW)         // 0x01
    {
        if (_keypad_mapping[0] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[0];
        }
    }
    else if (ec_keys & EC_KEY_MASK_SW)    // 0x02
    {
        if (_keypad_mapping[1] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[1];
        }
    }
    else if (ec_keys & EC_KEY_MASK_PLAY)  // 0x04
    {
        if (_keypad_mapping[2] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[2];
        }
    }
    else if (ec_keys & EC_KEY_MASK_NE)    // 0x08
    {
        if (_keypad_mapping[3] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[3];
        }
    }
    else if (ec_keys & EC_KEY_MASK_SE)    // 0x10
    {
        if (_keypad_mapping[4] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[4];
        }
    }
    data->key = last_key;
}


static void keypad_init()
{
    lv_indev_drv_init(&_keypad_drv);
    _keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    _keypad_drv.read_cb = _keypad_read;
    indev_keypad = lv_indev_drv_register(&_keypad_drv);
    input_keypad_group = lv_group_create();
    lv_group_set_wrap(input_keypad_group, false);
    lv_indev_set_group(indev_keypad, input_keypad_group);
    lv_indev_enable(indev_keypad, false);
}


void input_enable_keypad_dev(bool enable)
{
    if (!enable) lv_group_remove_all_objs(input_keypad_group);
    lv_indev_enable(indev_keypad, enable);
}


void input_map_keypad(int id, uint32_t keycode)
{
    if (id < 0)
    {
        _keypad_mapping[0] = 0;
        _keypad_mapping[1] = 0;
        _keypad_mapping[2] = 0;
        _keypad_mapping[3] = 0;
        _keypad_mapping[4] = 0;
    }
    else if ((id >= KEY_INDEX_NW) && (id <= KEY_INDEX_SE))
    {
        _keypad_mapping[id] = keycode;
    }
}


void input_init()
{
    button_init();
    keypad_init();
}


