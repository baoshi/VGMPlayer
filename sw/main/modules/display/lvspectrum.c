#include "lvspectrum.h"

//
// Spectrum display
//
#define LV_SPECTRUM_BIN_GAP 4
#define LV_SPECTRUM_SEG_GAP 1
#define LV_SPECTRUM_PEAK_HEIGHT 2

static const uint32_t lv_spectrum_peak_color = 0xcccccc;

static const uint32_t lv_spectrum_colors[LV_SPECTRUM_MAX_SEGMENTS][LV_SPECTRUM_MAX_BINS] =
{
    {0xff93bc, 0xf390c2, 0xe38bcb, 0xd085d5, 0xbb7fe0, 0xa57eec, 0x8f7ef3, 0x7c7ef8, 0x6a84f8, 0x5490f5, 0x3ca1f1, 0x25b3eb, 0x11c5e3, 0x03d6db, 0x00e3d3, 0x02ebcc, 0x14ebc4, 0x33ebbc, 0x58ebb5, 0x81ebad, 0xabeba3, 0xcfeb99, 0xebeb8d, 0xf5e481, 0xf5d573, 0xf5c263, 0xf5ac51, 0xf59540, 0xf27f2f, 0xec6a20, 0xe65813, 0xe34d0a},
    {0xff93bc, 0xf390c2, 0xe38bcb, 0xd086d5, 0xbb7fe1, 0xa57eeb, 0x907ef3, 0x7c7ef7, 0x6984f8, 0x5391f6, 0x3ca1f1, 0x25b3eb, 0x12c5e3, 0x03d6db, 0x00e4d3, 0x02ebcc, 0x14ebc5, 0x32ebbc, 0x58ebb5, 0x82ebad, 0xaaeba3, 0xcfeb99, 0xebeb8e, 0xf5e482, 0xf5d573, 0xf5c263, 0xf5ac51, 0xf59640, 0xf37e2f, 0xeb6a20, 0xe65913, 0xe34d0a},
    {0xff93bc, 0xf490c2, 0xe38bcb, 0xd086d5, 0xbb80e1, 0xa57eeb, 0x907ef3, 0x7d7ef8, 0x6a84f8, 0x5490f5, 0x3ca1f1, 0x25b3eb, 0x11c5e3, 0x03d6db, 0x00e4d3, 0x02ebcb, 0x14ebc5, 0x32ebbd, 0x58ebb5, 0x82ebad, 0xabeba3, 0xcfeb99, 0xebeb8e, 0xf5e481, 0xf5d573, 0xf5c262, 0xf5ac52, 0xf59541, 0xf37e2f, 0xec6a20, 0xe65813, 0xe34d0a},
    {0xff93bc, 0xf390c2, 0xe38bcb, 0xd085d5, 0xbb80e1, 0xa57eeb, 0x8f7ef3, 0x7c7ef8, 0x6a84f8, 0x5491f6, 0x3ca1f1, 0x25b2eb, 0x11c5e3, 0x03d6db, 0x00e3d4, 0x02ebcb, 0x14ebc5, 0x32ebbd, 0x58ebb5, 0x82ebac, 0xabeba3, 0xcfeb99, 0xebeb8e, 0xf5e482, 0xf5d574, 0xf5c263, 0xf5ac51, 0xf59640, 0xf27e2f, 0xeb691f, 0xe65813, 0xe34c0a},
    {0xff93bc, 0xf391c2, 0xe38bcb, 0xd085d5, 0xbb80e1, 0xa57eeb, 0x907ef3, 0x7d7ef8, 0x6984f8, 0x5391f5, 0x3ca0f1, 0x25b3eb, 0x11c5e3, 0x03d6dc, 0x00e3d3, 0x02ebcc, 0x15ebc4, 0x32ebbd, 0x58ebb5, 0x82ebac, 0xaaeba3, 0xcfeb99, 0xebeb8d, 0xf5e481, 0xf5d573, 0xf5c263, 0xf5ac52, 0xf59540, 0xf37e2f, 0xeb6a20, 0xe65813, 0xe34d09},
    {0xff93bc, 0xf391c2, 0xe38bcb, 0xd085d6, 0xbb80e1, 0xa57eeb, 0x907ef3, 0x7c7ef7, 0x6a85f8, 0x5491f6, 0x3ca1f2, 0x25b2ea, 0x11c5e3, 0x03d6dc, 0x00e4d3, 0x02ebcb, 0x14ebc4, 0x32ebbd, 0x58ebb5, 0x81ebad, 0xabeba3, 0xcfeb99, 0xebeb8d, 0xf5e481, 0xf5d573, 0xf5c263, 0xf5ac51, 0xf59540, 0xf27e2f, 0xeb6a20, 0xe65813, 0xe34c0a},
    {0xff93bc, 0xf390c2, 0xe48bcb, 0xcf85d6, 0xbb7fe1, 0xa57eeb, 0x907ef3, 0x7c7ef8, 0x6984f8, 0x5491f6, 0x3ca0f2, 0x25b3eb, 0x11c5e3, 0x03d5dc, 0x00e4d3, 0x02ebcb, 0x14ebc5, 0x33ebbc, 0x58ebb5, 0x81ebac, 0xabeba3, 0xd0eb98, 0xebeb8d, 0xf5e481, 0xf5d673, 0xf5c263, 0xf5ac52, 0xf59540, 0xf37e2f, 0xec6a20, 0xe65813, 0xe34c09},
    {0xff93bc, 0xf390c2, 0xe38bcb, 0xcf85d6, 0xbb7fe1, 0xa57eeb, 0x8f7ef3, 0x7c7ef8, 0x6984f8, 0x5391f5, 0x3ca1f1, 0x25b3eb, 0x11c5e3, 0x03d6db, 0x00e3d3, 0x02ebcc, 0x14ebc5, 0x32ebbd, 0x58ebb5, 0x81ebac, 0xabeba3, 0xcfeb99, 0xebeb8d, 0xf5e481, 0xf5d573, 0xf5c263, 0xf5ac52, 0xf59540, 0xf37f2f, 0xec6a20, 0xe65814, 0xe34c0a},
    {0xff93bc, 0xf391c2, 0xe38ccb, 0xd086d6, 0xbb7fe1, 0xa57eeb, 0x907ef4, 0x7d7ef7, 0x6a84f8, 0x5391f5, 0x3ca1f1, 0x25b3eb, 0x11c5e4, 0x03d5db, 0x00e3d3, 0x02ebcc, 0x14ebc4, 0x33ebbc, 0x58ebb5, 0x82ebac, 0xaaeba3, 0xcfeb99, 0xebeb8e, 0xf5e482, 0xf5d573, 0xf5c263, 0xf5ac52, 0xf59540, 0xf37f2f, 0xeb691f, 0xe65813, 0xe34c0a},
    {0xff93bc, 0xf490c2, 0xe38bcb, 0xd086d6, 0xbb80e1, 0xa57eeb, 0x907ef3, 0x7c7ef7, 0x6985f8, 0x5390f6, 0x3ba1f1, 0x25b3eb, 0x11c5e4, 0x03d6dc, 0x00e4d3, 0x02ebcc, 0x14ebc4, 0x32ebbd, 0x58ebb5, 0x82ebac, 0xabeba3, 0xcfeb99, 0xebeb8e, 0xf5e481, 0xf5d573, 0xf5c263, 0xf5ad52, 0xf59540, 0xf27f30, 0xeb6a20, 0xe65813, 0xe34d0a},
    {0xff93bc, 0xf390c2, 0xe38bcb, 0xd085d6, 0xba7fe1, 0xa57eec, 0x907ef3, 0x7c7ef7, 0x6984f8, 0x5390f6, 0x3ba1f2, 0x25b3ea, 0x11c5e4, 0x03d6dc, 0x00e3d3, 0x02ebcb, 0x15ebc4, 0x32ebbd, 0x58ebb5, 0x81ebac, 0xabeba3, 0xcfeb99, 0xebeb8e, 0xf5e482, 0xf5d573, 0xf5c263, 0xf5ac52, 0xf59540, 0xf37e30, 0xec6a20, 0xe65913, 0xe34c0a},
    {0xff93bc, 0xf390c2, 0xe38bcb, 0xd085d6, 0xbb80e1, 0xa57eec, 0x907ef3, 0x7c7ef8, 0x6a85f8, 0x5491f5, 0x3ca1f1, 0x24b3eb, 0x11c5e3, 0x03d6dc, 0x00e4d3, 0x01ebcc, 0x14ebc4, 0x32ebbd, 0x58ebb5, 0x81ebad, 0xabeba3, 0xcfeb99, 0xebeb8e, 0xf5e481, 0xf5d573, 0xf5c263, 0xf5ac52, 0xf59640, 0xf37f2f, 0xeb6920, 0xe55913, 0xe34c0a},
    {0xff93bc, 0xf390c3, 0xe38bcb, 0xd085d6, 0xbb7fe1, 0xa57eeb, 0x907ef3, 0x7c7ef8, 0x6984f8, 0x5391f5, 0x3ca1f1, 0x25b2eb, 0x11c5e3, 0x03d6dc, 0x00e3d4, 0x02ebcc, 0x14ebc4, 0x33ebbd, 0x58ebb4, 0x81ebac, 0xabeba3, 0xd0eb98, 0xebeb8e, 0xf5e481, 0xf5d573, 0xf5c262, 0xf5ac51, 0xf59640, 0xf37f2f, 0xec6920, 0xe65914, 0xe34c09},
    {0xff93bc, 0xf490c2, 0xe38bcb, 0xd085d5, 0xbb80e1, 0xa57eec, 0x907ef3, 0x7c7ef8, 0x6a84f8, 0x5391f5, 0x3ba1f1, 0x24b2eb, 0x11c5e4, 0x03d6db, 0x00e4d4, 0x02ebcc, 0x14ebc4, 0x32ebbc, 0x59ebb4, 0x82ebad, 0xabeba4, 0xcfeb99, 0xebeb8e, 0xf5e481, 0xf5d673, 0xf5c263, 0xf5ac51, 0xf59540, 0xf37f2f, 0xeb6a20, 0xe65914, 0xe34c0a},
    {0xff93bc, 0xf390c2, 0xe38bcc, 0xd085d6, 0xbb7fe1, 0xa57eeb, 0x907ef3, 0x7c7ef8, 0x6a84f8, 0x5391f6, 0x3ca1f1, 0x25b3eb, 0x11c6e4, 0x03d6db, 0x00e4d3, 0x02ebcc, 0x14ebc4, 0x32ebbd, 0x58ebb5, 0x81ebad, 0xaaeba3, 0xcfeb99, 0xebeb8e, 0xf5e482, 0xf5d673, 0xf5c263, 0xf5ac52, 0xf59540, 0xf37e2f, 0xeb6a20, 0xe65913, 0xe34c0a},
    {0xff93bc, 0xf390c2, 0xe38bcb, 0xd086d6, 0xbb80e1, 0xa57eeb, 0x907ef3, 0x7c7ef8, 0x6a84f8, 0x5391f5, 0x3ca1f1, 0x25b3eb, 0x11c5e3, 0x03d5db, 0x00e3d3, 0x02ebcc, 0x14ebc5, 0x32ebbd, 0x58ebb5, 0x82ebac, 0xabeba3, 0xcfeb99, 0xeceb8e, 0xf5e481, 0xf5d573, 0xf5c263, 0xf5ac52, 0xf59540, 0xf27e2f, 0xec6a20, 0xe65813, 0xe34c0a},
};


