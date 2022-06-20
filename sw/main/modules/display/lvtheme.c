#include "lvtheme.h"

/*********************
 *      DEFINES
 *********************/
#define COLOR_BACKGROUND            lv_color_hex(0x0A1929)
#define COLOR_TEXT                  lv_color_hex(0xBABAC2)
#define COLOR_BORDER                lv_color_hex(0x14385C)
#define COLOR_BUTTON_FACE_INACTIVE  lv_color_hex(0x14385C)
#define COLOR_BUTTON_TEXT_INACTIVE  lv_color_hex(0x66B2FF)
#define COLOR_BUTTON_FACE_ACTIVE    lv_color_hex(0x14385C)
#define COLOR_BUTTON_TEXT_ACTIVE    lv_color_hex(0xFFFFFF)


#define COLOR_SCR     lv_color_black()

#define COLOR_WHITE   lv_color_white()
#define COLOR_LIGHT   lv_palette_lighten(LV_PALETTE_GREY, 1)
#define COLOR_DARK    lv_palette_main(LV_PALETTE_GREY)
#define COLOR_DIM     lv_palette_darken(LV_PALETTE_GREY, 2)
#define SCROLLBAR_WIDTH     2

#define RADIUS_DEFAULT 5


/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    lv_style_t scr;
    lv_style_t white;
    lv_style_t light;
    lv_style_t dark;
    lv_style_t dim;
    lv_style_t scrollbar;
    lv_style_t scrollbar_scrolled;
#if LV_USE_TEXTAREA
    lv_style_t ta_cursor;
#endif
    lv_style_t panel;
    lv_style_t list_btn, list_btn_focused, list_btn_pressed;
} theme_styles_t;


static void style_init_reset(lv_style_t * style);
static void theme_apply(lv_theme_t * th, lv_obj_t * obj);


