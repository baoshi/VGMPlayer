#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "my_mem.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "lvsupp.h"
#include "tick.h"
#include "input.h"
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


static void alert_setup_input()
{
    // Button
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    // Keypad
    input_disable_keypad_dev();
    input_map_keypad(-1, 0);
    input_map_keypad(INPUT_KEY_PLAY, LV_KEY_ENTER); // for keypad indev, only LV_KEY_ENTER emits RELEASED event
    input_enable_keypad_dev();
}


static void enter_handler(lv_event_t *e)
{
    alert_t* ctx = (alert_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (LV_EVENT_PRESSED == code)
    {
        // If enter key was pressed, disable autoclose
        if (ctx->timer_auto_close != 0)
        {
            tick_disarm_timer_event(ctx->timer_auto_close);
            ctx->timer_auto_close = 0;
        }
    }
    else if (LV_EVENT_CLICKED == code)
    {
        EQ_QUICK_PUSH(EVT_ALERT_MANUAL_CLOSE);
    }
}


event_t const *alert_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
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
        ctx->input = input_export_config();
        alert_setup_input();
        lv_obj_add_event_cb(ctx->popup, enter_handler, LV_EVENT_ALL, (void *)ctx);
        lv_group_add_obj(input_keypad_group, ctx->popup);
        // Auto close timeout
        if (ctx->auto_close_ms > 0)
            ctx->timer_auto_close = tick_arm_timer_event(ctx->auto_close_ms, false, EVT_ALERT_AUTO_CLOSE, true);
        break;
    case EVT_EXIT:
        ALT_LOGD("Alert: exit\n");
        lv_alert_close(ctx->popup);
        ctx->popup = NULL;
        input_restore_config(ctx->input);
        ctx->input = NULL;
        break;
    case EVT_ALERT_MANUAL_CLOSE:
        ALT_LOGD("Alert: manual close\n");
        // no break
    case EVT_ALERT_AUTO_CLOSE:
        ctx->timer_auto_close = 0;
        ALT_LOGD("Alert: close and transit to %s state\n", ctx->creator->name);
        STATE_TRAN((hsm_t *)app, ctx->creator);
        if (ctx->exit_event != 0)
        {
            EQ_QUICK_PUSH(ctx->exit_event);
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
 * @brief Create alert popup and append "alert" sub-state to the current state
 * 
 * @param app           Top level state machine
 * @param icon          Icon displayed besides alert message
 * @param text          Alert message
 * @param auto_close    Time (in ms) the alert will auto close. Set to 0 to disable
 * @param exit_event    Event to post into event queue upon close. If 0, will post EVT_ALERT_CLOSED
 */
void alert_create(app_t *app, const void *icon, const char *text, int auto_close, int exit_event)
{
    alert_t *ctx = &(app->alert_ctx);
    // Popup cannot be nested
    MY_ASSERT(NULL == ctx->popup);
    memset(ctx, 0, sizeof(alert_t));
    // Create popup
    ctx->popup = lv_alert_create(lv_scr_act(), icon, text);
    // Save current state as alert creator
    ctx->creator = STATE_CURR(app);
    ctx->auto_close_ms = auto_close;
    ctx->exit_event = exit_event;
    // Append alert state to current state and transit to it
    ALT_LOGD("Alert: creation, append to %s state\n", ctx->creator->name);
    state_ctor(&(app->alert), "alert", ctx->creator, (event_handler_t)alert_handler);
    STATE_TRAN((hsm_t *)app, &(app->alert));
}
