#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "my_mem.h"
#include "lvinput.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "lvsupp.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "backlight.h"
#include "app.h"


// State "message"

#ifndef ALERT_DEBUG
# define ALERT_DEBUG 1
#endif


// Debug log
#if ALERT_DEBUG
#define ALT_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define ALT_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define ALT_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define ALT_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define ALT_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define ALT_LOGD(x, ...)
#define ALT_LOGI(x, ...)
#define ALT_LOGW(x, ...)
#define ALT_LOGE(x, ...)
#define ALT_DEBUGF(x, ...)
#endif



static void alert_keypad_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    uint32_t c = lv_indev_get_key(lv_indev_get_act());
    if ('P' == c)
    {
        EQ_QUICK_PUSH(EVT_ALERT_MANUAL_CLOSE);
    }
}


event_t const *alert_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Create alert popup
            Map keypad
            Arm autoclose
        EVT_EXIT:
            Close popup, disable keypad
        EVT_ALERT_MANUAL_CLOSE
            Disarm auto close and do AUTO_CLOSE
        EVT_ALERT_AUTO_CLOSE:
            Close alert and and send EVT_ALERT_CLOSED
    */
    alert_t *ctx = &(app->alert_ctx);
    event_t const *r = 0;
    switch (evt->code)
    {
    case EVT_ENTRY:
        ALT_LOGD("Alert: entry\n");
        // Create alert popup
        ctx->popup = lv_alert_create(lv_scr_act(), ctx->icon, ctx->text);
        // Setup keypad
        lvi_disable_keypad();
        lvi_map_keypad(LVI_BUTTON_PLAY, 'P');  // Remap PLAY -> LV_EVENT_KEY 'P'
        lv_obj_add_event_cb(ctx->popup, alert_keypad_handler, LV_EVENT_KEY, (void *)ctx);
        lv_group_remove_all_objs(lvi_keypad_group);
        lv_group_add_obj(lvi_keypad_group, ctx->popup);
        // Auto close timeout
        ctx->timer_auto_close = tick_arm_timer_event(2000, false, EVT_ALERT_AUTO_CLOSE, true);
        break;
    case EVT_EXIT:
        ALT_LOGD("Alert: exit\n");
        lv_group_remove_all_objs(lvi_keypad_group);
        lv_alert_close(ctx->popup);
        lvi_disable_keypad();
        if (ctx->text)
        {
            MY_FREE(ctx->text);
            ctx->text = 0;
        }
        break;
    case EVT_ALERT_MANUAL_CLOSE:
        ALT_LOGD("Alert: manual close\n");
        if (ctx->timer_auto_close != 0)
        {
            tick_disarm_timer_event(ctx->timer_auto_close);
        }
        // no break
    case EVT_ALERT_AUTO_CLOSE:
        ctx->timer_auto_close = 0;
        ALT_LOGD("Alert: close and transit to %s state\n", ctx->creator->name);
        STATE_TRAN((hsm_t *)app, ctx->creator);
        if (ctx->exit_event.code != 0)
        {
            event_queue_push_back(&(ctx->exit_event), true);
        }
        else
        {
            EQ_QUICK_PUSH(EVT_ALERT_CLOSED);
        }
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


/**
 * @brief Append "exception" sub-state to the current state and display alert popup
 * 
 * @param app   Top level state machine
 * @param icon  Icon displayed besides alert message
 * @param text  Alert message
 * @param exit  Event to post into event queue upon close, if null, will post EVT_ALERT_CLOSED
 */
void alert_create(app_t *app, const void *icon, const char *text, const event_t *exit)
{
    alert_t *ctx = &(app->alert_ctx);
    memset(ctx, 0, sizeof(alert_t));
    // Save current state as alert creator
    ctx->creator = STATE_CURR(app);
    // members
    ctx->icon = icon;
    if (text)
    {
        int len = strlen(text);
        ctx->text = MY_MALLOC(len + 1);
        if (ctx->text)
        {
            memcpy(ctx->text, text, len);
            ctx->text[len] = '\0';
        }
    }
    if (exit)
    {
        ctx->exit_event.code = exit->code;
        ctx->exit_event.param = exit->param;
    }
    // Append alert state to current state and transit to it
    ALT_LOGD("Alert: creation, append to %s state\n", ctx->creator->name);
    state_ctor(&(app->alert), "alert", ctx->creator, (event_handler_t)alert_handler);
    STATE_TRAN((hsm_t *)app, &(app->alert));
}
