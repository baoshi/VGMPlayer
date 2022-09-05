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
#include "popup.h"
#include "battery.h"
#include "app.h"


#ifndef TOP_DEBUG
# define TOP_DEBUG 1
#endif

// Debug log
#if TOP_DEBUG
#define TOP_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define TOP_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define TOP_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define TOP_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define TOP_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define TOP_LOGD(x, ...)
#define TOP_LOGI(x, ...)
#define TOP_LOGW(x, ...)
#define TOP_LOGE(x, ...)
#define TOP_DEBUGF(x, ...)
#endif

#define UI_UPDATE_INTERVAL_MS 500


static void top_on_ui_update(app_t *app)
{
    static battery_state_t old_state = BATTERY_STATE_NONE;
    battery_state_t state = battery_get_state();
    if (old_state != state)
    {
        old_state = state;
        switch (state)
        {
        case BATTERY_STATE_UNKNOWN:
            lv_img_set_src(app->img_battery, &img_battery_unknown);
            break;
        case BATTERY_STATE_CHARGING:
            lv_img_set_src(app->img_battery, &img_battery_charging);
            break;
        case BATTERY_STATE_100:
            lv_img_set_src(app->img_battery, &img_battery_100);
            break;
        case BATTERY_STATE_75:
            lv_img_set_src(app->img_battery, &img_battery_75);
            break;
        case BATTERY_STATE_50:
            lv_img_set_src(app->img_battery, &img_battery_50);
            break;
        case BATTERY_STATE_25:
            lv_img_set_src(app->img_battery, &img_battery_25);
            break;
        case BATTERY_STATE_0:
            lv_img_set_src(app->img_battery, &img_battery_0);
            break;
        }
    }
}


event_t const *top_handler(app_t *app, event_t const *evt)
{
    event_t const *r = 0;
    switch (evt->code)
    {
    case EVT_ENTRY:
        lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        app->img_battery = lv_img_create(lv_layer_top());
        lv_obj_set_pos(app->img_battery, 213, 7);  // battery image height 26x13, top bar has 26 pixels
        app->timer_ui_update = tick_arm_timer_event(UI_UPDATE_INTERVAL_MS, true, EVT_APP_UI_UPDATE, true);
        break;
    case EVT_EXIT:
        tick_disarm_timer_event(app->timer_ui_update);
        app->timer_ui_update = 0;
        break;
    case EVT_START:
        STATE_START(app, &(app->browser));
        break;
    case EVT_APP_UI_UPDATE:
        top_on_ui_update(app);
        break;
    case EVT_OPEN_BRIGHTNESS_POPUP:
        brightness_popup(app);
        break;
    case EVT_CLOSE_BRIGHTNESS_POPUP:
        brightness_close(app);
        break;
    case EVT_CLOSE_ALERT:
        // alert can popup from anywhere, close alert happens here
        alert_close(app);
        break;
    default:
        TOP_LOGD("Top: event %d not handled\n", evt->code);
    }
    return r;
}
