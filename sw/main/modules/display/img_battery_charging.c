#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_BATTERY_CHARGING
#define LV_ATTRIBUTE_IMG_IMG_BATTERY_CHARGING
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMG_BATTERY_CHARGING uint8_t img_battery_charging_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0xe3, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0x96, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xe3, 0xe3, 0xe3, 0xe3, 
  0xb6, 0xb6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x49, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb6, 0xb6, 0xe3, 0xe3, 0xe3, 
  0xb6, 0x00, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x35, 0x28, 0x39, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0x00, 0xb6, 0xe3, 0xe3, 0xe3, 
  0xb6, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x39, 0x24, 0x2c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0xb6, 0xe3, 0xb6, 0xe3, 
  0xb6, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x3d, 0x28, 0x04, 0x39, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0xb6, 0xe3, 0xe3, 0xb6, 
  0xb6, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x3d, 0x2d, 0x00, 0x04, 0x35, 0x39, 0x35, 0x39, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0xb6, 0xe3, 0xe3, 0xb6, 
  0xb6, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x38, 0x2c, 0x00, 0x00, 0x00, 0x04, 0x00, 0x28, 0x39, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0xb6, 0xe3, 0xe3, 0xb6, 
  0xb6, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x39, 0x2c, 0x2c, 0x2c, 0x04, 0x00, 0x24, 0x59, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0xb6, 0xe3, 0xe3, 0xb6, 
  0xb6, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x3c, 0x1c, 0x1c, 0x3d, 0x28, 0x04, 0x55, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0xb6, 0xe3, 0xe3, 0xb6, 
  0xb6, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x35, 0x00, 0x31, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0xb6, 0xe3, 0xb6, 0xe3, 
  0xb6, 0x00, 0x00, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x3c, 0x28, 0x2d, 0x3c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00, 0x00, 0xb6, 0xe3, 0xe3, 0xe3, 
  0xb6, 0xb6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x29, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb6, 0xb6, 0xe3, 0xe3, 0xe3, 
  0xe3, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0x96, 0x96, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xb6, 0xe3, 0xe3, 0xe3, 0xe3, 
  0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1f, 0xf8, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf3, 0x9c, 0xb2, 0x94, 0xf3, 0x9c, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0xf4, 0xa4, 0xf4, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa2, 0x10, 0xe7, 0x39, 0xa6, 0x31, 0x61, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xa4, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0xf4, 0xa4, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe1, 0x0f, 0x45, 0x2d, 0x23, 0x1a, 0x44, 0x26, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0xf4, 0xa4, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x45, 0x2e, 0x83, 0x19, 0x64, 0x23, 0xc1, 0x0f, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0xf4, 0xa4, 0x1f, 0xf8, 
  0xf4, 0xa4, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x06, 0x37, 0x03, 0x1a, 0xc1, 0x08, 0xe5, 0x2d, 0xe0, 0x07, 0xe0, 0x07, 0xc1, 0x0f, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0xf4, 0xa4, 
  0xf4, 0xa4, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xc1, 0x0f, 0x05, 0x2f, 0x26, 0x33, 0x00, 0x00, 0x21, 0x09, 0x85, 0x2c, 0xc5, 0x2d, 0x44, 0x25, 0x25, 0x2e, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0xf4, 0xa4, 
  0xf4, 0xa4, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x44, 0x26, 0xc4, 0x22, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x81, 0x08, 0x40, 0x00, 0xc3, 0x19, 0xa5, 0x2d, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0xf4, 0xa4, 
  0xf4, 0xa4, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x86, 0x35, 0x44, 0x23, 0x23, 0x1b, 0x24, 0x23, 0x21, 0x09, 0x00, 0x00, 0x42, 0x11, 0xa6, 0x35, 0xa1, 0x0f, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0xf4, 0xa4, 
  0xf4, 0xa4, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x82, 0x17, 0xa2, 0x17, 0xe1, 0x0f, 0xa4, 0x26, 0x03, 0x1a, 0x81, 0x08, 0x66, 0x35, 0xe1, 0x0f, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0xf4, 0xa4, 
  0xf4, 0xa4, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x45, 0x2d, 0x81, 0x08, 0x46, 0x34, 0xe2, 0x17, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0xf4, 0xa4, 0x1f, 0xf8, 
  0xf4, 0xa4, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x23, 0x1f, 0x83, 0x1a, 0x25, 0x2b, 0x83, 0x1f, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0xf4, 0xa4, 0xf4, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x08, 0x85, 0x29, 0xc6, 0x31, 0x65, 0x29, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xa4, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1f, 0xf8, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf3, 0x9c, 0x91, 0x8c, 0xb2, 0x94, 0xf3, 0x9c, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit BUT the 2 bytes are swapped*/
  0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xf8, 0x1f, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0x9c, 0xf3, 0x94, 0xb2, 0x9c, 0xf3, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xa4, 0xf4, 0xa4, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xa2, 0x39, 0xe7, 0x31, 0xa6, 0x08, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0xf4, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xa4, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x0f, 0xe1, 0x2d, 0x45, 0x1a, 0x23, 0x26, 0x44, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xa4, 0xf4, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x2e, 0x45, 0x19, 0x83, 0x23, 0x64, 0x0f, 0xc1, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xa4, 0xf4, 0xf8, 0x1f, 
  0xa4, 0xf4, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x37, 0x06, 0x1a, 0x03, 0x08, 0xc1, 0x2d, 0xe5, 0x07, 0xe0, 0x07, 0xe0, 0x0f, 0xc1, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xa4, 0xf4, 
  0xa4, 0xf4, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x0f, 0xc1, 0x2f, 0x05, 0x33, 0x26, 0x00, 0x00, 0x09, 0x21, 0x2c, 0x85, 0x2d, 0xc5, 0x25, 0x44, 0x2e, 0x25, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xa4, 0xf4, 
  0xa4, 0xf4, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x26, 0x44, 0x22, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x08, 0x81, 0x00, 0x40, 0x19, 0xc3, 0x2d, 0xa5, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xa4, 0xf4, 
  0xa4, 0xf4, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x35, 0x86, 0x23, 0x44, 0x1b, 0x23, 0x23, 0x24, 0x09, 0x21, 0x00, 0x00, 0x11, 0x42, 0x35, 0xa6, 0x0f, 0xa1, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xa4, 0xf4, 
  0xa4, 0xf4, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x17, 0x82, 0x17, 0xa2, 0x0f, 0xe1, 0x26, 0xa4, 0x1a, 0x03, 0x08, 0x81, 0x35, 0x66, 0x0f, 0xe1, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xa4, 0xf4, 
  0xa4, 0xf4, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x2d, 0x45, 0x08, 0x81, 0x34, 0x46, 0x17, 0xe2, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xa4, 0xf4, 0xf8, 0x1f, 
  0xa4, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x1f, 0x23, 0x1a, 0x83, 0x2b, 0x25, 0x1f, 0x83, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xa4, 0xf4, 0xa4, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x41, 0x29, 0x85, 0x31, 0xc6, 0x29, 0x65, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0xf4, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xf8, 0x1f, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0x9c, 0xf3, 0x8c, 0x91, 0x94, 0xb2, 0x9c, 0xf3, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xa4, 0xf4, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