static void lv_spectrum_on_size_changed(lv_obj_t *obj)
{
    // Update spectrum display parameters
    lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
    lv_area_t rect;
    lv_obj_get_content_coords(obj, &rect);
    int width = rect.x2 - rect.x1 + 1;
    int height = rect.y2 - rect.y1 + 1;
    int temp, count;
    // find width of each bin, reduce number of bins if necessary
    count = spectrum->bin_count;
    // total width = count * bin_width + (count - 1) * GAP
    while (1)
    {
        temp = (width - (count - 1) * LV_SPECTRUM_BIN_GAP) / count;
        if (temp >= 1) break;
        --count;
    }
    spectrum->bin_width = temp;
    spectrum->bin_count = count;
    // find height of each segment
    count = LV_SPECTRUM_MAX_SEGMENTS;
    // total height = PEAK_HEIGHT + count * seg_height + count * GAP
    while (1)
    {
        temp = (height - count * LV_SPECTRUM_SEG_GAP - LV_SPECTRUM_PEAK_HEIGHT) / count;
        if (temp >= LV_SPECTRUM_PEAK_HEIGHT) break;
        --count;
        if (count == 0) break;
    }
    spectrum->bin_segment_count = count;
    spectrum->bin_segment_height = temp;
    spectrum->bin_start_x = (width - spectrum->bin_width * spectrum->bin_count - (spectrum->bin_count - 1) * LV_SPECTRUM_BIN_GAP) / 2;
    spectrum->bin_peak_y1 = height - (spectrum->bin_segment_height + LV_SPECTRUM_SEG_GAP) * spectrum->bin_segment_count - 1;
    spectrum->bin_peak_y2 = height - spectrum->bin_segment_height - LV_SPECTRUM_SEG_GAP - 1;
}



