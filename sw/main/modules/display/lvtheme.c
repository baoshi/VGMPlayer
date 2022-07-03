#include "lvstyle.h"
#include "lvtheme.h"


/*********************
 *      DEFINES
 *********************/



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
    lv_style_t white;
    lv_style_t light;
    lv_style_t dark;
    lv_style_t dim;
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
    if (lv_obj_get_parent(obj) == NULL) // Apply to screen
    {
        lv_obj_add_style(obj, &lvs_default, 0);
        return;
    }
    // Class based styles
    if (lv_obj_check_type(obj, &lv_obj_class))              // Root object
    {
        lv_obj_add_style(obj, &lvs_scrollbar, LV_PART_SCROLLBAR);
    }
    else if (lv_obj_check_type(obj, &lv_list_class))        // List box
    {
        lv_obj_add_style(obj, &lvs_list, 0);
        lv_obj_add_style(obj, &lvs_scrollbar, LV_PART_SCROLLBAR);
    }
    else if (lv_obj_check_type(obj, &lv_list_btn_class))     // Buttons in list box
    {
        lv_obj_add_style(obj, &lvs_list_btn, 0);
        lv_obj_add_style(obj, &lvs_list_btn_focused, LV_STATE_FOCUSED);
        lv_obj_add_style(obj, &lvs_list_btn_focused, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(obj, &lvs_list_btn_pressed, LV_STATE_PRESSED);
    }
    else if (lv_obj_check_type(obj, &lv_msgbox_class))      // Message box
    {
        lv_obj_add_style(obj, &lvs_msgbox, 0);
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
        lv_obj_add_style(obj, &lvs_scrollbar, LV_PART_SCROLLBAR);
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
