#pragma once
// Supplementary LVGL functions

#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif


lv_obj_t *lv_list_add_btn_ex(lv_obj_t *list, const void *icon, const char *txt, lv_label_long_mode_t mode);
void lv_list_btn_set_label_long_mode(lv_obj_t *btn, lv_label_long_mode_t mode);


typedef struct 
{
    lv_obj_t obj;
    lv_obj_t *icon;
    lv_obj_t *bar;
} lv_barbox_t;

extern const lv_obj_class_t lv_barbox_class;

lv_obj_t *lv_barbox_create(lv_obj_t *parent, int32_t min, int32_t max, int32_t value);
void lv_barbox_close(lv_obj_t * bbox);
void lv_barbox_close_async(lv_obj_t * bbox);


#ifdef __cplusplus
}
#endif
