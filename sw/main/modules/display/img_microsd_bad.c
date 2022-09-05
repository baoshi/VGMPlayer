#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_MICROSD_BAD
#define LV_ATTRIBUTE_IMG_IMG_MICROSD_BAD
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMG_MICROSD_BAD uint8_t img_microsd_bad_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0xe3, 0x17, 0x17, 0x17, 0x17, 0xe3, 0xe3, 0xe3, 0xe3, 0x17, 0x17, 0x17, 0x17, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x17, 0xe3, 0xe3, 0xe3, 0x17, 0x1b, 0x17, 0x17, 0x17, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x17, 0xe3, 0xe3, 0x17, 0x1b, 0x1b, 0x17, 0x17, 0x17, 0xe3, 0xe3, 0xe3, 0xe3, 0xe3, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x1b, 0x17, 0xe3, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x17, 0x13, 0x0e, 0x0e, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x00, 0x00, 0x00, 0x17, 0x17, 
  0x17, 0x17, 0x1b, 0x1b, 0x17, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x1b, 0x17, 0x1b, 0x1b, 0x00, 0x00, 0x00, 0x17, 0x17, 
  0x17, 0x17, 0x1b, 0x1b, 0x0e, 0x00, 0x05, 0x0a, 0x05, 0x00, 0x00, 0x17, 0x17, 0x1b, 0x1b, 0x17, 0x17, 0x17, 0x17, 0x17, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x0e, 0x17, 0x1b, 0x0e, 0x00, 0x00, 0x17, 0x1b, 0x1b, 0x1b, 0x00, 0x00, 0x00, 0x17, 0x17, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x0e, 0x00, 0x00, 0x09, 0x17, 0x1b, 0x1b, 0x1b, 0x00, 0x00, 0x00, 0x17, 0x1b, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x0e, 0x00, 0x00, 0x09, 0x17, 0x17, 0x17, 0x1b, 0x1b, 0x17, 0x17, 0x17, 0x17, 0x1b, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x0a, 0x00, 0x00, 0x17, 0x1b, 0x1b, 0x17, 0x1b, 0x1b, 0x00, 0x00, 0x00, 0x17, 0x17, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x17, 0x0e, 0x13, 0x17, 0x17, 0x17, 0x17, 0x1b, 0x1b, 0x00, 0x00, 0x00, 0x17, 0x17, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x09, 0x00, 0x05, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x17, 0x00, 0x00, 0x00, 0x17, 0x17, 0x17, 0x1b, 0x1b, 0x17, 0x00, 0x00, 0x00, 0x17, 0x1b, 
  0x17, 0x17, 0x1b, 0x1b, 0x1b, 0x1b, 0x13, 0x0a, 0x0e, 0x17, 0x1b, 0x1b, 0x17, 0x1b, 0x17, 0x00, 0x00, 0x00, 0x17, 0x17, 
  0x37, 0x17, 0x1b, 0x17, 0x17, 0x17, 0x17, 0x1b, 0x1b, 0x17, 0x1b, 0x1b, 0x17, 0x17, 0x1b, 0x17, 0x17, 0x17, 0x17, 0x1b, 
  0xe3, 0x32, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0xe3, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x1f, 0xf8, 0xfa, 0x14, 0x3b, 0x0d, 0x1b, 0x0d, 0x1b, 0x15, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1b, 0x15, 0x1b, 0x0d, 0x1b, 0x0d, 0x1b, 0x0d, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x1b, 0x15, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1b, 0x0d, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x1b, 0x15, 0x1f, 0xf8, 0x1f, 0xf8, 0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x7e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x1f, 0xf8, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x5d, 0x05, 0x16, 0x04, 0x93, 0x03, 0x93, 0x03, 0x99, 0x04, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x05, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0xfa, 0x04, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x03, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x05, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0xf0, 0x02, 0x00, 0x00, 0xc4, 0x00, 0x2c, 0x02, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x93, 0x03, 0x5d, 0x05, 0x9e, 0x05, 0x52, 0x03, 0x00, 0x00, 0x00, 0x00, 0xb9, 0x04, 0x9e, 0x05, 0x9e, 0x05, 0xbe, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x0d, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x93, 0x03, 0x00, 0x00, 0x00, 0x00, 0xca, 0x01, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x0d, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x93, 0x03, 0x00, 0x00, 0x00, 0x00, 0xca, 0x01, 0x5d, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x2c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x5d, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x05, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0xb9, 0x04, 0x93, 0x03, 0x57, 0x04, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x05, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0xca, 0x01, 0x00, 0x00, 0xc4, 0x00, 0x5d, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 
  0x1b, 0x15, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x5d, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xda, 0x04, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x0d, 
  0x1b, 0x0d, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x16, 0x04, 0x2c, 0x02, 0x93, 0x03, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x05, 0x9e, 0x05, 
  0xd9, 0x24, 0x7e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0xbe, 0x0d, 
  0x1f, 0xf8, 0xb3, 0x13, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xf5, 0x0b, 0xf5, 0x0b, 0xf5, 0x0b, 0xd4, 0x0b, 0x1f, 0xf8, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit BUT the 2 bytes are swapped*/
  0xf8, 0x1f, 0x14, 0xfa, 0x0d, 0x3b, 0x0d, 0x1b, 0x15, 0x1b, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0x15, 0x1b, 0x0d, 0x1b, 0x0d, 0x1b, 0x0d, 0x1b, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x15, 0x1b, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0x0d, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x15, 0x1b, 0xf8, 0x1f, 0xf8, 0x1f, 0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x7e, 0x05, 0x9e, 0x05, 0x9e, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 0xf8, 0x1f, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0xf8, 0x1f, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x5d, 0x04, 0x16, 0x03, 0x93, 0x03, 0x93, 0x04, 0x99, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x05, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x04, 0xfa, 0x00, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x52, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x05, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x02, 0xf0, 0x00, 0x00, 0x00, 0xc4, 0x02, 0x2c, 0x00, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x05, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x03, 0x93, 0x05, 0x5d, 0x05, 0x9e, 0x03, 0x52, 0x00, 0x00, 0x00, 0x00, 0x04, 0xb9, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x0d, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x03, 0x93, 0x00, 0x00, 0x00, 0x00, 0x01, 0xca, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x0d, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x03, 0x93, 0x00, 0x00, 0x00, 0x00, 0x01, 0xca, 0x05, 0x5d, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x02, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x05, 0x5d, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x05, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x04, 0xb9, 0x03, 0x93, 0x04, 0x57, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x05, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x01, 0xca, 0x00, 0x00, 0x00, 0xc4, 0x05, 0x5d, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 
  0x15, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xda, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x0d, 0x9e, 
  0x0d, 0x1b, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x04, 0x16, 0x02, 0x2c, 0x03, 0x93, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9e, 0x05, 0x9e, 
  0x24, 0xd9, 0x05, 0x7e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x05, 0x9e, 0x0d, 0xbe, 
  0xf8, 0x1f, 0x13, 0xb3, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xd4, 0x0b, 0xf5, 0x0b, 0xf5, 0x0b, 0xf5, 0x0b, 0xd4, 0xf8, 0x1f, 
