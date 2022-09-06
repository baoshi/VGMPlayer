#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "lvsupp.h"
#include "event_ids.h"
#include "event_queue.h"
#include "tick.h"
#include "input.h"
#include "audio.h"
#include "app.h"


#ifndef VOLUME_DEBUG
# define VOLUME_DEBUG 1
#endif


// Debug log
#if VOLUME_DEBUG
#define VOL_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define VOL_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define VOL_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define VOL_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define VOL_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define VOL_LOGD(x, ...)
#define VOL_LOGI(x, ...)
#define VOL_LOGW(x, ...)
#define VOL_LOGE(x, ...)
#define VOL_DEBUGF(x, ...)
#endif


static void volume_on_value_changed(lv_event_t *e)
{
    volume_t *ctx = (volume_t *)lv_event_get_user_data(e);
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t s = lv_slider_get_value(slider);
    if (audio_get_jack_state())
    {
        // headphone
        config_set_dirty();
        config.volume_headphone = s;
        VOL_LOGI("Volume: headphone set %d\n", s);
        audio_set_headphone_volume(s);
    }
    else
    {
        // speaker
        config_set_dirty();
        config.volume_speaker = s;
        VOL_LOGI("Volume: speaker set %d\n", s);
        audio_set_speaker_volume(s);
    }
    if (ctx->timer_auto_close) tick_reset_timer_event(ctx->timer_auto_close);
}


static void create_volume_popup(volume_t *ctx)
{
    // Create volume popup
    lv_obj_t *slider;
    if (audio_get_jack_state())
    {
        ctx->popup = lv_sliderbox_create(lv_scr_act(), &img_popup_headphone, 0, 63, config.volume_headphone);
        slider = lv_sliderbox_get_slider(ctx->popup);
        lv_obj_add_event_cb(slider, volume_on_value_changed, LV_EVENT_VALUE_CHANGED, (void *)ctx);
        lv_group_add_obj(ctx->keypad_group, slider);
    }
    else
    {
        ctx->popup = lv_sliderbox_create(lv_scr_act(), &img_popup_speaker, 0, 63, config.volume_speaker);
        slider = lv_sliderbox_get_slider(ctx->popup);
        lv_obj_add_event_cb(slider, volume_on_value_changed, LV_EVENT_VALUE_CHANGED, (void *)ctx);
        lv_group_add_obj(ctx->keypad_group, slider);
    }
}


void volume_popup(app_t *app)
{
    VOL_LOGD("Volume: popup\n");
    volume_t *ctx = &(app->volume_ctx);
    MY_ASSERT(NULL == ctx->popup);
    memset(ctx, 0, sizeof(volume_t));

    // Save previous input config
    ctx->prev_input = input_save();

    // Setup own input
    // Button
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_CLOSE_VOLUME_POPUP_CONT);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_CLOSE_VOLUME_POPUP);
    input_enable_button_dev();
    // Keypad
    input_disable_keypad_dev();
    input_map_keypad(-1, 0);
    input_map_keypad(INPUT_KEY_UP, LV_KEY_UP);
    input_map_keypad(INPUT_KEY_DOWN, LV_KEY_DOWN);
    ctx->keypad_group = lv_group_create();
    lv_indev_set_group(indev_keypad, ctx->keypad_group);
    input_enable_keypad_dev();
    // Create volume popup
    create_volume_popup(ctx);

    // auto close event
    ctx->timer_auto_close = tick_arm_timer_event(POPUP_AUTO_CLOSE_MS, false, EVT_CLOSE_VOLUME_POPUP, true);
}


void volume_popup_refresh(app_t *app)
{
    // if volume popup is not shown, do nothing.
    // otherwise recreate it to reflect audio destination change.
    volume_t *ctx = &(app->volume_ctx);
    MY_ASSERT(ctx->popup != NULL);
    lv_group_remove_all_objs(ctx->keypad_group);
    lv_sliderbox_close(ctx->popup);
    // Create new volume popup
    create_volume_popup(ctx);
    // reset autoclose timer
    if (ctx->timer_auto_close) tick_reset_timer_event(ctx->timer_auto_close);
}


void volume_close(app_t *app)
{
    if ((!app->busy) && config_is_dirty())
    {
        config_save();
    }
    volume_t *ctx = &(app->volume_ctx);
    if (ctx->timer_auto_close != 0)
    {
        tick_disarm_timer_event(ctx->timer_auto_close);
        ctx->timer_auto_close = 0;
    }
    lv_group_remove_all_objs(ctx->keypad_group);
    lv_group_del(ctx->keypad_group);
    ctx->keypad_group = NULL;
    if (ctx->popup)
    {
        lv_sliderbox_close(ctx->popup);
        ctx->popup = NULL;
    }
    // restore previous input
    input_load(ctx->prev_input);
    ctx->prev_input = NULL;
    VOL_LOGD("Volume: closed\n");
}