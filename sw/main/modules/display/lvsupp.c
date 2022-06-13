#include "lvsupp.h"


lv_obj_t * lv_list_add_btn_ex(lv_obj_t * list, const void * icon, const char * txt, lv_label_long_mode_t mode)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

#if LV_USE_IMG == 1
    if (icon) 
    {
        lv_obj_t * img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }
#endif

    if (txt) 
    {
        lv_obj_t * label = lv_label_create(obj);
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