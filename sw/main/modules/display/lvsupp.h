#pragma once
// Supplementary LVGL functions

#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif


lv_obj_t *lv_list_add_btn_ex(lv_obj_t *list, const void *icon, const char *txt, lv_label_long_mode_t mode);
void lv_list_btn_set_label_long_mode(lv_obj_t *btn, lv_label_long_mode_t mode);


//
// Bar box
//

typedef struct lv_barbox_s
{
    lv_obj_t obj;
    lv_obj_t *bar;
} lv_barbox_t;

extern const lv_obj_class_t lv_barbox_class;

lv_obj_t *lv_barbox_create(lv_obj_t *parent, const void *icon, int32_t min, int32_t max, int32_t value);
void lv_barbox_set_value(lv_obj_t *obj, int32_t value);
void lv_barbox_close(lv_obj_t *obj);
void lv_barbox_close_async(lv_obj_t *obj);


//
// Slider box
//

typedef struct lv_sliderbox_s
{
    lv_obj_t obj;
    lv_obj_t *slider;
} lv_sliderbox_t;

extern const lv_obj_class_t lv_sliderbox_class;

lv_obj_t *lv_sliderbox_create(lv_obj_t *parent, const void *icon, int32_t min, int32_t max, int32_t value);
lv_obj_t *lv_sliderbox_get_slider(lv_obj_t *obj);
void lv_sliderbox_set_value(lv_obj_t *obj, int32_t value);
void lv_sliderbox_close(lv_obj_t *obj);
void lv_sliderbox_close_async(lv_obj_t *obj);


 //
// Alert box
//

typedef struct lv_alert_s
{
    lv_obj_t obj;
} lv_alert_t;

extern const lv_obj_class_t lv_alert_class;

lv_obj_t *lv_alert_create(lv_obj_t *parent, const void *icon, const char *text);
void lv_alert_close(lv_obj_t *obj);
void lv_alert_close_async(lv_obj_t *obj);


#ifdef __cplusplus
}
#endif
