#include "lvsupp.h"
#include "lvtheme.h"


/*********************
 *      DEFINES
 *********************/
#define COLOR_BACKGROUND            lv_color_make(0, 0, 0)
#define COLOR_TEXT                  lv_color_make(225, 217, 210)
#define COLOR_BORDER                lv_color_make(60, 169, 227)
#define COLOR_BUTTON_FACE_INACTIVE  lv_color_make(0, 0, 0)
#define COLOR_BUTTON_TEXT_INACTIVE  lv_color_make(225, 217, 210)
#define COLOR_BUTTON_FACE_FOCUSED   lv_color_make(44, 102, 132)
#define COLOR_BUTTON_TEXT_FOCUSED   lv_color_make(225, 217, 210)
#define COLOR_BUTTON_FACE_PRESSED   lv_color_make(61, 174, 233)
#define COLOR_BUTTON_TEXT_PRESSED   lv_color_make(225, 217, 210)


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
    lv_style_t listbox, list_btn, list_btn_focused, list_btn_pressed;
    lv_style_t popup;
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
    lv_style_set_bg_color(&styles.scrollbar, COLOR_BUTTON_FACE_FOCUSED);
    lv_style_set_bg_opa(&styles.scrollbar, LV_OPA_COVER);
    lv_style_set_pad_right(&styles.scrollbar, 4);
    lv_style_set_pad_top(&styles.scrollbar,  4);
    lv_style_set_pad_bottom(&styles.scrollbar,  4);
    lv_style_set_size(&styles.scrollbar,  5);
    
    // Listbox
    style_init_reset(&styles.listbox);
    lv_style_set_bg_opa(&styles.listbox, LV_OPA_COVER);   // Same opqaue background as screen
    lv_style_set_bg_color(&styles.listbox, COLOR_BACKGROUND);
    lv_style_set_border_color(&styles.listbox, COLOR_BORDER);  // 1 pixel border
    lv_style_set_border_side(&styles.listbox, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_width(&styles.listbox, 1);
    lv_style_set_border_post(&styles.listbox, true);      // border shall draw after children
    lv_style_set_pad_right(&styles.listbox, 14);          // right with scrollbar
    lv_style_set_pad_left(&styles.listbox, 8);
    lv_style_set_pad_top(&styles.listbox, 6);
    lv_style_set_pad_bottom(&styles.listbox, 6);
    lv_style_set_pad_row(&styles.listbox, 1);             // 1 pixels gap between each row
    // Default list button
    style_init_reset(&styles.list_btn);
    lv_style_set_pad_left(&styles.list_btn, 8);
    lv_style_set_pad_top(&styles.list_btn, 4);
    lv_style_set_pad_bottom(&styles.list_btn, 4);
    lv_style_set_bg_opa(&styles.list_btn_focused, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.list_btn_focused, COLOR_BUTTON_FACE_INACTIVE);
    lv_style_set_text_color(&styles.list_btn_focused, COLOR_BUTTON_TEXT_INACTIVE);
    // List button when focused
    style_init_reset(&styles.list_btn_focused);
    lv_style_set_bg_opa(&styles.list_btn_focused, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.list_btn_focused, COLOR_BUTTON_FACE_FOCUSED);
    lv_style_set_text_color(&styles.list_btn_focused, COLOR_BUTTON_TEXT_FOCUSED);
    // List button when pressed
    style_init_reset(&styles.list_btn_pressed);
    lv_style_set_bg_opa(&styles.list_btn_pressed, LV_OPA_COVER);
    lv_style_set_bg_color(&styles.list_btn_pressed, COLOR_BUTTON_FACE_PRESSED);
    lv_style_set_text_color(&styles.list_btn_pressed, COLOR_BUTTON_TEXT_PRESSED);

    // Popup Box
    style_init_reset(&styles.popup);
    lv_style_set_radius(&styles.popup, RADIUS_DEFAULT);        // Round rectangle
    lv_style_set_bg_opa(&styles.popup, LV_OPA_COVER);          // Same opqaue background as screen
    lv_style_set_bg_color(&styles.popup, COLOR_BACKGROUND);    // Colors
    lv_style_set_text_color(&styles.popup, COLOR_TEXT);
    lv_style_set_border_color(&styles.popup, COLOR_BORDER);    // 2 pixel border
    lv_style_set_border_width(&styles.popup, 2);
    lv_style_set_border_post(&styles.popup, true);             // border shall draw after children
    lv_style_set_pad_all(&styles.popup, 10);                   // Leave some space from border
    lv_style_set_clip_corner(&styles.popup, true);             // clip the overflowed content on the rounded corner

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
        lv_obj_add_style(obj, &styles.listbox, 0);
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
    // Message box
    else if (lv_obj_check_type(obj, &lv_msgbox_class)) 
    {
        lv_obj_add_style(obj, &styles.popup, 0);
    }
    // Bar box
    else if (lv_obj_check_type(obj, &lv_barbox_class)) 
    {
        lv_obj_add_style(obj, &styles.popup, 0);
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