#endif
#if LV_COLOR_DEPTH == 32
  /*Pixel format: Fix 0xFF: 8 bit, Red: 8 bit, Green: 8 bit, Blue: 8 bit*/
  0xff, 0x00, 0xff, 0xff, 0xd1, 0x9a, 0x0c, 0xff, 0xdb, 0xa3, 0x05, 0xff, 0xd9, 0xa1, 0x06, 0xff, 0xd5, 0x9f, 0x0c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xd5, 0x9f, 0x0c, 0xff, 0xd9, 0xa1, 0x06, 0xff, 0xd9, 0xa1, 0x06, 0xff, 0xd8, 0xa0, 0x07, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xd6, 0xa0, 0x0f, 0xff, 0xf0, 0xae, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xd5, 0x9f, 0x0c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xef, 0xaf, 0x00, 0xff, 0xee, 0xaf, 0x01, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xd6, 0xa0, 0x0b, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xd5, 0x9f, 0x0c, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xef, 0xad, 0x00, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x01, 0xff, 0xef, 0xb0, 0x02, 0xff, 0xef, 0xb0, 0x02, 0xff, 0xef, 0xb0, 0x02, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xf0, 0xb0, 0x02, 0xff, 0xf0, 0xae, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xe6, 0xa9, 0x00, 0xff, 0xb1, 0x81, 0x00, 0xff, 0x97, 0x6e, 0x01, 0xff, 0x98, 0x6e, 0x00, 0xff, 0xc6, 0x91, 0x00, 0xff, 0xef, 0xaf, 0x00, 0xff, 0xef, 0xaf, 0x00, 0xff, 0xed, 0xaf, 0x03, 0xff, 0xef, 0xae, 0x00, 0xff, 0xef, 0xae, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xaf, 0x02, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xd2, 0x9a, 0x00, 0xff, 0x22, 0x16, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x8c, 0x66, 0x00, 0xff, 0xf0, 0xb0, 0x01, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb1, 0x05, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xef, 0xaf, 0x01, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0x7f, 0x5b, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x22, 0x16, 0x00, 0xff, 0x60, 0x44, 0x00, 0xff, 0x21, 0x15, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xda, 0xa0, 0x02, 0xff, 0xee, 0xaf, 0x01, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xaf, 0x02, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0x99, 0x6f, 0x00, 0xff, 0xe6, 0xa9, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0x8c, 0x66, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xc7, 0x92, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb2, 0x0a, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xaf, 0x04, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0x97, 0x6e, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x4e, 0x37, 0x01, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xef, 0xb0, 0x02, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb1, 0x05, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xb0, 0x06, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0x99, 0x6f, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x4e, 0x37, 0x00, 0xff, 0xe5, 0xa8, 0x00, 0xff, 0xef, 0xaf, 0x00, 0xff, 0xed, 0xaf, 0x02, 0xff, 0xef, 0xb0, 0x00, 0xff, 0xf0, 0xb1, 0x06, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xb0, 0x04, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0x60, 0x44, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xe5, 0xa9, 0x02, 0xff, 0xef, 0xb0, 0x02, 0xff, 0xef, 0xb0, 0x02, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x03, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xef, 0xae, 0x01, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xc7, 0x92, 0x00, 0xff, 0x97, 0x6e, 0x00, 0xff, 0xbb, 0x89, 0x01, 0xff, 0xee, 0xaf, 0x01, 0xff, 0xee, 0xaf, 0x01, 0xff, 0xee, 0xaf, 0x01, 0xff, 0xef, 0xaf, 0x01, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xaf, 0x02, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0x4f, 0x37, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x22, 0x16, 0x00, 0xff, 0xe6, 0xa9, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xaf, 0x03, 0xff, 
  0xd5, 0x9f, 0x0c, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xe6, 0xa9, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xd0, 0x99, 0x02, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xee, 0xb0, 0x03, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xb0, 0x04, 0xff, 
  0xd7, 0xa0, 0x09, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xb1, 0x81, 0x00, 0xff, 0x60, 0x44, 0x00, 0xff, 0x99, 0x6f, 0x00, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xf0, 0xb0, 0x01, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xee, 0xaf, 0x02, 0xff, 0xef, 0xb0, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf0, 0xaf, 0x03, 0xff, 
  0xc5, 0x98, 0x1e, 0xff, 0xef, 0xad, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xef, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xf0, 0xb0, 0x00, 0xff, 0xef, 0xaf, 0x00, 0xff, 0xef, 0xaf, 0x00, 0xff, 0xf0, 0xb0, 0x01, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xee, 0xaf, 0x03, 0xff, 0xf1, 0xb2, 0x09, 0xff, 
  0xff, 0x00, 0xff, 0xff, 0x97, 0x72, 0x10, 0xff, 0xa3, 0x79, 0x07, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa1, 0x78, 0x08, 0xff, 0xa4, 0x7b, 0x08, 0xff, 0xa6, 0x7c, 0x08, 0xff, 0xa7, 0x7c, 0x08, 0xff, 0xa3, 0x79, 0x0b, 0xff, 0xff, 0x00, 0xff, 0xff, 
#endif
};

const lv_img_dsc_t img_microsd_bad = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 20,
  .header.h = 17,
  .data_size = 340 * LV_COLOR_SIZE / 8,
  .data = img_microsd_bad_map,
};
