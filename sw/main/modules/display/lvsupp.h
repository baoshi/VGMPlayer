#pragma once
// Supplementary LVGL functions

#include <lvgl.h>


#ifdef __cplusplus
extern "C" {
#endif


lv_obj_t * lv_list_add_btn_ex(lv_obj_t * list, const void * icon, const char * txt, lv_label_long_mode_t mode);
void lv_list_btn_set_label_long_mode(lv_obj_t * btn, lv_label_long_mode_t mode);


#ifdef __cplusplus
}
#endif
