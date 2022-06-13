#include <lvgl.h>
#include "splash.h"


static void splash_event_handler(lv_event_t *e)
{
    lv_obj_t *screen = (lv_obj_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (screen)
    {
        switch (code)
        {
        case LV_EVENT_SCREEN_UNLOADED:
            lv_obj_del(screen);
            break;
        default:
            break;
        }
    }
}

void splash()
{

    lv_obj_t *screen = lv_scr_act();
    // Self-destruction callback
    lv_obj_add_event_cb(screen, splash_event_handler, LV_EVENT_ALL, (void *)screen);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_t *label1 = lv_label_create(screen);
    lv_label_set_recolor(label1, true); // Enable re-coloring by commands in the text
    lv_label_set_text(label1, "#0000ff V##ff00ff G##ff0000 M##ffffff Player#");
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    // Draw screen
    lv_refr_now(NULL);
}
