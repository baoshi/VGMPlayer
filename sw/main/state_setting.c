#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvinput.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "lvsupp.h"
#include "event_ids.h"
#include "event_queue.h"
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
}


event_t const *setting_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Map keypad to U/D keycode
        EVT_EXIT:
            Disable keypad
        EVT_START:
            Start setting_volume
        EVT_BACK_CLICKED:
            Transit to creator state and send EVT_SETTING_CLOSED
    */
    setting_t *ctx = &(me->setting_ctx);
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
            ST_LOGD("Setting: entry\n");
            // All setting popup are using the same keypad map
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_NE, 'U');  // Remap NE -> LV_EVENT_KEY 'U'
            lvi_map_keypad(LVI_BUTTON_SE, 'D');  // Remap SE -> LV_EVENT_KEY 'D'
            break;
        case EVT_EXIT:
            ST_LOGD("Setting: exit\n");
            lvi_disable_keypad();
            break;
        case EVT_START:
            ST_LOGD("Setting: start\n");
            STATE_START(me, &(me->setting_volume));
            break;
        case EVT_BACK_CLICKED:
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
    int32_t c = *((int32_t *)lv_event_get_param(e));
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
        EVT_SETTING_CLICKED:
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
        lv_group_remove_all_objs(lvi_keypad_group);
        lv_group_add_obj(lvi_keypad_group, ctx->popup);
        break;
    case EVT_EXIT:
        ST_LOGD("Setting_Volume: exit\n");
        lv_group_remove_all_objs(lvi_keypad_group);
        lv_barbox_close(ctx->popup);
        ctx->popup = 0;
        break;
    case EVT_SETTING_CLICKED:
        STATE_TRAN(me, &(me->setting_brightness));
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


static void brightness_event_handler(lv_event_t* e)
{
    setting_t* ctx = (setting_t*)lv_event_get_user_data(e);
    int32_t c = *((int32_t *)lv_event_get_param(e));
    switch (c)
    {
        case 'U':
            config.backlight_brigntness_normal += 10;
            if (config.backlight_brigntness_normal > 99) config.backlight_brigntness_normal = 99;
            config_set_dirty();
            ST_LOGI("Setting_Brightness: %d\n", config.backlight_brigntness_normal);
            backlight_set_direct(config.backlight_brigntness_normal);
            lv_barbox_set_value(ctx->popup, config.backlight_brigntness_normal);
            break;
        case 'D':
            ST_LOGD("Setting_Brigntness: Down\n");
            config.backlight_brigntness_normal -= 10;
            if (config.backlight_brigntness_normal < 0) config.backlight_brigntness_normal = 0;
            config_set_dirty();
            ST_LOGI("Setting_Brightness: %d\n", config.backlight_brigntness_normal);
            backlight_set_direct(config.backlight_brigntness_normal);
            lv_barbox_set_value(ctx->popup, config.backlight_brigntness_normal);
            break;
        default:
            ST_LOGD("Setting_Brigntness: Unknown event (%d)\n", c);
            break;
    }
}


event_t const *setting_brightness_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Create brightnes box
            Wire LV_EVENT_KEY handler to process U/D key
        EVT_EXIT:
            Close volume box
        EVT_SETTING_CLICKED:
            Send EVT_BACK_CLICKED for setting state to handle (exit)
    */
    event_t const *r = 0;
    setting_t *ctx = &(me->setting_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        ST_LOGD("Setting_Brightness: entry\n");
        ctx->popup = lv_barbox_create(lv_scr_act(), &img_setting_brightness, 0, 99, config.backlight_brigntness_normal);
        lv_obj_add_event_cb(ctx->popup, brightness_event_handler, LV_EVENT_KEY, (void*)ctx);
        lv_group_remove_all_objs(lvi_keypad_group);
        lv_group_add_obj(lvi_keypad_group, ctx->popup);
        break;
    case EVT_EXIT:
        ST_LOGD("Setting_Brightness: exit\n");
        lv_group_remove_all_objs(lvi_keypad_group);
        lv_barbox_close(ctx->popup);
        ctx->popup = 0;
        break;
    case EVT_SETTING_CLICKED:
        EQ_QUICK_PUSH(EVT_BACK_CLICKED);    // setting state will handle this and close popup
        break;
    default:
        r = evt;
        break;
    }
    return r;
}

