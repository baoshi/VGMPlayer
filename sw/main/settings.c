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


event_t const *settings_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            ST_LOGD("Settings: entry\n");
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
            STATE_START(me, &(me->settings_brightness));
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
            // Remap keypad and attach input group to the barbox
            lv_group_remove_all_objs(lvi_keypad_group);
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_NE, 'U');  // Remap NE -> character 'U'
            lvi_map_keypad(LVI_BUTTON_SE, 'D');  // Remap SE -> character 'D'
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
        case EVT_BACK_CLICKED:
        {
            STATE_TRAN(me, ctx->super);
            EQ_QUICK_PUSH(EVT_SETTINGS_CLOSED);
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


event_t const *settings_earpiece_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    settings_t *ctx = &(me->settings_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            ST_LOGD("Settings_Earpiece: entry\n");
            break;
        }
        case EVT_EXIT:
        {
            ST_LOGD("Settings_Earpiece: exit\n");
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


event_t const *settings_speaker_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    settings_t *ctx = &(me->settings_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            ST_LOGD("Settings_Speaker: entry\n");
            break;
        }
        case EVT_EXIT:
        {
            ST_LOGD("Settings_Speaker: exit\n");
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



void settings_popup(app_t *me, state_t *super)
{
    settings_t *ctx = &(me->settings_ctx);
    ctx->super = super;
    state_ctor(&(me->settings), "settings", super, (event_handler_t)settings_handler);
    STATE_TRAN(me, &(me->settings));
}