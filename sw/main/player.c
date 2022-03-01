#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvinput.h"
#include "lvstyle.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "disk.h"
#include "path_utils.h"
#include "ec.h"
#include "audio.h"
#include "decoder_s16.h"
#include "app.h"


#define UI_UPDATE_INTERVAL_MS   200


#ifndef PLAYER_DEBUG
# define PLAYER_DEBUG 1
#endif


// Debug log
#if PLAYER_DEBUG
#define PL_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define PL_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define PL_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define PL_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define PL_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define PL_LOGD(x, ...)
#define PL_LOGI(x, ...)
#define PL_LOGW(x, ...)
#define PL_LOGE(x, ...)
#define PL_DEBUGF(x, ...)
#endif


static void screen_event_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (ctx)
    {
        switch (code)
        {
            case LV_EVENT_SCREEN_UNLOADED:
                PL_LOGD("Player: screen unloaded\n");
                lv_obj_del(ctx->screen);
                break;
            default:
                break;
        }
    }
}


static void button_mode_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        EQ_QUICK_PUSH(EVT_PLAYER_MODE_CLICKED);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Mode button long pressed\n");
    }
}


static void button_play_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_CLICKED);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Play button long pressed\n");
    }
}


static void button_up_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        EQ_QUICK_PUSH(EVT_PLAYER_UP_CLICKED);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Up button long pressed\n");
    }
}


static void button_down_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        EQ_QUICK_PUSH(EVT_PLAYER_DOWN_CLICKED);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Down button long pressed\n");
    }
}

static void create_screen(player_t* ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void*)ctx);
    // All buttons used as "Button" device
    lvi_clear_keypad_group();
    lvi_disable_keypad();
    lvi_disable_button();
    lvi_pos_button(LVI_BUTTON_MODE, 0, 0);
    lvi_pos_button(LVI_BUTTON_PLAY, 1, 0);
    lvi_pos_button(LVI_BUTTON_UP, 2, 0);
    lvi_pos_button(LVI_BUTTON_DOWN, 3, 0);
    // 4 invisible buttons
    lv_obj_t* btn;
    // Mode
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_mode_handler, LV_EVENT_ALL, (void*)ctx);
    // Play
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 1, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_play_handler, LV_EVENT_ALL, (void*)ctx);
    // Up
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 2, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_up_handler, LV_EVENT_ALL, (void*)ctx);
    // Down
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 3, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_down_handler, LV_EVENT_ALL, (void*)ctx);
    // Create top label
    ctx->lbl_top = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_top, 200);
    lv_obj_set_style_text_align(ctx->lbl_top, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(ctx->lbl_top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_label_set_text(ctx->lbl_top, "");
    // Load screen
    lv_scr_load(ctx->screen);
}


event_t const *player_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    player_t *ctx = &(me->player_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
            PL_LOGD("Player: entry\n");
            create_screen(ctx);
            ctx->alarm_ui_update = tick_arm_time_event(UI_UPDATE_INTERVAL_MS, true, EVT_PLAYER_UI_UPDATE, true);
            ctx->decoder = 0;
            audio_postinit();
            break;
        case EVT_EXIT:
            PL_LOGD("Player: exit\n");
            tick_disarm_time_event(ctx->alarm_ui_update);
            ctx->alarm_ui_update = -1;
            break;
        case EVT_START:
        {
            PL_LOGD("Player: start\n");
            // check file extension and enter corresponding state
            char ext[16];
            if (path_get_ext(ctx->file, ext))
            {
                if (0 == strcasecmp(ext, "s16"))
                {
                    STATE_START(me, &me->player_s16); 
                }
            }
            break;
        }
        case EVT_PLAYER_UI_UPDATE:
        {
            char buf[32];
            sprintf(buf, "U=%d C=%d B=%.1fv", ec_usb, ec_charge, ec_battery);
            lv_label_set_text(ctx->lbl_top, buf);
            break;
        }
        default:
            r = evt;
            break;
    }
    return r;
}


event_t const *player_s16_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    player_t *ctx = &(me->player_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        PL_LOGD("Player_S16: entry\n");
        MY_ASSERT(ctx->decoder == 0);
        ctx->decoder = (decoder_t *)decoder_s16_create(ctx->file);
        // TODO: Handle error here
        MY_ASSERT(ctx->decoder != 0);
        break;
    case EVT_EXIT:
        PL_LOGD("Player_S16: exit\n");
        audio_stop_playback();
        if (ctx->decoder)
        {
            decoder_s16_destroy((decoder_s16_t *)(ctx->decoder));
            ctx->decoder = 0;
        }
        break;
    case EVT_START:
        PL_LOGD("Player: s16: start\n");
        audio_setup_playback(ctx->decoder);
        audio_start_playback();
        STATE_START(me, &me->player_s16_playing); 
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


event_t const *player_s16_playing_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    player_t *ctx = &(me->player_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        PL_LOGD("Player_S16_Playing: entry\n");
        break;
    case EVT_EXIT:
        PL_LOGD("Player_S16_Playing: exit\n");
        break;
    case EVT_START:
        PL_LOGD("Player_S16_Playing: start\n");
        break;
    case EVT_PLAYER_MODE_CLICKED:
        audio_pause_playback();
        STATE_TRAN(me, &(me->player_s16_paused));
        break;
    case EVT_PLAYER_SONG_ENDED:
        STATE_TRAN(me, &(me->browser_disk));
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


event_t const *player_s16_paused_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    player_t *ctx = &(me->player_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        PL_LOGD("Player_S16_Paused: entry\n");
        break;
    case EVT_EXIT:
        PL_LOGD("Player_S16_Paused: exit\n");
        break;
    case EVT_START:
        PL_LOGD("Player_S16_Paused: start\n");
        break;
    case EVT_PLAYER_PLAY_CLICKED:
        audio_unpause_playback();
        STATE_TRAN(me, &(me->player_s16_playing));
        break;
    case EVT_PLAYER_MODE_CLICKED:
        audio_stop_playback();
        STATE_TRAN(me, &(me->browser_disk));
        break;
    default:
        r = evt;
        break;
    }
    return r;
}