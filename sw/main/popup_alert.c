#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "lvsupp.h"
#include "input.h"
#include "app.h"


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


static void key_enter_handler(lv_event_t *e)
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
        EQ_QUICK_PUSH(EVT_CLOSE_ALERT);
    }
}


/**
 * @brief Create alert popup
 * 
 * @param ctx           top level state machine
 * @param icon          Icon displayed besides alert message
 * @param text          Alert message
 * @param auto_close    Time (in ms) the alert will auto close. Set to 0 to disable
 */
void alert_popup(app_t *app, const void *icon, const char *text, int auto_close)
{
    ALT_LOGD("Alert: popup\n");
    
    alert_t *ctx = &(app->alert_ctx);
    MY_ASSERT(NULL == ctx->popup);
    memset(ctx, 0, sizeof(alert_t));

    // Save previous input config
    ctx->prev_input = input_save();

    // Setup own input
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    // PLAY key used as Keypad
    input_disable_keypad_dev();
    input_map_keypad(-1, 0);
    input_map_keypad(INPUT_KEY_PLAY, LV_KEY_ENTER); // for keypad indev, only LV_KEY_ENTER emits RELEASED event
    ctx->keypad_group = lv_group_create();
    lv_indev_set_group(indev_keypad, ctx->keypad_group);
    input_enable_keypad_dev();

    // create alert popup
    ctx->popup = lv_alert_create(lv_scr_act(), icon, text);
    lv_group_add_obj(ctx->keypad_group, ctx->popup);
    lv_obj_add_event_cb(ctx->popup, key_enter_handler, LV_EVENT_ALL, (void *)ctx);

    // auto close event
    if (auto_close > 0)
        ctx->timer_auto_close = tick_arm_timer_event(auto_close, false, EVT_CLOSE_ALERT, true);
}


void alert_close(app_t *app)
{
    alert_t *ctx = &(app->alert_ctx);
    if (ctx->timer_auto_close != 0)
    {
        tick_disarm_timer_event(ctx->timer_auto_close);
        ctx->timer_auto_close = 0;
    }
    // disable input
    lv_group_remove_all_objs(ctx->keypad_group);
    lv_group_del(ctx->keypad_group);
    ctx->keypad_group = NULL;
    // close popup
    lv_alert_close(ctx->popup);
    ctx->popup = NULL;
    // restore previous input
    input_load(ctx->prev_input);
    ctx->prev_input = NULL;
    ALT_LOGD("Alert: closed\n");
}
