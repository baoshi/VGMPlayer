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

#ifndef BRIGHTNESS_DEBUG
# define BRIGHTNESS_DEBUG 1
#endif


// Debug log
#if BRIGHTNESS_DEBUG
#define BRT_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define BRT_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define BRT_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define BRT_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define BRT_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define BRT_LOGD(x, ...)
#define BRT_LOGI(x, ...)
#define BRT_LOGW(x, ...)
#define BRT_LOGE(x, ...)
#define BRT_DEBUGF(x, ...)
#endif


static void brightness_on_value_changed(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    int32_t s = lv_slider_get_value(slider);
    s = s * 5 - 1;
    if (s < 9) s = 9;
    if (s > 99) s = 99;
    config.backlight_brigntness_normal = s;
    config_set_dirty();
    BRT_LOGI("Brightness: set %d\n", config.backlight_brigntness_normal);
    backlight_set_direct(config.backlight_brigntness_normal);
}


void brightness_popup(app_t *app)
{
    BRT_LOGD("Brigntness: popup\n");

    brightness_t *ctx = &(app->brightness_ctx);
    MY_ASSERT(NULL == ctx->popup);
    memset(ctx, 0, sizeof(brightness_t));

    // Save previous input config
    ctx->prev_input = input_save();

    // Setup own input
    // Button
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_CLOSE_BRIGHTNESS_POPUP);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_CLOSE_BRIGHTNESS_POPUP);
    input_enable_button_dev();
    // Keypad
    input_disable_keypad_dev();
    input_map_keypad(-1, 0);
    input_map_keypad(INPUT_KEY_UP, LV_KEY_UP);
    input_map_keypad(INPUT_KEY_DOWN, LV_KEY_DOWN);
    ctx->keypad_group = lv_group_create();
    lv_indev_set_group(indev_keypad, ctx->keypad_group);
    input_enable_keypad_dev();

    // Brightness range is [9..99]
    // Maps to Slider [2-20] -> brightness = slider * 5 - 1
    // slider = (brightness + 1) / 5
    int32_t s = (config.backlight_brigntness_normal + 1) / 5;
    if (s < 2) s = 2;
    if (s > 20) s = 20;
    ctx->popup = lv_sliderbox_create(lv_scr_act(), &img_popup_brightness, 2, 20, s);
    lv_obj_t *slider = lv_sliderbox_get_slider(ctx->popup);
    lv_group_add_obj(ctx->keypad_group, slider);
    lv_obj_add_event_cb(slider, brightness_on_value_changed, LV_EVENT_VALUE_CHANGED, NULL);
}


void brightness_close(app_t *app)
{
    brightness_t *ctx = &(app->brightness_ctx);
    lv_group_remove_all_objs(ctx->keypad_group);
    lv_group_del(ctx->keypad_group);
    ctx->keypad_group = NULL;
    lv_sliderbox_close(ctx->popup);
    ctx->popup = NULL;
    // restore previous input
    input_load(ctx->prev_input);
    ctx->prev_input = NULL;
    BRT_LOGD("Brightness: closed\n");
}