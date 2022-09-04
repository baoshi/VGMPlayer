#pragma once
#include <stdint.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif


//
// Spectrum display
//

#define LV_SPECTRUM_MAX_BINS        32
#define LV_SPECTRUM_MAX_SEGMENTS    16

typedef struct lv_spectrum_s
{
    lv_obj_t obj;
    int bin_max_height;
    int bin_width;
    int bin_start_x;  
    int bin_count;
    int bin_segment_count;
    int bin_segment_height;
    int bin_peak_y1;    // bottom of peak (from top edge) segment when peak = 255
    int bin_peak_y2;    // bottom of peak (from top edge) segment when peak = 0
    uint8_t bin_values[LV_SPECTRUM_MAX_BINS];
    uint8_t bin_peak_values[LV_SPECTRUM_MAX_BINS];
} lv_spectrum_t;

extern const lv_obj_class_t lv_spectrum_class;

lv_obj_t* lv_spectrum_create(lv_obj_t* parent);
void lv_spectrum_set_bin_values(lv_obj_t* obj, const uint8_t* bvs, int len);

#ifdef __cplusplus
}
#endif

