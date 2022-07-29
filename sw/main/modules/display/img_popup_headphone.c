#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_POPUP_HEADPHONE
#define LV_ATTRIBUTE_IMG_IMG_POPUP_HEADPHONE
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMG_POPUP_HEADPHONE uint8_t img_popup_headphone_map[] = {
  0x00, 0x00, 0x00, 0x01, 0x6b, 0xef, 0xfe, 0xb7, 0x20, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x10, 0x00, 0x00, 
  0x00, 0x00, 0x4f, 0xff, 0xb6, 0x21, 0x12, 0x5a, 0xff, 0xf5, 0x00, 0x00, 
  0x00, 0x06, 0xff, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x2a, 0xff, 0x70, 0x00, 
  0x00, 0x4f, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf5, 0x00, 
  0x00, 0xff, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xff, 0x10, 
  0x09, 0xfb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaf, 0xb0, 
  0x1f, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf1, 
  0x6f, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf7, 
  0xbf, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xfc, 
  0xef, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 
  0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 
  0xff, 0x16, 0xcc, 0x50, 0x00, 0x00, 0x00, 0x00, 0x05, 0xcc, 0x71, 0xef, 
  0xff, 0xdf, 0xff, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x5f, 0xff, 0xfe, 0xef, 
  0xef, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xff, 0xff, 0xfe, 
  0xef, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xdf, 0xff, 0xff, 0xff, 
  0xef, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xff, 0xff, 0xff, 
  0xef, 0xff, 0xff, 0xfd, 0x00, 0x00, 0x00, 0x00, 0xcf, 0xff, 0xff, 0xfe, 
  0x8f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xdf, 0xff, 0xff, 0xf9, 
  0x0e, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0xdf, 0xff, 0xff, 0xf1, 
  0x02, 0xff, 0xff, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x6f, 0xff, 0xff, 0x20, 
  0x00, 0x29, 0xee, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xee, 0xa2, 0x00, 
};

const lv_img_dsc_t img_popup_headphone = {
  .header.cf = LV_IMG_CF_ALPHA_4BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 24,
  .header.h = 24,
  .data_size = 288,
  .data = img_popup_headphone_map,
};