static void lv_spectrum_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
    lv_spectrum_t *spectrum = (lv_spectrum_t*)obj;
    spectrum->bin_count = LV_SPECTRUM_MAX_BINS;
    lv_memset_00(spectrum->bin_values, spectrum->bin_count * sizeof(uint8_t));
    lv_memset_00(spectrum->bin_peak_values, spectrum->bin_count * sizeof(uint8_t));
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}


static void lv_spectrum_destructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
    LV_ASSERT_OBJ(obj, &lv_spectrum_class);
    // Nothing to do    
}


static void lv_spectrum_event(const lv_obj_class_t* class_p, lv_event_t* e)
{
    lv_res_t res = lv_obj_event_base(&lv_spectrum_class, e);
    if (res != LV_RES_OK) return;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_SIZE_CHANGED)
    {
        lv_spectrum_on_size_changed(obj);
    }
    else if (code == LV_EVENT_DRAW_MAIN)
    {
        lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
        lv_draw_ctx_t *draw_ctx = lv_event_get_draw_ctx(e);
        lv_opa_t opa = lv_obj_get_style_opa(obj, LV_PART_MAIN);
        if (opa < LV_OPA_MIN) return;

        if (spectrum->bin_count <= 0 || spectrum->bin_segment_count <= 0) return;

        lv_area_t content_rect;
        lv_obj_get_content_coords(obj, &content_rect);
        lv_draw_rect_dsc_t draw_segment_dsc, draw_peak_dsc;
        lv_draw_rect_dsc_init(&draw_segment_dsc);
        lv_draw_rect_dsc_init(&draw_peak_dsc);
        draw_segment_dsc.bg_opa = LV_OPA_COVER;
        draw_peak_dsc.bg_opa = LV_OPA_COVER;
        draw_peak_dsc.bg_color = lv_color_hex(lv_spectrum_peak_color);
        lv_area_t segment, peak;
        segment.x1 = peak.x1 = content_rect.x1 + spectrum->bin_start_x;
        for (int bin = 0; bin < spectrum->bin_count; ++bin)
        {
            segment.x2 = peak.x2 = segment.x1 + spectrum->bin_width - 1;
            segment.y2 = content_rect.y2;
            int segs = spectrum->bin_values[bin] * (spectrum->bin_segment_count - 1) / 255 + 1;
            if (segs >= spectrum->bin_segment_count)
                segs = spectrum->bin_segment_count;
            for (int seg = 0; seg < segs; ++seg)
            {
                segment.y1 = segment.y2 - spectrum->bin_segment_height + 1;
                int color_index = bin * (LV_SPECTRUM_MAX_BINS - 1) / (spectrum->bin_count - 1);
                draw_segment_dsc.bg_color = lv_color_hex(lv_spectrum_colors[seg][color_index]);
                lv_draw_rect(draw_ctx, &draw_segment_dsc, &segment);
                segment.y2 = segment.y1 - LV_SPECTRUM_SEG_GAP - 1;
            }
            // for peak value, 255 -> peak_y1 ; 0 -> peak_y2
            peak.y2 = content_rect.y1 + spectrum->bin_peak_y1 + (255 - spectrum->bin_peak_values[bin]) * (spectrum->bin_peak_y2 - spectrum->bin_peak_y1) / 255;
            peak.y1 = peak.y2 - LV_SPECTRUM_PEAK_HEIGHT;
            lv_draw_rect(draw_ctx, &draw_peak_dsc, &peak);
            // next bin
            segment.x1 = peak.x1 = segment.x2 + LV_SPECTRUM_BIN_GAP + 1;
        }
    }
}


