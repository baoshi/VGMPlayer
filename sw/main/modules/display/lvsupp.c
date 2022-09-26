#include <stdio.h>
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


const lv_obj_class_t lv_sliderbox_class = 
{
    .base_class = &lv_obj_class,
    .width_def = 200,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_sliderbox_t)
};


lv_obj_t *lv_sliderbox_create(lv_obj_t *parent, const void *icon, int32_t min, int32_t max, int32_t value, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
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
    lv_obj_align(obj, align, x_ofs, y_ofs);
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


lv_obj_t *lv_alert_create(lv_obj_t *parent, const void *icon, const char *text, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
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
    lv_obj_align(obj, align, x_ofs, y_ofs);
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

