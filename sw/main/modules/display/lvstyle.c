#include "lvstyle.h"


lv_style_t lvs_invisible_button;
lv_style_t lvs_recolored_icon;

lv_style_t lvs_popup;

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

    lv_style_init(&lvs_recolored_icon);
    lv_style_set_img_recolor(&lvs_recolored_icon, lv_color_make(60, 169, 227));

    // Popup Box
    lv_style_init(&lvs_popup);
    lv_style_set_bg_opa(&lvs_popup, LV_OPA_COVER);              // Same opqaue background as screen
    lv_style_set_bg_color(&lvs_popup, COLOR_BACKGROUND);        // Background color
    lv_style_set_text_color(&lvs_popup, COLOR_TEXT);            // Forground color
    lv_style_set_radius(&lvs_popup, 4);                         // Rounded corner
    lv_style_set_clip_corner(&lvs_popup, true);                 // clip the overflowed content on the rounded corner
    lv_style_set_border_color(&lvs_popup, COLOR_BORDER);        // 2 pixel border width
    lv_style_set_border_width(&lvs_popup, 2);
    lv_style_set_border_post(&lvs_popup, true);                 // border shall draw after children
    lv_style_set_outline_color(&lvs_popup, COLOR_BACKGROUND);   // Outline
    lv_style_set_outline_opa(&lvs_popup, LV_OPA_80);
    lv_style_set_outline_width(&lvs_popup, 4);
    lv_style_set_pad_all(&lvs_popup, 15);                       // Leave some space from border
}

