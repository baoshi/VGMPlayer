#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvstyle.h"
#include "lvtheme.h"
#include "lvsupp.h"
#include "event_ids.h"
#include "event_queue.h"
#include "input.h"
#include "popup.h"
#include "backlight.h"
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


event_t const *top_handler(app_t *app, event_t const *evt)
{
    event_t const *r = 0;
    switch (evt->code)
    {
    case EVT_ENTRY:
        break;
    case EVT_EXIT:
        break;
    case EVT_START:
        STATE_START(app, &(app->browser));
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
