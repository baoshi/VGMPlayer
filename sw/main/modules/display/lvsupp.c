#include "lvstyle.h"
#include "lvsupp.h"


lv_obj_t * lv_list_add_btn_ex(lv_obj_t * list, const void * icon, const char * txt, lv_label_long_mode_t mode)
{
    lv_obj_t *obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

    if (icon) 
    {
        lv_obj_t *img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }

    if (txt) 
    {
        lv_obj_t *label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, mode);
        lv_obj_set_flex_grow(label, 1);
    }

    return obj;
}


void lv_list_btn_set_label_long_mode(lv_obj_t * btn, lv_label_long_mode_t mode)
{
    uint32_t i;
    for (i = 0; i < lv_obj_get_child_cnt(btn); i++) 
    {
        lv_obj_t * child = lv_obj_get_child(btn, i);
        if(lv_obj_check_type(child, &lv_label_class)) 
        {
            lv_label_set_long_mode(child, mode);
            break;            
        }
    }
}


const lv_obj_class_t lv_barbox_class = 
{
    .base_class = &lv_obj_class,
    .width_def = 200,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_barbox_t)
};


lv_obj_t *lv_barbox_create(lv_obj_t *parent, const void *icon, int32_t min, int32_t max, int32_t value)
{
    lv_obj_t *obj = lv_obj_class_create_obj(&lv_barbox_class, parent);
    LV_ASSERT_MALLOC(obj);
    if (obj == NULL) return NULL;
    lv_obj_class_init_obj(obj);
    lv_obj_add_style(obj, &lvs_msgbox, 0);
    lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(obj, 10, 0);
    if (icon)
    {
        lv_obj_t *img = lv_img_create(obj);
        lv_obj_add_style(img, &lvs_recolored_icon, 0);
        lv_img_set_src(img, icon);
        lv_obj_set_flex_grow(img, 0);
    }
    lv_barbox_t *bbox = (lv_barbox_t *)obj;
    bbox->bar = lv_bar_create(obj);
    lv_obj_set_flex_grow(bbox->bar, 1);
    lv_bar_set_range(bbox->bar, min, max);
    lv_bar_set_value(bbox->bar, value, false);
    lv_obj_center(obj);
    return obj;
}


void lv_barbox_set_value(lv_obj_t *obj, int32_t value)
{
    lv_barbox_t *bbox = (lv_barbox_t *)obj;
    lv_bar_set_value(bbox->bar, value, false);
}


void lv_barbox_close(lv_obj_t *obj)
{
    lv_obj_del(obj);
}


void lv_barbox_close_async(lv_obj_t *obj)
{
    lv_obj_del_async(obj);
}


const lv_obj_class_t lv_sliderbox_class = 
{
    .base_class = &lv_obj_class,
    .width_def = 200,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_sliderbox_t)
};


lv_obj_t *lv_sliderbox_create(lv_obj_t *parent, const void *icon, int32_t min, int32_t max, int32_t value)
{
    lv_obj_t *obj = lv_obj_class_create_obj(&lv_sliderbox_class, parent);
    LV_ASSERT_MALLOC(obj);
    if (obj == NULL) return NULL;
    lv_obj_class_init_obj(obj);
    lv_obj_add_style(obj, &lvs_msgbox, 0);
    lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(obj, 10, 0);
    if (icon)
    {
        lv_obj_t *img = lv_img_create(obj);
        lv_obj_add_style(img, &lvs_recolored_icon, 0);
        lv_img_set_src(img, icon);
        lv_obj_set_flex_grow(img, 0);
    }
    lv_sliderbox_t *sbox = (lv_sliderbox_t *)obj;
    sbox->slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(sbox->slider, 1);
    //lv_obj_set_height(sbox->slider, 10);
    lv_slider_set_range(sbox->slider, min, max);
    lv_slider_set_value(sbox->slider, value, false);
    lv_obj_center(obj);
    return obj;
}


lv_obj_t *lv_sliderbox_get_slider(lv_obj_t *obj)
{
    lv_sliderbox_t *sbox = (lv_sliderbox_t *)obj;
    return sbox->slider;
}


void lv_sliderbox_set_value(lv_obj_t *obj, int32_t value)
{
    lv_sliderbox_t *sbox = (lv_sliderbox_t *)obj;
    lv_slider_set_value(sbox->slider, value, false);
}


void lv_sliderbox_close(lv_obj_t *obj)
{
    lv_obj_del(obj);
}


void lv_sliderbox_close_async(lv_obj_t *obj)
{
    lv_obj_del_async(obj);
}



