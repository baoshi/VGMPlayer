#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvinput.h"
#include "lvstyle.h"
#include "lvsupp.h"
#include "event_ids.h"
#include "event_queue.h"
#include "app.h"


// State "settings"
// State "settings" contains 2 sub-states: "settings-volume", "settings-brightness", 
// State "settings" is not initialzed at program start. Caller use settings_create() to append it to the current state
// State "settings" does not transit to other states except for when exit, it transit to the creator's state and emit EVT_SETTING_CLOSED event


#ifndef SETTINGS_DEBUG
# define SETTINGS_DEBUG 1
#endif


// Debug log
#if SETTINGS_DEBUG
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





// When called, append "settings" sub-state to the current state
void settings_create(app_t *me)
{
    state_t *curr = ((hsm_t *)me)->curr;
    settings_t *ctx = &(me->settings_ctx);
    ctx->creator = curr;  // save 
    ST_LOGD("Settings: creation, append to %s state\n", ctx->creator->name);
    state_ctor(&(me->settings), "settings", curr, (event_handler_t)settings_handler);
}


static void settings_exit(app_t *me)
{
    // Transit to the creator's state
    settings_t *ctx = &(me->settings_ctx);
    ST_LOGD("Settings: exit, transit to %s state\n", ctx->creator->name);
    STATE_TRAN_DYNAMIC(me, ctx->creator);
    EQ_QUICK_PUSH(EVT_SETTING_CLOSED);
}


event_t const *settings_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            ST_LOGD("Settings: entry\n");
            // All settings popup are using the same keypad map
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_NE, 'U');  // Remap NE -> character 'U'
            lvi_map_keypad(LVI_BUTTON_SE, 'D');  // Remap SE -> character 'D'
            break;
        }
        case EVT_EXIT:
        {
            ST_LOGD("Settings: exit\n");
            break;
        }
        case EVT_START:
        {
            ST_LOGD("Settings: start\n");
            STATE_START(me, &(me->settings_volume));
            break;
        }
        case EVT_BACK_CLICKED:
        {
            settings_exit(me);
            break;
        }
        default:
        {
            r = evt;
            break;
        }
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


event_t const *settings_volume_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    settings_t *ctx = &(me->settings_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            ST_LOGD("Settings_Volume: entry\n");
            ctx->popup = lv_barbox_create(lv_scr_act(), 0, 100, 20);
            lv_obj_add_event_cb(ctx->popup, volume_event_handler, LV_EVENT_KEY, (void*)ctx);
            lv_group_remove_all_objs(lvi_keypad_group);
            lv_group_add_obj(lvi_keypad_group, ctx->popup);
            break;
        }
        case EVT_EXIT:
        {
            ST_LOGD("Settings_Volume: exit\n");
            lv_barbox_close(ctx->popup);
            ctx->popup = 0;
            break;
        }
        case EVT_SETTING_CLICKED:
        {
            STATE_TRAN(me, &(me->settings_brightness));
            break;
        }
        default:
        {
            r = evt;
            break;
        }
    }
    return r;
}


static void brightness_event_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    int32_t c = *((int32_t *)lv_event_get_param(e));
    switch (c)
    {
        case 'U':
            ST_LOGD("Settings_Brightness: Up\n");
            break;
        case 'D':
            ST_LOGD("Settings_Brigntness: Down\n");
            break;
        default:
            ST_LOGD("Settings_Brigntness: (%d)\n", c);
            break;
    }
}


event_t const *settings_brightness_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    settings_t *ctx = &(me->settings_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            ST_LOGD("Settings_Brightness: entry\n");
            ctx->popup = lv_barbox_create(lv_scr_act(), 0, 100, 50);
            lv_obj_add_event_cb(ctx->popup, brightness_event_handler, LV_EVENT_KEY, (void*)ctx);
            lv_group_remove_all_objs(lvi_keypad_group);
            lv_group_add_obj(lvi_keypad_group, ctx->popup);
            break;
        }
        case EVT_EXIT:
        {
            ST_LOGD("Settings_Brightness: exit\n");
            lv_barbox_close(ctx->popup);
            ctx->popup = 0;
            break;
        }
        case EVT_SETTING_CLICKED:
        {
            EQ_QUICK_PUSH(EVT_BACK_CLICKED);    // this exit settings
            break;
        }
        default:
        {
            r = evt;
            break;
        }
    }
    return r;

}

