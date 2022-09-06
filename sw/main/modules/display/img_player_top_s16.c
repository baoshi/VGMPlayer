#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_IMG_PLAYER_TOP_S16
#define LV_ATTRIBUTE_IMG_IMG_PLAYER_TOP_S16
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_IMG_PLAYER_TOP_S16 uint8_t img_player_top_s16_map[] = {
#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8
  /*Pixel format: Red: 3 bit, Green: 3 bit, Blue: 2 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb1, 0xf5, 0x69, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xf5, 0xf5, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x69, 0xf5, 0xad, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xad, 0xf1, 0x48, 0xf1, 0xb1, 0x69, 0x44, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd1, 0xac, 0x00, 0xd1, 0xf1, 0xf1, 0xf1, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xf1, 0x8d, 0x00, 0x00, 0x24, 0x44, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x44, 0x88, 0x88, 0x88, 0x68, 0x45, 0x00, 0x00, 0x24, 0xf1, 0x44, 0x00, 0x00, 0x73, 0xbb, 0xbb, 0xbb, 0x4e, 0x00, 0x00, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x00, 0x73, 0xbb, 0xbb, 0x72, 0x00, 
  0xd1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0x24, 0x00, 0x69, 0xf1, 0x44, 0x00, 0x4e, 0x9b, 0x25, 0x00, 0x25, 0xbb, 0x49, 0x00, 0x25, 0x97, 0xbb, 0x4e, 0x00, 0x00, 0x4e, 0x97, 0x00, 0x25, 0xbb, 0x25, 
  0x00, 0x00, 0x00, 0x00, 0x45, 0xf1, 0x89, 0x00, 0xad, 0xf1, 0x00, 0x00, 0x6e, 0x97, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x4e, 0x97, 0x4e, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0xcd, 0x00, 0xcd, 0xcd, 0x00, 0x00, 0x25, 0x97, 0xbb, 0x73, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x9b, 0x97, 0xbb, 0xbb, 0x72, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xad, 0xe8, 0x20, 0xed, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x97, 0xbb, 0x25, 0x00, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x9b, 0x97, 0x00, 0x00, 0xbb, 0x29, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x69, 0xed, 0x44, 0xed, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x72, 0x00, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x97, 0x4e, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0xed, 0xad, 0xed, 0x44, 0x00, 0x00, 0x97, 0x97, 0x00, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x4e, 0x73, 0x00, 0x00, 0xbb, 0x29, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xed, 0xe9, 0xed, 0x00, 0x00, 0x00, 0x25, 0x97, 0xbb, 0xbb, 0xbb, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x97, 0x4e, 0x00, 0x00, 0x00, 0x97, 0xbb, 0xbb, 0x4e, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa9, 0xe9, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xa9, 0x93, 0x29, 0xed, 0x46, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0x20, 0x49, 0xfd, 0x25, 0xfd, 0x49, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x62, 0xe6, 0xfc, 0x45, 0xa3, 0x87, 0xec, 0x61, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x9b, 0x26, 0xe4, 0xa4, 0x51, 0x65, 0xfc, 0x89, 0xa3, 0xe4, 0x59, 0x63, 0x41, 0xa5, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xd4, 0x04, 0xb3, 0x41, 0x08, 0x09, 0xbc, 0x66, 0xfc, 0x86, 0xfc, 0x65, 0xfc, 0xa8, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x18, 0x48, 0xf4, 0xa5, 0x8a, 0x00, 0x00, 0x00, 0x00, 0xe3, 0x28, 0x23, 0x31, 0x03, 0x31, 0xe3, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x44, 0x41, 0x82, 0x71, 0xa2, 0x71, 0xa2, 0x71, 0xc3, 0x69, 0x44, 0x31, 0x00, 0x00, 0x00, 0x00, 0x03, 0x29, 0xe6, 0xfb, 0x41, 0x49, 0x00, 0x00, 0x00, 0x00, 0x75, 0x6c, 0x9f, 0xa6, 0x9f, 0xa6, 0x9f, 0xa6, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x6c, 0x9f, 0xa6, 0x9f, 0xa6, 0xf3, 0x63, 0x00, 0x00, 
  0xa9, 0xc3, 0xa6, 0xfb, 0xc7, 0xfb, 0xe8, 0xf3, 0xc7, 0xfb, 0xe8, 0xf3, 0x24, 0x31, 0x00, 0x00, 0xc6, 0x51, 0xc6, 0xfb, 0x64, 0x39, 0x00, 0x00, 0x0f, 0x4b, 0xbc, 0x8d, 0xe4, 0x18, 0x00, 0x00, 0x46, 0x21, 0x1e, 0x96, 0x2b, 0x3a, 0x00, 0x00, 0xe4, 0x18, 0x5a, 0x85, 0x9f, 0xa6, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x4b, 0xd7, 0x74, 0x00, 0x00, 0x46, 0x21, 0x9f, 0xa6, 0x46, 0x21, 
  0x20, 0x08, 0x20, 0x08, 0x20, 0x08, 0x41, 0x08, 0x85, 0x39, 0xa6, 0xfb, 0x25, 0x7a, 0x00, 0x00, 0x29, 0x9b, 0x87, 0xeb, 0x41, 0x08, 0x00, 0x00, 0x91, 0x5b, 0x5a, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x63, 0x0f, 0x4b, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0xaa, 0xdb, 0xa6, 0xc2, 0x00, 0x00, 0x48, 0xcb, 0x29, 0xbb, 0x00, 0x00, 0x00, 0x00, 0xe4, 0x18, 0x5a, 0x85, 0x9f, 0xa6, 0x75, 0x6c, 0xf3, 0x63, 0x82, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x8d, 0x5a, 0x85, 0x9f, 0xa6, 0x9f, 0xa6, 0xf3, 0x63, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0xb3, 0x64, 0xda, 0x61, 0x10, 0x47, 0xeb, 0x88, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x08, 0xf3, 0x63, 0xd7, 0x74, 0x9f, 0xa6, 0x46, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x8d, 0xd7, 0x74, 0x00, 0x00, 0x82, 0x08, 0x1e, 0x96, 0xc9, 0x29, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x61, 0xe6, 0xfa, 0x44, 0x51, 0xe7, 0xfa, 0x03, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x6c, 0xf3, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x85, 0xad, 0x42, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0xad, 0x42, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x41, 0xc7, 0xfa, 0xa8, 0xaa, 0x08, 0xfb, 0x23, 0x31, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x85, 0xd7, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x85, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x4b, 0x75, 0x6c, 0x00, 0x00, 0x82, 0x08, 0x1e, 0x96, 0xc9, 0x29, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x10, 0x88, 0xea, 0x67, 0xfa, 0xc9, 0xd2, 0x21, 0x08, 0x00, 0x00, 0x00, 0x00, 0xe4, 0x18, 0xd7, 0x74, 0x9f, 0xa6, 0x9f, 0xa6, 0x9f, 0xa6, 0x91, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x0f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd7, 0x74, 0x9f, 0xa6, 0x9f, 0xa6, 0x0f, 0x4b, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x9a, 0x26, 0xfa, 0xe8, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
  /*Pixel format: Red: 5 bit, Green: 6 bit, Blue: 5 bit BUT the 2 bytes are swapped*/
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x93, 0xa9, 0xed, 0x29, 0x5a, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xc2, 0xfd, 0x49, 0xfd, 0x25, 0xbc, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x26, 0xfc, 0xe6, 0xa3, 0x45, 0xec, 0x87, 0x10, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9b, 0x88, 0xe4, 0x26, 0x51, 0xa4, 0xfc, 0x65, 0xa3, 0x89, 0x59, 0xe4, 0x41, 0x63, 0x41, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd4, 0x08, 0xb3, 0x04, 0x08, 0x41, 0xbc, 0x09, 0xfc, 0x66, 0xfc, 0x86, 0xfc, 0x65, 0xfc, 0xa8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x82, 0xf4, 0x48, 0x8a, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x28, 0xe3, 0x31, 0x23, 0x31, 0x03, 0x28, 0xe3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x41, 0x44, 0x71, 0x82, 0x71, 0xa2, 0x71, 0xa2, 0x69, 0xc3, 0x31, 0x44, 0x00, 0x00, 0x00, 0x00, 0x29, 0x03, 0xfb, 0xe6, 0x49, 0x41, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x75, 0xa6, 0x9f, 0xa6, 0x9f, 0xa6, 0x9f, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x75, 0xa6, 0x9f, 0xa6, 0x9f, 0x63, 0xf3, 0x00, 0x00, 
  0xc3, 0xa9, 0xfb, 0xa6, 0xfb, 0xc7, 0xf3, 0xe8, 0xfb, 0xc7, 0xf3, 0xe8, 0x31, 0x24, 0x00, 0x00, 0x51, 0xc6, 0xfb, 0xc6, 0x39, 0x64, 0x00, 0x00, 0x4b, 0x0f, 0x8d, 0xbc, 0x18, 0xe4, 0x00, 0x00, 0x21, 0x46, 0x96, 0x1e, 0x3a, 0x2b, 0x00, 0x00, 0x18, 0xe4, 0x85, 0x5a, 0xa6, 0x9f, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x0f, 0x74, 0xd7, 0x00, 0x00, 0x21, 0x46, 0xa6, 0x9f, 0x21, 0x46, 
  0x08, 0x20, 0x08, 0x20, 0x08, 0x20, 0x08, 0x41, 0x39, 0x85, 0xfb, 0xa6, 0x7a, 0x25, 0x00, 0x00, 0x9b, 0x29, 0xeb, 0x87, 0x08, 0x41, 0x00, 0x00, 0x5b, 0x91, 0x85, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0xf3, 0x4b, 0x0f, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xdb, 0xaa, 0xc2, 0xa6, 0x00, 0x00, 0xcb, 0x48, 0xbb, 0x29, 0x00, 0x00, 0x00, 0x00, 0x18, 0xe4, 0x85, 0x5a, 0xa6, 0x9f, 0x6c, 0x75, 0x63, 0xf3, 0x08, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xbc, 0x85, 0x5a, 0xa6, 0x9f, 0xa6, 0x9f, 0x63, 0xf3, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb3, 0x4a, 0xda, 0x64, 0x10, 0x61, 0xeb, 0x47, 0x82, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x82, 0x63, 0xf3, 0x74, 0xd7, 0xa6, 0x9f, 0x21, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xbc, 0x74, 0xd7, 0x00, 0x00, 0x08, 0x82, 0x96, 0x1e, 0x29, 0xc9, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0xc6, 0xfa, 0xe6, 0x51, 0x44, 0xfa, 0xe7, 0x39, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x75, 0x63, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x85, 0x5a, 0x42, 0xad, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x42, 0xad, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x44, 0xfa, 0xc7, 0xaa, 0xa8, 0xfb, 0x08, 0x31, 0x23, 0x00, 0x00, 0x00, 0x00, 0x85, 0x5a, 0x74, 0xd7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0x5a, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x0f, 0x6c, 0x75, 0x00, 0x00, 0x08, 0x82, 0x96, 0x1e, 0x29, 0xc9, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x61, 0xea, 0x88, 0xfa, 0x67, 0xd2, 0xc9, 0x08, 0x21, 0x00, 0x00, 0x00, 0x00, 0x18, 0xe4, 0x74, 0xd7, 0xa6, 0x9f, 0xa6, 0x9f, 0xa6, 0x9f, 0x5b, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0x4b, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xd7, 0xa6, 0x9f, 0xa6, 0x9f, 0x4b, 0x0f, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x28, 0xfa, 0x26, 0x79, 0xe8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
#endif
#if LV_COLOR_DEPTH == 32
  /*Pixel format: Fix 0xFF: 8 bit, Red: 8 bit, Green: 8 bit, Blue: 8 bit*/
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x02, 0x02, 0xff, 0x49, 0x75, 0x91, 0xff, 0x4a, 0xa2, 0xe4, 0xff, 0x33, 0x47, 0x54, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x11, 0x17, 0x1d, 0xff, 0x44, 0xa9, 0xf9, 0xff, 0x28, 0xa4, 0xf7, 0xff, 0x49, 0x86, 0xba, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x2c, 0x43, 0x5f, 0xff, 0x2c, 0x9a, 0xf6, 0xff, 0x28, 0x68, 0xa0, 0xff, 0x34, 0x91, 0xe8, 0xff, 0x0a, 0x0c, 0x0e, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x42, 0x6e, 0x9a, 0xff, 0x2d, 0x85, 0xe1, 0xff, 0x1c, 0x32, 0x4e, 0xff, 0x26, 0x8b, 0xfb, 0xff, 0x4a, 0x70, 0x9c, 0xff, 0x23, 0x3d, 0x57, 0xff, 0x1a, 0x2c, 0x3f, 0xff, 0x24, 0x34, 0x43, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x43, 0x80, 0xce, 0xff, 0x1c, 0x5f, 0xaf, 0xff, 0x05, 0x07, 0x09, 0xff, 0x4a, 0x80, 0xb9, 0xff, 0x2e, 0x8d, 0xf5, 0xff, 0x32, 0x8e, 0xf5, 0xff, 0x29, 0x8a, 0xfa, 0xff, 0x3d, 0x95, 0xf5, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x0e, 0x11, 0x15, 0xff, 0x3e, 0x88, 0xf0, 0xff, 0x2a, 0x54, 0x85, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x17, 0x1c, 0x25, 0xff, 0x1a, 0x24, 0x30, 0xff, 0x18, 0x20, 0x2d, 0xff, 0x15, 0x1b, 0x24, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x1d, 0x26, 0x3e, 0xff, 0x0c, 0x30, 0x72, 0xff, 0x10, 0x32, 0x72, 0xff, 0x0d, 0x33, 0x72, 0xff, 0x16, 0x39, 0x68, 0xff, 0x20, 0x29, 0x32, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x1b, 0x20, 0x2a, 0xff, 0x31, 0x7d, 0xfb, 0xff, 0x0b, 0x28, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xaa, 0x8a, 0x69, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xaa, 0x8a, 0x69, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x99, 0x7c, 0x5e, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x46, 0x75, 0xc3, 0xff, 0x32, 0x75, 0xf4, 0xff, 0x36, 0x79, 0xf4, 0xff, 0x3d, 0x7c, 0xf3, 0xff, 0x38, 0x78, 0xf8, 0xff, 0x3d, 0x7a, 0xef, 0xff, 0x1c, 0x24, 0x2f, 0xff, 0x00, 0x00, 0x00, 0xff, 0x32, 0x39, 0x50, 0xff, 0x30, 0x78, 0xf7, 0xff, 0x1e, 0x2a, 0x36, 0xff, 0x00, 0x00, 0x00, 0xff, 0x77, 0x61, 0x49, 0xff, 0xdd, 0xb3, 0x88, 0xff, 0x22, 0x1c, 0x15, 0xff, 0x00, 0x00, 0x00, 0xff, 0x33, 0x29, 0x1f, 0xff, 0xee, 0xc1, 0x93, 0xff, 0x55, 0x45, 0x34, 0xff, 0x00, 0x00, 0x00, 0xff, 0x22, 0x1c, 0x15, 0xff, 0xcc, 0xa6, 0x7e, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x77, 0x61, 0x49, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x00, 0x00, 0x00, 0xff, 0x33, 0x29, 0x1f, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x33, 0x29, 0x1f, 0xff, 
  0x02, 0x03, 0x04, 0xff, 0x03, 0x05, 0x08, 0xff, 0x03, 0x05, 0x08, 0xff, 0x04, 0x06, 0x09, 0xff, 0x26, 0x2e, 0x3a, 0xff, 0x32, 0x74, 0xf7, 0xff, 0x2b, 0x44, 0x76, 0xff, 0x00, 0x00, 0x00, 0xff, 0x4b, 0x65, 0x9b, 0xff, 0x38, 0x71, 0xe8, 0xff, 0x06, 0x07, 0x08, 0xff, 0x00, 0x00, 0x00, 0xff, 0x88, 0x6e, 0x54, 0xff, 0xcc, 0xa6, 0x7e, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x99, 0x7c, 0x5e, 0xff, 0x77, 0x61, 0x49, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x02, 0x02, 0xff, 0x4c, 0x73, 0xda, 0xff, 0x2c, 0x52, 0xbd, 0xff, 0x00, 0x00, 0x00, 0xff, 0x3f, 0x66, 0xc6, 0xff, 0x45, 0x65, 0xb6, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x22, 0x1c, 0x15, 0xff, 0xcc, 0xa6, 0x7e, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xaa, 0x8a, 0x69, 0xff, 0x99, 0x7c, 0x5e, 0xff, 0x11, 0x0e, 0x0a, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xdd, 0xb3, 0x88, 0xff, 0xcc, 0xa6, 0x7e, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x99, 0x7c, 0x5e, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x53, 0x66, 0xad, 0xff, 0x1e, 0x4b, 0xd4, 0xff, 0x0b, 0x0d, 0x13, 0xff, 0x3b, 0x66, 0xe6, 0xff, 0x42, 0x4f, 0x83, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x11, 0x0e, 0x0a, 0xff, 0x99, 0x7c, 0x5e, 0xff, 0xbb, 0x98, 0x73, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x33, 0x29, 0x1f, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xdd, 0xb3, 0x88, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x00, 0x00, 0x00, 0xff, 0x11, 0x0e, 0x0a, 0xff, 0xee, 0xc1, 0x93, 0xff, 0x44, 0x37, 0x2a, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x32, 0x38, 0x63, 0xff, 0x32, 0x5d, 0xf9, 0xff, 0x1c, 0x28, 0x4c, 0xff, 0x39, 0x5c, 0xfc, 0xff, 0x18, 0x21, 0x3b, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xaa, 0x8a, 0x69, 0xff, 0x99, 0x7c, 0x5e, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xcc, 0xa6, 0x7e, 0xff, 0x66, 0x53, 0x3f, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x66, 0x53, 0x3f, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x21, 0x26, 0x40, 0xff, 0x3a, 0x58, 0xf5, 0xff, 0x3f, 0x53, 0xa7, 0xff, 0x41, 0x5e, 0xf4, 0xff, 0x1b, 0x22, 0x31, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xcc, 0xa6, 0x7e, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xcc, 0xa6, 0x7e, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x77, 0x61, 0x49, 0xff, 0xaa, 0x8a, 0x69, 0xff, 0x00, 0x00, 0x00, 0xff, 0x11, 0x0e, 0x0a, 0xff, 0xee, 0xc1, 0x93, 0xff, 0x44, 0x37, 0x2a, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x0b, 0x0c, 0x10, 0xff, 0x3d, 0x50, 0xe8, 0xff, 0x36, 0x4a, 0xfb, 0xff, 0x4a, 0x56, 0xd1, 0xff, 0x04, 0x05, 0x06, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x22, 0x1c, 0x15, 0xff, 0xbb, 0x98, 0x73, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x88, 0x6e, 0x54, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xbb, 0x98, 0x73, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0xff, 0xcf, 0x9d, 0xff, 0x77, 0x61, 0x49, 0xff, 0x00, 0x00, 0x00, 0xff, 
  0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x3d, 0x44, 0x95, 0xff, 0x2d, 0x42, 0xf5, 0xff, 0x3d, 0x3d, 0x77, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 
#endif
};

const lv_img_dsc_t img_player_top_s16 = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 32,
  .header.h = 15,
  .data_size = 480 * LV_COLOR_SIZE / 8,
  .data = img_player_top_s16_map,
};