const lv_obj_class_t lv_spectrum_class =
{
    .constructor_cb = lv_spectrum_constructor,
    .destructor_cb = lv_spectrum_destructor,
    .event_cb = lv_spectrum_event,
    .instance_size = sizeof(lv_spectrum_t),
    .base_class = &lv_obj_class,
    .width_def = 300,
    .height_def = 200
};


lv_obj_t * lv_spectrum_create(lv_obj_t* parent)
{
    lv_obj_t *obj = lv_obj_class_create_obj(&lv_spectrum_class, parent);
    LV_ASSERT_MALLOC(obj);
    lv_obj_class_init_obj(obj);
    return obj;
}


void lv_spectrum_set_bin_values(lv_obj_t *obj, const uint8_t *bvs, int len)
{
    LV_ASSERT_OBJ(obj, &lv_spectrum_class);
    lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
    len = LV_MIN(len, LV_SPECTRUM_MAX_BINS);
    bool relayout = (len != spectrum->bin_count);
    spectrum->bin_count = len;
    // update values. if the incoming value is larger than current, update it immediately; 
    // otherwise decrease current value slowly towards incoming value.
    int c;
    for (int bin = 0; bin < len; ++bin)
    {
        c = bvs[bin];
        spectrum->bin_values[bin] = (uint8_t)c;
        if (c > spectrum->bin_values[bin])
        {
            spectrum->bin_values[bin] = (uint8_t)c;
        }
        else
        {
            spectrum->bin_values[bin] = (uint8_t)((c * 5 + (int)spectrum->bin_values[bin] * 5) / 10);
        }
        if (c > spectrum->bin_peak_values[bin])
        {
            spectrum->bin_peak_values[bin] = (uint8_t)c;
        }
        else
        {
            spectrum->bin_peak_values[bin] = (uint8_t)((c + (int)spectrum->bin_peak_values[bin] * 9) / 10);
        }
    }
    if (relayout)
        lv_spectrum_on_size_changed(obj);
    lv_obj_invalidate(obj);
}


void lv_spectrum_set_reset(lv_obj_t *obj)
{
    LV_ASSERT_OBJ(obj, &lv_spectrum_class);
    lv_spectrum_t *spectrum = (lv_spectrum_t *)obj;
    lv_memset_00(spectrum->bin_values, spectrum->bin_count * sizeof(uint8_t));
    lv_memset_00(spectrum->bin_peak_values, spectrum->bin_count * sizeof(uint8_t));
    lv_obj_invalidate(obj);
}