const lv_obj_class_t lv_alert_class = 
{
    .base_class = &lv_obj_class,
    .width_def = 200,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_alert_t)
};


lv_obj_t *lv_alert_create(lv_obj_t *parent, const void *icon, const char *text)
{
    lv_obj_t *obj = lv_obj_class_create_obj(&lv_alert_class, parent);
    LV_ASSERT_MALLOC(obj);
    if (obj == NULL) return NULL;
    lv_obj_class_init_obj(obj);
    lv_obj_add_style(obj, &lvs_msgbox, 0);
    lv_obj_set_size(obj, LV_PCT(80), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(obj, 10, 0);
    if (icon)
    {
        lv_obj_t *img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }
    if (text)
    {
        lv_obj_t *label = lv_label_create(obj);
        lv_label_set_text(label, text);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_flex_grow(label, 1);
    }
    lv_obj_center(obj);
    return obj;
}


void lv_alert_close(lv_obj_t *obj)
{
    lv_obj_del(obj);
}


void lv_alert_close_async(lv_obj_t *obj)
{
    lv_obj_del_async(obj);
}


//
// Spectrum display
//
#define LV_SPECTRUM_BIN_GAP 1

static void lv_spectrum_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
    lv_obj_remove_style_all(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

}


static void lv_spectrum_destructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, &lv_spectrum_class);
    lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
    lv_mem_free(spectrum->bin_height);
}


static void lv_spectrum_event(const lv_obj_class_t *class_p, lv_event_t *e)
{
    lv_res_t res = lv_obj_event_base(&lv_spectrum_class, e);
    if(res != LV_RES_OK) return;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_REFR_EXT_DRAW_SIZE) 
    {
        
    }
    else if (code == LV_EVENT_COVER_CHECK) 
    {
        lv_event_set_cover_res(e, LV_COVER_RES_NOT_COVER);
    }
    else if (code == LV_EVENT_DRAW_POST) 
    {
        lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
        lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(e);
        lv_opa_t opa = lv_obj_get_style_opa(obj, LV_PART_MAIN);
        if (opa < LV_OPA_MIN) return;
        // for x num of bins, we need x * bin_width + (x - 1) * gap.
        // Calculate bin_width
        int num_bins = spectrum->num_bins;
        int bin_width = ((lv_obj_get_width(obj) - (num_bins - 1) * LV_SPECTRUM_BIN_GAP)) / num_bins;
        int x_start = (lv_obj_get_width(obj) - bin_width * num_bins - (num_bins - 1) * LV_SPECTRUM_BIN_GAP) / 2;
        for (int bin = 0; bin < num_bins; ++bin)
        {
            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_opa = LV_OPA_COVER;
            draw_rect_dsc.bg_color = lv_color_hex(0x0000FF);
            lv_area_t bar_rect;
            bar_rect.x1 = x_start + bin * (bin_width + LV_SPECTRUM_BIN_GAP);
            bar_rect.x2 = bar_rect.x1 + bin_width - 1;
            bar_rect.y2 = obj->coords.y2;
            bar_rect.y1 = obj->coords.y2 - spectrum->bin_height[bin];
            lv_draw_rect(draw_ctx, &draw_rect_dsc, &bar_rect);
        }
    }
}


const lv_obj_class_t lv_spectrum_class = 
{
    .constructor_cb = lv_spectrum_constructor,
    .destructor_cb = lv_spectrum_destructor,
    .event_cb = lv_spectrum_event,
    .instance_size = sizeof(lv_spectrum_t),
    .base_class = &lv_obj_class
};


lv_obj_t * lv_spectrum_create(lv_obj_t *parent, int num_bins)
{
    lv_obj_t *obj = lv_obj_class_create_obj(&lv_spectrum_class, parent);
    LV_ASSERT_MALLOC(obj);
    lv_obj_class_init_obj(obj);
    lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
    spectrum->num_bins = num_bins;
    spectrum->bin_height = lv_mem_alloc(num_bins * sizeof(uint8_t));
    LV_ASSERT_MALLOC(spectrum->bin_height);
    lv_memset_00(spectrum->bin_height, num_bins * sizeof(uint8_t));
    return obj;
}


void lv_spectrum_set_bin_values(lv_obj_t *obj, const uint8_t *bvs)
{
    LV_ASSERT_OBJ(obj, &lv_spectrum_class);
    lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
    memcpy(spectrum->bin_height, bvs, spectrum->num_bins * sizeof(uint8_t));
    lv_obj_invalidate(obj);
}