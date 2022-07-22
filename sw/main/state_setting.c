#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "lvsupp.h"
#include "event_ids.h"
#include "event_queue.h"
#include "input.h"
#include "backlight.h"
#include "app.h"


// State "setting"
// State "setting" contains 2 sub-states: "setting-volume", "setting-brightness", 
// State "setting" is not initialzed at program start. Caller use setting_create() to append it to the current state
// State "setting" does not transit to any other states except when exit, it transits to the creator's state and emit EVT_SETTING_CLOSED event


#ifndef SETTING_DEBUG
# define SETTING_DEBUG 1
#endif


// Debug log
#if SETTING_DEBUG
#define ST_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define ST_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define ST_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define ST_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define ST_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define ST_LOGD(x, ...)
#define ST_LOGI(x, ...)
#define ST_LOGW(x, ...)
#define ST_LOGE(x, ...)
#define ST_DEBUGF(x, ...)
#endif


// When called, append "setting" sub-state to the current state
void setting_create(app_t *me)
{
    state_t *curr = ((hsm_t *)me)->curr;
    setting_t *ctx = &(me->setting_ctx);
    ctx->creator = curr;  // save 
    ST_LOGD("Setting: creation, append to %s state\n", ctx->creator->name);
    state_ctor(&(me->setting), "setting", curr, (event_handler_t)setting_handler);
    STATE_TRAN((hsm_t *)me, &me->setting);
}


event_t const *setting_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
        EVT_EXIT:
        EVT_START:
            Start setting_volume
        EVT_BUTTON_BACK_CLICKED:
            Transit to creator state and send EVT_SETTING_CLOSED
    */
    setting_t *ctx = &(me->setting_ctx);
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
            ST_LOGD("Setting: entry\n");
            break;
        case EVT_EXIT:
            ST_LOGD("Setting: exit\n");
            
            break;
        case EVT_START:
            ST_LOGD("Setting: start\n");
            STATE_START(me, &(me->setting_volume));
            break;
        case EVT_BUTTON_BACK_CLICKED:
            ST_LOGD("Setting: close and transit to %s state\n", ctx->creator->name);
            STATE_TRAN((hsm_t *)me, ctx->creator);
            EQ_QUICK_PUSH(EVT_SETTING_CLOSED);
            break;
        default:
            r = evt;
            break;
    }
    return r;
}


static void volume_event_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    uint32_t c = lv_indev_get_key(lv_indev_get_act());
    switch (c)
    {
        case 'U':
            ST_LOGD("Settings_Volume: Up\n");
            break;
        case 'D':
            ST_LOGD("Settings_Volume: Down\n");
            break;
        default:
            ST_LOGD("Settings_Volume: (%d)\n", c);
            break;
    }
}


event_t const *setting_volume_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Create volume box
            Wire LV_EVENT_KEY handler to process U/D key
        EVT_EXIT:
            Close volume box
        EVT_BUTTON_SETTING_CLICKED:
            Transit to setting_brightness
    */
    event_t const *r = 0;
    setting_t *ctx = &(me->setting_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        ST_LOGD("Setting_Volume: entry\n");
        ctx->popup = lv_barbox_create(lv_scr_act(), 0, 0, 100, 20);
        lv_obj_add_event_cb(ctx->popup, volume_event_handler, LV_EVENT_KEY, (void *)ctx);
        lv_group_remove_all_objs(input_keypad_group);
        lv_group_add_obj(input_keypad_group, ctx->popup);
        break;
    case EVT_EXIT:
        ST_LOGD("Setting_Volume: exit\n");
        lv_barbox_close(ctx->popup);
        ctx->popup = 0;
        break;
    case EVT_BUTTON_SETTING_CLICKED:
        STATE_TRAN(me, &(me->setting_brightness));
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


static void brightness_on_value_changed(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t s = lv_slider_get_value(slider);
    s = s * 5 - 1;
    if (s < 9) s = 9;
    if (s > 99) s = 99;
    config.backlight_brigntness_normal = s;
    config_set_dirty();
    ST_LOGI("Setting_Brightness: %d\n", config.backlight_brigntness_normal);
    backlight_set_direct(config.backlight_brigntness_normal);
}


static void brightness_on_entry(setting_t *ctx)
{
    /* 
        Map keypad up/down
        Create brightness box
        Wire LV_EVENT_VALUE_CHANGED handler to process brightness change
    */
    input_map_keypad(-1, 0);
    input_map_keypad(INPUT_KEY_UP, LV_KEY_UP);
    input_map_keypad(INPUT_KEY_DOWN, LV_KEY_DOWN);
    input_enable_keypad_dev();
    // Brightness range is [9..99]
    // Maps to Slider [2-20] -> brightness = slider * 5 - 1
    // slider = (brightness + 1) / 5
    int32_t s = (config.backlight_brigntness_normal + 1) / 5;
    if (s < 2) s = 2;
    if (s > 20) s = 20;
    ctx->popup = lv_sliderbox_create(lv_scr_act(), &img_setting_brightness, 2, 20, s);
    lv_obj_t *slider = lv_sliderbox_get_slider(ctx->popup);
    lv_group_remove_all_objs(input_keypad_group);
    lv_group_add_obj(input_keypad_group, slider);
    lv_obj_add_event_cb(slider, brightness_on_value_changed, LV_EVENT_VALUE_CHANGED, NULL);
}


static void brightness_on_exit(setting_t *ctx)
{
    /*
        Close popup
        Unmap keypad
    */
    lv_sliderbox_close(ctx->popup);
    ctx->popup = 0;
    input_map_keypad(-1, 0);
    input_disable_keypad_dev();
}


event_t const *setting_brightness_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Map keypad up/down
            Create brightness box
            Wire LV_EVENT_VALUE_CHANGED handler to process brightness change
        EVT_EXIT:
            Close popup
            Unmap keypad
        EVT_BUTTON_SETTING_CLICKED:
            Send EVT_BUTTON_BACK_CLICKED for setting state to handle (exit)
    */
    event_t const *r = 0;
    setting_t *ctx = &(me->setting_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        ST_LOGD("Setting_Brightness: entry\n");
        brightness_on_entry(ctx);
        break;
    case EVT_EXIT:
        ST_LOGD("Setting_Brightness: exit\n");
        brightness_on_exit(ctx);
        break;
    case EVT_BUTTON_SETTING_CLICKED:
        EQ_QUICK_PUSH(EVT_BUTTON_BACK_CLICKED);    // setting state will handle this and close popup
        break;
    default:
        r = evt;
        break;
    }
    return r;
}

