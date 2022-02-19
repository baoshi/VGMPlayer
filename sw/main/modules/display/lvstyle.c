#include "lvstyle.h"


lv_style_t lvs_invisible_button;


void lvs_init()
{
    // Invisible button
    lv_style_init(&lvs_invisible_button);
    lv_style_set_text_opa(&lvs_invisible_button, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&lvs_invisible_button, LV_OPA_TRANSP);
    lv_style_set_border_width(&lvs_invisible_button, 0);
    lv_style_set_shadow_width(&lvs_invisible_button, 0);
    lv_style_set_outline_width(&lvs_invisible_button, 0);
    lv_style_set_outline_opa(&lvs_invisible_button, LV_OPA_TRANSP);
}

