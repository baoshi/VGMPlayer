#include "lvstyle.h"



lv_style_t lvs_default;
lv_style_t lvs_scrollbar;
lv_style_t lvs_list;
lv_style_t lvs_list_btn, lvs_list_btn_focused, lvs_list_btn_pressed;
lv_style_t lvs_bar_bg, lvs_bar_ind;
lv_style_t lvs_slider_knob;
lv_style_t lvs_msgbox;

lv_style_t lvs_invisible_btn;
lv_style_t lvs_recolored_icon;
lv_style_t lvs_browser_file_list;


void lvs_init()
{
    // Invisible button
    lv_style_init(&lvs_invisible_btn);
    lv_style_set_text_opa(&lvs_invisible_btn, LV_OPA_TRANSP);
    lv_style_set_bg_opa(&lvs_invisible_btn, LV_OPA_TRANSP);
    lv_style_set_border_width(&lvs_invisible_btn, 0);
    lv_style_set_shadow_width(&lvs_invisible_btn, 0);
    lv_style_set_outline_width(&lvs_invisible_btn, 0);
    lv_style_set_outline_opa(&lvs_invisible_btn, LV_OPA_TRANSP);

    // Default style
    lv_style_init(&lvs_default);
    lv_style_set_bg_opa(&lvs_default, LV_OPA_COVER);
    lv_style_set_bg_color(&lvs_default, COLOR_BACKGROUND);
    lv_style_set_text_color(&lvs_default, COLOR_TEXT);
    lv_style_set_text_font(&lvs_default, LV_FONT_DEFAULT);

    // Scrollbar
    lv_style_init(&lvs_scrollbar);
    lv_style_set_bg_color(&lvs_scrollbar, COLOR_BUTTON_FACE_FOCUSED);
    lv_style_set_bg_opa(&lvs_scrollbar, LV_OPA_COVER);
    lv_style_set_pad_left(&lvs_scrollbar, 4);
    lv_style_set_pad_right(&lvs_scrollbar, 6);
    lv_style_set_pad_top(&lvs_scrollbar, 6);
    lv_style_set_pad_bottom(&lvs_scrollbar, 6);
    lv_style_set_size(&lvs_scrollbar, 5);
    lv_style_set_radius(&lvs_scrollbar, CORNER_RADIUS);     // Rounded corner

    // List
    lv_style_init(&lvs_list);
    lv_style_set_bg_opa(&lvs_list, LV_OPA_COVER);           // Same opqaue background as screen
    lv_style_set_bg_color(&lvs_list, COLOR_BACKGROUND);
    lv_style_set_border_color(&lvs_list, COLOR_BORDER);     // 1 pixel border
    lv_style_set_radius(&lvs_list, CORNER_RADIUS);          // Rounded corner
    lv_style_set_border_width(&lvs_list, 1);
    lv_style_set_border_post(&lvs_list, true);              // border shall draw after children
    lv_style_set_pad_right(&lvs_list, 14);                  // right with scrollbar
    lv_style_set_pad_left(&lvs_list, 6);
    lv_style_set_pad_top(&lvs_list, 6);
    lv_style_set_pad_bottom(&lvs_list, 6);
    lv_style_set_pad_row(&lvs_list, 1);                     // 1 pixels gap between each row

    // List button
    lv_style_init(&lvs_list_btn);
    lv_style_set_pad_left(&lvs_list_btn, 4);
    lv_style_set_pad_right(&lvs_list_btn, 4);
    lv_style_set_pad_top(&lvs_list_btn, 4);
    lv_style_set_pad_bottom(&lvs_list_btn, 4);
    lv_style_set_radius(&lvs_list_btn, CORNER_RADIUS);
    lv_style_set_bg_opa(&lvs_list_btn_focused, LV_OPA_COVER);
    lv_style_set_bg_color(&lvs_list_btn_focused, COLOR_BUTTON_FACE_INACTIVE);
    // List button when focused
    lv_style_init(&lvs_list_btn_focused);
    lv_style_set_bg_opa(&lvs_list_btn_focused, LV_OPA_COVER);
    lv_style_set_bg_color(&lvs_list_btn_focused, COLOR_BUTTON_FACE_FOCUSED);
    // List button when pressed
    lv_style_init(&lvs_list_btn_pressed);
    lv_style_set_bg_opa(&lvs_list_btn_pressed, LV_OPA_COVER);
    lv_style_set_bg_color(&lvs_list_btn_pressed, COLOR_BUTTON_FACE_PRESSED);

    // Bar
    lv_style_init(&lvs_bar_bg);
    lv_style_set_border_color(&lvs_bar_bg, COLOR_BORDER);
    lv_style_set_border_width(&lvs_bar_bg, 1);
    lv_style_set_bg_opa(&lvs_bar_bg, LV_OPA_COVER);
    lv_style_set_bg_color(&lvs_bar_bg, COLOR_DARK);
    lv_style_set_height(&lvs_bar_bg, 10);   // bar background height
    lv_style_set_pad_all(&lvs_bar_bg, 2);   // indicator is 2 pixels within background height
    lv_style_set_radius(&lvs_bar_bg, LV_RADIUS_CIRCLE);
    lv_style_init(&lvs_bar_ind);
    lv_style_set_bg_opa(&lvs_bar_ind, LV_OPA_COVER);
    lv_style_set_bg_color(&lvs_bar_ind, COLOR_LIGHT);
    lv_style_set_radius(&lvs_bar_ind, LV_RADIUS_CIRCLE);

    // Slider Knob
    lv_style_init(&lvs_slider_knob);
    lv_style_set_bg_opa(&lvs_slider_knob, LV_OPA_COVER);
    lv_style_set_bg_color(&lvs_slider_knob, COLOR_LIGHT);
    lv_style_set_border_color(&lvs_slider_knob, COLOR_DARK);
    lv_style_set_border_width(&lvs_slider_knob, 1);
    lv_style_set_pad_all(&lvs_slider_knob, 3);  // make knob larger
    lv_style_set_radius(&lvs_slider_knob, LV_RADIUS_CIRCLE);

     // Popup Box / Msg box
    lv_style_init(&lvs_msgbox);
    lv_style_set_bg_opa(&lvs_msgbox, LV_OPA_COVER);              // Same opqaue background as screen
    lv_style_set_bg_color(&lvs_msgbox, COLOR_BACKGROUND);        // Background color
    lv_style_set_text_color(&lvs_msgbox, COLOR_TEXT);            // Forground color
    lv_style_set_radius(&lvs_msgbox, CORNER_RADIUS);             // Rounded corner
    lv_style_set_clip_corner(&lvs_msgbox, true);                 // clip the overflowed content on the rounded corner
    lv_style_set_border_color(&lvs_msgbox, COLOR_BORDER);        // 2 pixel border width
    lv_style_set_border_width(&lvs_msgbox, 2);
    lv_style_set_border_post(&lvs_msgbox, true);                 // border shall draw after children
    lv_style_set_outline_color(&lvs_msgbox, COLOR_BACKGROUND);   // Outline
    lv_style_set_outline_opa(&lvs_msgbox, LV_OPA_80);
    lv_style_set_outline_width(&lvs_msgbox, 4);
    lv_style_set_pad_all(&lvs_msgbox, 15);                       // Leave some space from border

    // Recolored ALPHA-only channel icon
    lv_style_init(&lvs_recolored_icon);
    lv_style_set_img_recolor(&lvs_recolored_icon, COLOR_BORDER);
    lv_style_set_img_recolor_opa(&lvs_recolored_icon, LV_OPA_COVER);

    // Styles for file list in browser screen
    lv_style_init(&lvs_browser_file_list);
    lv_style_set_border_side(&lvs_browser_file_list, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_left(&lvs_browser_file_list, 2);

}