static theme_styles_t styles;
static lv_theme_t theme;
static bool inited;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init(void)
{
    // Default screen style
    style_init_reset(&styles.scr);
    lv_style_set_bg_opa(&styles.scr, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.scr, COLOR_BACKGROUND);
    lv_style_set_text_color(&styles.scr, COLOR_TEXT);
    lv_style_set_text_font(&styles.scr, LV_FONT_DEFAULT);
    // Scrollbar
    style_init_reset(&styles.scrollbar);
    lv_style_set_bg_color(&styles.scrollbar, COLOR_BUTTON_FACE_INACTIVE);
    lv_style_set_bg_opa(&styles.scrollbar, LV_OPA_COVER);
    lv_style_set_radius(&styles.scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_pad_right(&styles.scrollbar, 4);
    lv_style_set_pad_top(&styles.scrollbar,  4);
    lv_style_set_pad_bottom(&styles.scrollbar,  4);
    lv_style_set_size(&styles.scrollbar,  5);
    // Panel
    style_init_reset(&styles.panel);
    lv_style_set_radius(&styles.panel, RADIUS_DEFAULT); // Round rectangle
    lv_style_set_bg_opa(&styles.panel, LV_OPA_COVER);   // Same opqaue background as screen
    lv_style_set_bg_color(&styles.panel, COLOR_BACKGROUND);
    lv_style_set_border_color(&styles.panel, COLOR_BORDER);  // 1 pixel border
    lv_style_set_border_width(&styles.panel, 1);
    lv_style_set_border_post(&styles.panel, true);      // border shall draw after children
    lv_style_set_pad_right(&styles.panel, 14);
    lv_style_set_pad_top(&styles.panel, 8);
    lv_style_set_pad_bottom(&styles.panel, 8);
    lv_style_set_pad_left(&styles.panel, 8);
    lv_style_set_clip_corner(&styles.panel, true);      // clip the overflowed content on the rounded corner
    
    // List box
    lv_style_set_pad_row(&styles.panel, 4);             // 4 pixels gap between each row
    // Default list button
    style_init_reset(&styles.list_btn);
    lv_style_set_pad_left(&styles.list_btn, 8);
    lv_style_set_pad_top(&styles.list_btn, 4);
    lv_style_set_pad_bottom(&styles.list_btn, 4);
    lv_style_set_text_color(&styles.list_btn, COLOR_TEXT);
    // List button when focused
    style_init_reset(&styles.list_btn_focused);
    lv_style_set_radius(&styles.list_btn_focused, RADIUS_DEFAULT);
    lv_style_set_bg_opa(&styles.list_btn_focused, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.list_btn_focused, COLOR_BUTTON_FACE_INACTIVE);
    lv_style_set_text_color(&styles.list_btn_focused, COLOR_BUTTON_TEXT_INACTIVE);
    // List button when pressed
    style_init_reset(&styles.list_btn_pressed);
    lv_style_set_bg_opa(&styles.list_btn_pressed, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.list_btn_pressed, COLOR_BUTTON_FACE_ACTIVE);
    lv_style_set_text_color(&styles.list_btn_pressed, COLOR_BUTTON_TEXT_ACTIVE);

    style_init_reset(&styles.white);
    lv_style_set_bg_opa(&styles.white, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.white, COLOR_WHITE);
    lv_style_set_line_width(&styles.white, 1);
    lv_style_set_line_color(&styles.white, COLOR_WHITE);
    lv_style_set_arc_width(&styles.white, 2);
    lv_style_set_arc_color(&styles.white, COLOR_WHITE);


    style_init_reset(&styles.light);
    lv_style_set_bg_opa(&styles.light, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.light, COLOR_LIGHT);
    lv_style_set_line_width(&styles.light, 1);
    lv_style_set_line_color(&styles.light, COLOR_LIGHT);
    lv_style_set_arc_width(&styles.light, 2);
    lv_style_set_arc_color(&styles.light, COLOR_LIGHT);

    style_init_reset(&styles.dark);
    lv_style_set_bg_opa(&styles.dark, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.dark, COLOR_DARK);
    lv_style_set_line_width(&styles.dark, 1);
    lv_style_set_line_color(&styles.dark, COLOR_DARK);
    lv_style_set_arc_width(&styles.dark, 2);
    lv_style_set_arc_color(&styles.dark, COLOR_DARK);

    style_init_reset(&styles.dim);
    lv_style_set_bg_opa(&styles.dim, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.dim, COLOR_DIM);
    lv_style_set_line_width(&styles.dim, 1);
    lv_style_set_line_color(&styles.dim, COLOR_DIM);
    lv_style_set_arc_width(&styles.dim, 2);
    lv_style_set_arc_color(&styles.dim, COLOR_DIM);

#if LV_USE_TEXTAREA
    style_init_reset(&styles.ta_cursor);
    lv_style_set_border_side(&styles.ta_cursor, LV_BORDER_SIDE_LEFT);
    lv_style_set_border_color(&styles.ta_cursor, COLOR_DIM);
    lv_style_set_border_width(&styles.ta_cursor, 2);
    lv_style_set_bg_opa(&styles.ta_cursor, LV_OPA_TRANSP);
    lv_style_set_anim_time(&styles.ta_cursor, 500);
#endif
}


lv_theme_t* lvt_init(lv_disp_t* disp)
{
    theme.disp = disp;
    theme.font_small = LV_FONT_DEFAULT;
    theme.font_normal = LV_FONT_DEFAULT;
    theme.font_large = LV_FONT_DEFAULT;
    theme.apply_cb = theme_apply;
    style_init();
    if (disp == NULL || lv_disp_get_theme(disp) == &theme)
    {
        lv_obj_report_style_change(NULL);
    }
    return (&theme);
}


static void theme_apply(lv_theme_t *th, lv_obj_t *obj)
{
    if (lv_obj_get_parent(obj) == NULL) // Apply to a screen
    {
        lv_obj_add_style(obj, &styles.scr, 0);
        return;
    }

    if (lv_obj_check_type(obj, &lv_obj_class)) 
    {
        //lv_obj_add_style(obj, &styles.white, 0);
        lv_obj_add_style(obj, &styles.scrollbar, LV_PART_SCROLLBAR);
    }
    // List box
    else if (lv_obj_check_type(obj, &lv_list_class)) 
    {
        lv_obj_add_style(obj, &styles.panel, 0);
        lv_obj_add_style(obj, &styles.scrollbar, LV_PART_SCROLLBAR);
    }
    // Buttons in list box
    else if (lv_obj_check_type(obj, &lv_list_btn_class)) 
    {
        lv_obj_add_style(obj, &styles.list_btn, 0);
        lv_obj_add_style(obj, &styles.list_btn_focused, LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &styles.list_btn_focused, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &styles.list_btn_pressed, LV_STATE_PRESSED);
    }
    else if (lv_obj_check_type(obj, &lv_msgbox_class)) 
    {
        lv_obj_add_style(obj, &styles.panel, 0);
        return;
    }

#if LV_USE_BAR
    else if(lv_obj_check_type(obj, &lv_bar_class)) {
        lv_obj_add_style(obj, &styles.light, 0);
        lv_obj_add_style(obj, &styles.dark, LV_PART_INDICATOR);
    }
#endif

#if LV_USE_SLIDER
    else if(lv_obj_check_type(obj, &lv_slider_class)) {
        lv_obj_add_style(obj, &styles.light, 0);
        lv_obj_add_style(obj, &styles.dark, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &styles.dim, LV_PART_KNOB);
    }
#endif



#if LV_USE_TEXTAREA
    else if(lv_obj_check_type(obj, &lv_textarea_class)) {
        lv_obj_add_style(obj, &styles.white, 0);
        lv_obj_add_style(obj, &styles.scrollbar, LV_PART_SCROLLBAR);
        lv_obj_add_style(obj, &styles.ta_cursor, LV_PART_CURSOR);
    }
#endif


#if LV_USE_SPINBOX
    else if(lv_obj_check_type(obj, &lv_spinbox_class)) {
        lv_obj_add_style(obj, &styles.light, 0);
        lv_obj_add_style(obj, &styles.dark, LV_PART_CURSOR);
    }
#endif
#if LV_USE_TILEVIEW
    else if(lv_obj_check_type(obj, &lv_tileview_class)) {
        lv_obj_add_style(obj, &styles.scr, 0);
        lv_obj_add_style(obj, &styles.scrollbar, LV_PART_SCROLLBAR);
    }
    else if(lv_obj_check_type(obj, &lv_tileview_tile_class)) {
        lv_obj_add_style(obj, &styles.scrollbar, LV_PART_SCROLLBAR);
    }
#endif

#if LV_USE_COLORWHEEL
    else if(lv_obj_check_type(obj, &lv_colorwheel_class)) {
        lv_obj_add_style(obj, &styles.light, 0);
        lv_obj_add_style(obj, &styles.light, LV_PART_KNOB);
    }
#endif

#if LV_USE_LED
    else if(lv_obj_check_type(obj, &lv_led_class)) {
        lv_obj_add_style(obj, &styles.light, 0);
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void style_init_reset(lv_style_t * style)
{
    if(inited) {
        lv_style_reset(style);
    }
    else {
        lv_style_init(style);
    }
}
