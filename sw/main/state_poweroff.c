#include "sw_conf.h"
#include "my_debug.h"
#include "lvstyle.h"
#include "lvassets.h"
#include "lvsupp.h"
#include "event_ids.h"
#include "event_queue.h"
#include "tick.h"
#include "input.h"
#include "popup.h"
#include "battery.h"
#include "ec.h"
#include "app.h"


#ifndef OFF_DEBUG
# define OFF_DEBUG 1
#endif

// Debug log
#if OFF_DEBUG
#define OFF_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define OFF_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define OFF_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define OFF_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define OFF_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define OFF_LOGD(x, ...)
#define OFF_LOGI(x, ...)
#define OFF_LOGW(x, ...)
#define OFF_LOGE(x, ...)
#define OFF_DEBUGF(x, ...)
#endif


event_t const *poweroff_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Disable all inputs
        EVT_EXIT:
        EVT_START:
    */
    event_t const *r = 0;
    poweroff_t *ctx = &(app->poweroff_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        OFF_LOGD("Poweroff: entry\n");
        input_disable_button_dev();
        input_disable_keypad_dev();
        ctx->screen = lv_obj_create(NULL);
        lv_scr_load(ctx->screen);
        alert_popup(app, NULL, "Power Off", 1000, EVT_APP_POWER_OFF);
        break;
    case EVT_EXIT:
        break;
    case EVT_START:
        break;
    default:
        r = evt;
        break;
    }
    return r;
}
