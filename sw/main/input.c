#include "my_mem.h"
#include "sw_conf.h"
#include "ec.h"
#include "event_ids.h"
#include "event_queue.h"
#include "input.h"

//
// LVGL button device
// To use the 5 physical buttons as LVGL button device, we create 5 invisible buttons on the screen, and 
// associate the physical button to the coordinates where the invisible buttons are positioned.
// Buttons are disabled when created. To enable and use a button, do:
// 1. Map an application event to the LVGL button event. e.g.
//    input_map_button(INPUT_KEY_SETTING, LV_EVENT_CLICKED, EVT_BUTTON_SETTING_CLICKED);
// 2. Enable button
//    input_enable_button(INPUT_KEY_SETTING);
// 3. Enable button device
//    input_enable_button_dev();
// Then EVT_BUTTON_SETTING_CLICKED will be posted to the application event queue when SETTING key is clicked.
// input_map_button(INPUT_KEY_SETTING, 0, 0) removes all events for the button.
//


//
// LVGL Button device
//

lv_indev_t *indev_button = NULL;
lv_obj_t *input_button_setting = NULL;
lv_obj_t *input_button_back = NULL;
lv_obj_t *input_button_play = NULL;
lv_obj_t *input_button_up = NULL;
lv_obj_t *input_button_down = NULL;

static lv_indev_drv_t _button_drv;

// physical button to virtual (on-screen) button coordinate mapping
static lv_point_t _vbutton_coord[INPUT_KEYS] = 
{
    {0, 0}, // KEY_INDEX_NW
    {1, 0}, // KEY_INDEX_SW
    {2, 0}, // KEY_INDEX_PLAY
    {3, 0}, // KEY_INDEX_NE
    {4, 0}  // KEY_INDEX_NE
};
static lv_style_t _style_invisible_btn;
static bool _style_initialized = false;

/* Mapping of button events <-> application events
 * LVGL button messages are (verify with lv_evnet.h if upgrade)
 * LV_EVENT_PRESSED = 1
 * LV_EVENT_PRESSING = 2
 * LV_EVENT_PRESS_LOST = 3
 * LV_EVENT_SHORT_CLICKED = 4
 * LV_EVENT_LONG_PRESSED = 5
 * LV_EVENT_LONG_PRESSED_REPEAT = 6
 * LV_EVENT_CLICKED = 7
 * LV_EVENT_RELEASED = 8
 */
#define LV_BUTTON_EVENT_MIN LV_EVENT_PRESSED
#define LV_BUTTON_EVENT_MAX LV_EVENT_RELEASED


//
// LVGL Keypad device
//

lv_indev_t* indev_keypad = NULL;
static lv_indev_drv_t _keypad_drv;

// Input Config
typedef struct input_config_s
{
    int button_event_table[INPUT_KEYS][LV_BUTTON_EVENT_MAX - LV_BUTTON_EVENT_MIN + 1];
    uint32_t keypad_code_table[INPUT_KEYS];
    bool button_enabled;
    bool keypad_enabled;
    lv_group_t *input_group;
} input_config_t;

static input_config_t _config = { 0 };


static void button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static uint32_t last_btn = 0;
    data->state = LV_INDEV_STATE_REL;    // Default state
    if (ec_keys & EC_KEY_MASK_NW)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_NW;
    }
    else if (ec_keys & EC_KEY_MASK_SW)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_SW;
    }
    else if (ec_keys & EC_KEY_MASK_PLAY)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_PLAY;
    }
    else if (ec_keys & EC_KEY_MASK_NE)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_NE;
    }
    else if (ec_keys & EC_KEY_MASK_SE)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = KEY_INDEX_SE;
    }
    data->btn_id = last_btn;
}


static void button_init()
{
    lv_indev_drv_init(&_button_drv);
    _button_drv.type = LV_INDEV_TYPE_BUTTON;
    _button_drv.long_press_time = BUTTON_LONGPRESS_TIME_MS;
    _button_drv.read_cb = button_read;
    indev_button = lv_indev_drv_register(&_button_drv);
    lv_indev_set_button_points(indev_button, _vbutton_coord);
    lv_indev_enable(indev_button, false);
}


static void button_event_handler(lv_event_t *e)
{
    int btn_id = (int)lv_event_get_user_data(e);
    int lv_event = (int)lv_event_get_code(e);
    if ((lv_event >= LV_BUTTON_EVENT_MIN) && (lv_event <= LV_BUTTON_EVENT_MAX))
    {
        int app_event = _config.button_event_table[btn_id][lv_event - LV_BUTTON_EVENT_MIN];
        if (app_event != 0) EQ_QUICK_PUSH(app_event);
    }
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


static lv_obj_t * button_create_virtual_button(lv_obj_t *screen, int btn_id)
{
    lv_obj_t *btn;
    btn = lv_btn_create(screen);
    lv_obj_add_style(btn, &_style_invisible_btn, 0);
    lv_obj_add_style(btn, &_style_invisible_btn, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, _vbutton_coord[btn_id].x, _vbutton_coord[btn_id].y);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_event_handler, LV_EVENT_ALL, (void *)btn_id);
    return btn;
}


void input_enable_button_dev()
{
    lv_indev_enable(indev_button, true);
    _config.button_enabled = true;
}


void input_disable_button_dev()
{
    lv_indev_enable(indev_button, false);
    _config.button_enabled = false;
}