#endif
#if LV_COLOR_DEPTH == 32
  /*Pixel format: Fix 0xFF: 8 bit, Red: 8 bit, Green: 8 bit, Blue: 8 bit*/
  0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9a, 0x9c, 0x9a, 0xff, 0x8e, 0x93, 0x8e, 0xff, 0x9a, 0x9d, 0x9a, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x12, 0x13, 0x12, 0xff, 0x36, 0x3b, 0x36, 0xff, 0x2c, 0x35, 0x2c, 0xff, 0x0a, 0x0c, 0x0a, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x05, 0xfc, 0x05, 0xff, 0x2b, 0xa8, 0x2b, 0xff, 0x1b, 0x43, 0x1b, 0xff, 0x21, 0xc7, 0x21, 0xff, 0x02, 0xfe, 0x02, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x2b, 0xc8, 0x2b, 0xff, 0x14, 0x2e, 0x14, 0xff, 0x1d, 0x6b, 0x1d, 0xff, 0x09, 0xf6, 0x09, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x2c, 0xe0, 0x2c, 0xff, 0x1a, 0x40, 0x1a, 0xff, 0x09, 0x18, 0x09, 0xff, 0x25, 0xbd, 0x25, 0xff, 0x00, 0xff, 0x00, 0xff, 0x03, 0xfc, 0x03, 0xff, 0x08, 0xf7, 0x08, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x05, 0xf7, 0x05, 0xff, 0x24, 0xdf, 0x24, 0xff, 0x2c, 0x65, 0x2c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x0b, 0x23, 0x0b, 0xff, 0x24, 0x90, 0x24, 0xff, 0x24, 0xb8, 0x24, 0xff, 0x22, 0xa8, 0x22, 0xff, 0x26, 0xc5, 0x26, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x1f, 0xc6, 0x1f, 0xff, 0x1e, 0x58, 0x1e, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff, 0x03, 0x08, 0x03, 0xff, 0x05, 0x10, 0x05, 0xff, 0x03, 0x07, 0x03, 0xff, 0x15, 0x37, 0x15, 0xff, 0x28, 0xb2, 0x28, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x2d, 0xb0, 0x2d, 0xff, 0x1e, 0x66, 0x1e, 0xff, 0x1a, 0x65, 0x1a, 0xff, 0x1d, 0x62, 0x1d, 0xff, 0x0b, 0x22, 0x0b, 0xff, 0x00, 0x00, 0x00, 0xff, 0x13, 0x27, 0x13, 0xff, 0x30, 0xb3, 0x30, 0xff, 0x0b, 0xf2, 0x0b, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x10, 0xef, 0x10, 0xff, 0x0d, 0xf3, 0x0d, 0xff, 0x06, 0xfb, 0x06, 0xff, 0x22, 0xd2, 0x22, 0xff, 0x14, 0x40, 0x14, 0xff, 0x07, 0x10, 0x07, 0xff, 0x33, 0xab, 0x33, 0xff, 0x08, 0xfc, 0x08, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x01, 0xfe, 0x01, 0xff, 0x25, 0xa6, 0x25, 0xff, 0x06, 0x0f, 0x06, 0xff, 0x2d, 0x88, 0x2d, 0xff, 0x0e, 0xfa, 0x0e, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x17, 0xe4, 0x17, 0xff, 0x18, 0x4f, 0x18, 0xff, 0x26, 0x62, 0x26, 0xff, 0x18, 0xf1, 0x18, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0xff, 0x06, 0x07, 0x06, 0xff, 0x27, 0x2f, 0x27, 0xff, 0x2e, 0x36, 0x2e, 0xff, 0x27, 0x2c, 0x27, 0xff, 0x02, 0x02, 0x02, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9a, 0x9d, 0x9a, 0xff, 0x8a, 0x91, 0x8a, 0xff, 0x8f, 0x93, 0x8f, 0xff, 0x9b, 0x9c, 0x9b, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0x9c, 0x9c, 0x9c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
#endif
};

const lv_img_dsc_t img_battery_charging = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 26,
  .header.h = 19,
  .data_size = 494 * LV_COLOR_SIZE / 8,
  .data = img_battery_charging_map,
};
