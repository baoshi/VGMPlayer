#include "ec.h"
#include "lvinput.h"


lv_indev_t* lvi_button;
lv_indev_t* lvi_keypad;
lv_group_t* lvi_keypad_group;

static lv_indev_drv_t _button_drv;
static uint8_t _button_mask;
static lv_point_t _button_coord[5] = 
{
    {0, 0}, // EC_BUTTON_MASK_NW
    {1, 0}, // EC_BUTTON_MASK_SW
    {2, 0}, // EC_BUTTON_MASK_PLAY
    {3, 0}, // EC_BUTTON_MASK_NE
    {4, 0}  // EC_BUTTON_MASK_SE
};
static lv_indev_drv_t _keypad_drv;
static uint32_t _keypad_mapping[5] = {0, 0, 0, 0, 0};


static void _keypad_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    static uint32_t last_key = 0;
    data->state = LV_INDEV_STATE_REL;           // Default state
    if (ec_buttons & EC_BUTTON_MASK_NW)
    {
        if (_keypad_mapping[0] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[0];
        }
    }
    else if (ec_buttons & EC_BUTTON_MASK_SW)
    {
        if (_keypad_mapping[1] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[1];
        }
    }
    else if (ec_buttons & EC_BUTTON_MASK_PLAY)
    {
        if (_keypad_mapping[2] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[2];
        }
    }
    else if (ec_buttons & EC_BUTTON_MASK_NE)
    {
        if (_keypad_mapping[3] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[3];
        }
    }
    else if (ec_buttons & EC_BUTTON_MASK_SE)    // 5
    {
        if (_keypad_mapping[4] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _keypad_mapping[4];
        }
    }
    data->key = last_key;
}


static void _button_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    static uint32_t last_btn = 0;
    data->state = LV_INDEV_STATE_REL;    // Default state
    if ((ec_buttons & EC_BUTTON_MASK_NW) && (_button_mask & 0x01))
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = LVI_BUTTON_NW;
    }
    else if ((ec_buttons & EC_BUTTON_MASK_SW) && (_button_mask & 0x02))
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = LVI_BUTTON_SW;
    }
    else if ((ec_buttons & EC_BUTTON_MASK_PLAY) && (_button_mask & 0x04))
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = LVI_BUTTON_PLAY;
    }
    else if ((ec_buttons & EC_BUTTON_MASK_NE) && (_button_mask & 0x08))
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = LVI_BUTTON_NE;
    }
    else if ((ec_buttons & EC_BUTTON_MASK_SE) && (_button_mask & 0x10))
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = LVI_BUTTON_SE;
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
    case LVI_BUTTON_NW:
        _keypad_mapping[0] = lvkey;
        break;
    case LVI_BUTTON_SW:
        _keypad_mapping[1] = lvkey;
        break;
    case LVI_BUTTON_PLAY:
        _keypad_mapping[2] = lvkey;
        break;
    case LVI_BUTTON_NE:
        _keypad_mapping[3] = lvkey;
        break;
    case LVI_BUTTON_SE:
        _keypad_mapping[4] = lvkey;
        break;        
    default:
        break;
    }
}


void lvi_pos_button(uint8_t button, lv_coord_t x, lv_coord_t y)
{
    switch (button)
    {
    case LVI_BUTTON_NW:     // 0
        _button_coord[0].x = x;
        _button_coord[0].y = y;
        _button_mask |= 0x01;
        break;
    case LVI_BUTTON_SW:     // 2
        _button_coord[1].x = x;
        _button_coord[1].y = y;
        _button_mask |= 0x02;
        break;
    case LVI_BUTTON_PLAY:   // 3
        _button_coord[2].x = x;
        _button_coord[2].y = y;
        _button_mask |= 0x04;
        break;
    case LVI_BUTTON_NE:     // 4
        _button_coord[3].x = x;
        _button_coord[3].y = y;
        _button_mask |= 0x08;
        break;
    case LVI_BUTTON_SE:     // 5
        _button_coord[3].x = x;
        _button_coord[3].y = y;
        _button_mask |= 0x10;
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
    _keypad_mapping[4] = 0;
}


void lvi_clear_keypad_group()
{
    lv_group_remove_all_objs(lvi_keypad_group);
}