// Create invisible virtual buttons
void input_create_buttons(lv_obj_t *screen)
{
    button_create_invisible_style();
    input_button_setting = button_create_virtual_button(screen, INPUT_KEY_SETTING);
    input_button_back = button_create_virtual_button(screen, INPUT_KEY_BACK);
    input_button_play = button_create_virtual_button(screen, INPUT_KEY_PLAY);
    input_button_up = button_create_virtual_button(screen, INPUT_KEY_UP);
    input_button_down = button_create_virtual_button(screen, INPUT_KEY_DOWN);
}


void input_delete_buttons()
{
    lv_obj_del(input_button_down); input_button_down = NULL;
    lv_obj_del(input_button_up); input_button_up = NULL;
    lv_obj_del(input_button_play); input_button_play = NULL;
    lv_obj_del(input_button_back); input_button_back = NULL;
    lv_obj_del(input_button_setting); input_button_setting = NULL;
}


/**
 * @brief Map LVGL button event to application event for a given button
 * 
 * @param id        Key ID (INPUT_KEY_XXX), -1 to disable all buttons
 * @param lv_event  LVGL button event [LV_BUTTON_EVENT_MIN .. LV_BUTTON_EVENT_MAX], 0 to disable all events
 * @param app_event Application event to be posted to the event queue, 0 to disable this event.
 */
void input_map_button(int id, int lv_event, int app_event)
{
    if (id < 0)
    {
        memset(_config.button_event_table, 0, sizeof(_config.button_event_table));
    }
    else if (0 == lv_event)
    {
        for (int i = 0; i <= LV_BUTTON_EVENT_MAX - LV_BUTTON_EVENT_MAX; ++i)
        {
            _config.button_event_table[id][i - LV_BUTTON_EVENT_MIN] = 0;
        }
    }
    else if ((lv_event >= LV_BUTTON_EVENT_MIN) && (lv_event <= LV_BUTTON_EVENT_MAX))
    {
        _config.button_event_table[id][lv_event - LV_BUTTON_EVENT_MIN] = app_event;
    }
}


static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static uint32_t last_key = 0;
    data->state = LV_INDEV_STATE_REL;           // Default state
    if (ec_keys & EC_KEY_MASK_NW)         // 0x01
    {
        if (_config.keypad_code_table[0] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _config.keypad_code_table[0];
        }
    }
    else if (ec_keys & EC_KEY_MASK_SW)    // 0x02
    {
        if (_config.keypad_code_table[1] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _config.keypad_code_table[1];
        }
    }
    else if (ec_keys & EC_KEY_MASK_PLAY)  // 0x04
    {
        if (_config.keypad_code_table[2] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _config.keypad_code_table[2];
        }
    }
    else if (ec_keys & EC_KEY_MASK_NE)    // 0x08
    {
        if (_config.keypad_code_table[3] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _config.keypad_code_table[3];
        }
    }
    else if (ec_keys & EC_KEY_MASK_SE)    // 0x10
    {
        if (_config.keypad_code_table[4] != 0)
        {
            data->state = LV_INDEV_STATE_PR;
            last_key = _config.keypad_code_table[4];
        }
    }
    data->key = last_key;
}


static void keypad_init()
{
    lv_indev_drv_init(&_keypad_drv);
    _keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    _keypad_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&_keypad_drv);
    lv_indev_enable(indev_keypad, false);
}


void input_enable_keypad_dev()
{
    lv_indev_enable(indev_keypad, true);
    _config.keypad_enabled = true;
}


void input_disable_keypad_dev()
{
    lv_indev_set_group(indev_keypad, NULL);
    lv_indev_enable(indev_keypad, false);
    _config.keypad_enabled = false;
}


/**
 * @brief Map input key as LVGL keypad code
 * 
 * @param id        Key ID (INPUT_KEY_XXX), -1 to disable all keypad
 * @param keycode   LVGL keycode, LV_KEY_XXX or character
  */
void input_map_keypad(int id, uint32_t keycode)
{
    if (id < 0)
    {
        for (int i = 0; i < INPUT_KEYS; ++i)
            _config.keypad_code_table[i] = 0;
    }
    else if ((id >= KEY_INDEX_MIN) && (id <= KEY_INDEX_MAX))
    {
        _config.keypad_code_table[id] = keycode;
    }
}


void input_init()
{
    memset(&_config, 0, sizeof(input_config_t));
    button_init();
    keypad_init();
}


void *input_save()
{
    input_config_t *mem = MY_MALLOC(sizeof(input_config_t));
    if (mem)
    {
        memcpy(mem, &_config, sizeof(input_config_t));
        if (_config.keypad_enabled)
        {
            mem->input_group = indev_keypad->group;
        }
        else
        {
            mem->input_group = NULL;
        }
    }
    return (void *)mem;
}


void input_load(void *config)
{
    if (config)
    {
        input_disable_button_dev();
        input_disable_keypad_dev();
        memcpy(&_config, (input_config_t *)config, sizeof(input_config_t));
        if (_config.button_enabled) input_enable_button_dev();
        if (_config.keypad_enabled) 
        {
            if (_config.input_group)
                lv_indev_set_group(indev_keypad, _config.input_group);
            input_enable_keypad_dev();
        }
        MY_FREE(config);
    }
}
