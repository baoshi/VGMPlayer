#include "ec.h"
#include "lvinput.h"


lv_indev_t* lvi_button;
lv_indev_t* lvi_keypad;
lv_group_t* lvi_keypad_group;

static lv_indev_drv_t _button_drv;
static uint8_t _button_mask;
static lv_point_t _button_coord[4] = 
{
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0}
};
static lv_indev_drv_t _keypad_drv;
static uint32_t _keypad_mapping[4] = {0, 0, 0, 0};


static void _keypad_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    static uint32_t last_key = 0;
    data->state = LV_INDEV_STATE_REL;   // Default state
    int8_t button = ec_read_buttons();
    // translate key to LV_KEY_XXX
    switch (button) 
    {
        case EC_BUTTON_MODE:
            if (_keypad_mapping[0] != 0)
            {
                data->state = LV_INDEV_STATE_PR;
                last_key = _keypad_mapping[3];
            }
            break;
        case EC_BUTTON_PLAY:
            if (_keypad_mapping[1] != 0)
            {
                data->state = LV_INDEV_STATE_PR;
                last_key = _keypad_mapping[1];
            }
            break;
        case EC_BUTTON_UP:
            if (_keypad_mapping[2] != 0)
            {
                data->state = LV_INDEV_STATE_PR;
                last_key = _keypad_mapping[2];
            }
            break;
        case EC_BUTTON_DOWN:
            if (_keypad_mapping[3] != 0)
            {
                data->state = LV_INDEV_STATE_PR;
                last_key = _keypad_mapping[3];
            }
            break;
        default:
            break;
    }
    data->key = last_key;
}


static void _button_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    static uint32_t last_btn = 0;
    data->state = LV_INDEV_STATE_REL;    // Default state
    int8_t button = ec_read_buttons();
    switch (button)
    {
        case EC_BUTTON_MODE:
            if (_button_mask & LVI_BUTTON_MODE)
            {
                data->state = LV_INDEV_STATE_PR;
                last_btn = 0;
            }
            break;
        case EC_BUTTON_PLAY:
            if (_button_mask & LVI_BUTTON_PLAY)
            {
                data->state = LV_INDEV_STATE_PR;
                last_btn = 1;
            }
            break;
        case EC_BUTTON_UP:
            if (_button_mask & LVI_BUTTON_UP)
            {
                data->state = LV_INDEV_STATE_PR;
                last_btn = 2;
            }
            break;
        case EC_BUTTON_DOWN:
            if (_button_mask & LVI_BUTTON_DOWN)
            {
                data->state = LV_INDEV_STATE_PR;
                last_btn = 3;
            }
            break;
        default:
            break;
    }
    data->btn_id = last_btn;
}


void lvi_init()
{
    // Keypad
    lv_indev_drv_init(&_keypad_drv);
    _keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    _keypad_drv.read_cb = _keypad_read;
    lvi_keypad = lv_indev_drv_register(&_keypad_drv);
    lvi_keypad_group = lv_group_create();
    lv_group_set_wrap(lvi_keypad_group, false);
    lv_indev_set_group(lvi_keypad, lvi_keypad_group);
    // Button
    _button_mask = 0;
    lv_indev_drv_init(&_button_drv);
    _button_drv.type = LV_INDEV_TYPE_BUTTON;
    _button_drv.read_cb = _button_read;
    lvi_button = lv_indev_drv_register(&_button_drv);
    lv_indev_set_button_points(lvi_button, _button_coord);
}


void lvi_map_keypad(uint8_t button, uint32_t lvkey)
{
    switch (button)
    {
    case LVI_BUTTON_MODE:
        _keypad_mapping[0] = lvkey;
        break;
    case LVI_BUTTON_PLAY:
        _keypad_mapping[1] = lvkey;
        break;
    case LVI_BUTTON_UP:
        _keypad_mapping[2] = lvkey;
        break;
    case LVI_BUTTON_DOWN:
        _keypad_mapping[3] = lvkey;
        break;
    default:
        break;
    }
}


void lvi_pos_button(uint8_t button, lv_coord_t x, lv_coord_t y)
{
    switch (button)
    {
    case LVI_BUTTON_MODE:   // 0
        _button_coord[0].x = x;
        _button_coord[0].y = y;
        _button_mask |= LVI_BUTTON_MODE;
        break;
    case LVI_BUTTON_PLAY:   // 1
        _button_coord[1].x = x;
        _button_coord[1].y = y;
        _button_mask |= LVI_BUTTON_PLAY;
        break;
    case LVI_BUTTON_UP:     // 2
        _button_coord[2].x = x;
        _button_coord[2].y = y;
        _button_mask |= LVI_BUTTON_UP;
        break;
    case LVI_BUTTON_DOWN:   // 3
        _button_coord[3].x = x;
        _button_coord[3].y = y;
        _button_mask |= LVI_BUTTON_DOWN;
        break;
    default:
        break;
    }
}


void lvi_disable_button()
{
    _button_mask = 0;
}


void lvi_disable_keypad()
{
    _keypad_mapping[0] = 0;
    _keypad_mapping[1] = 0;
    _keypad_mapping[2] = 0;
    _keypad_mapping[3] = 0;
}


void lvi_clear_keypad_group()
{
    lv_group_remove_all_objs(lvi_keypad_group);
}