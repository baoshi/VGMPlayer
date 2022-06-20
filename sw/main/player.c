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
#include "catalog.h"
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


enum
{
    PLAYER_OK = 0,
    PLAYER_ERR_UNSUPPORTED,
};


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


static void button_play_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        PL_LOGD("Play button pressed\n");
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Play button long pressed\n");
        EQ_QUICK_PUSH(EVT_PLAYER_EXIT_TO_BROWSER);
    }
}


static void button_up_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        PL_LOGD("Up button pressed\n");
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
        PL_LOGD("Down button pressed\n");
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Down button long pressed\n");
    }
}


static void button_next_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        PL_LOGD("Next button pressed\n");
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Next button long pressed\n");
    }
}


static void button_prev_handler(lv_event_t* e)
{
    player_t* ctx = (player_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED) 
    {
        PL_LOGD("Prev button pressed\n");
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_PREV);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        PL_LOGD("Prev button long pressed\n");
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
    lvi_pos_button(LVI_BUTTON_PLAY, 0, 0);
    lvi_pos_button(LVI_BUTTON_NW, 1, 0);
    lvi_pos_button(LVI_BUTTON_SW, 2, 0);
    lvi_pos_button(LVI_BUTTON_NE, 3, 0);
    lvi_pos_button(LVI_BUTTON_SE, 4, 0);
    // 5 invisible buttons
    lv_obj_t* btn;
    // Play
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_play_handler, LV_EVENT_ALL, (void*)ctx);
    // NW / Vol up
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 1, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_up_handler, LV_EVENT_ALL, (void*)ctx);
    // SW / Vol down
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 2, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_down_handler, LV_EVENT_ALL, (void*)ctx);
    // NE / Prev
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 3, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_prev_handler, LV_EVENT_ALL, (void*)ctx);
    // SE / Next
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 4, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_next_handler, LV_EVENT_ALL, (void*)ctx);
    // Create top label
    ctx->lbl_top = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_top, 200);
    lv_obj_set_style_text_align(ctx->lbl_top, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(ctx->lbl_top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_label_set_text(ctx->lbl_top, "");
    // Create bottom label
    ctx->lbl_bottom = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_bottom, 240);
    lv_obj_align(ctx->lbl_bottom, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text(ctx->lbl_bottom, "");
    lv_label_set_long_mode(ctx->lbl_bottom, LV_LABEL_LONG_SCROLL_CIRCULAR);
    // No message box yet
    ctx->mbx_alert = 0;
    // Calculates all coordinates
    lv_obj_update_layout(ctx->screen);
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
        {
            PL_LOGD("Player: entry\n");
            create_screen(ctx);
            ctx->timer_ui_update = tick_arm_timer_event(UI_UPDATE_INTERVAL_MS, true, EVT_PLAYER_UI_UPDATE, true);
            ctx->exception = PLAYER_OK;
            ctx->decoder = 0;
            break;
        }
        case EVT_EXIT:
        {
            PL_LOGD("Player: exit\n");
            tick_disarm_timer_event(ctx->timer_ui_update);
            ctx->timer_ui_update = 0;
            break;
        }
        case EVT_START:
        {
            PL_LOGD("Player: start\n");
            int r;
            char ext[4];
            uint8_t type;
            bool error = false;
            r = catalog_get_entry(me->catalog, ctx->file, FF_LFN_BUF + 1, &type);
            if (CAT_OK == r)
            {
                // check file extension and go into substates
                if (path_get_ext(ctx->file, ext, 4))
                {
                    if (0 == strcasecmp(ext, "s16"))
                    {
                        //STATE_START(me, &me->player_s16);
                    }
                    else
                    {
                        PL_LOGE("Player: unsupported file %s\n", ctx->file);
                        ctx->exception = PLAYER_ERR_UNSUPPORTED;
                        error = true;
                    }
                }
                else
                {
                    error = true;
                }
            }
            else
            {
                error = true;
            }
            if (error)
            {
                STATE_START(me, &me->player_exp);
            }
            break;
        }
        case EVT_PLAYER_UI_UPDATE:
        {
            char buf[32];
            sprintf(buf, "C=%d B=%.1fv", ec_charge, ec_battery);
            lv_label_set_text(ctx->lbl_top, buf);
            break;
        }
        case EVT_PLAYER_PLAY_SONG:
        {
            uint8_t type;
            int r;
            // Get the current file to play
            r = catalog_get_entry(me->catalog, ctx->file, FF_LFN_BUF + 1, &type);
            if ((CAT_OK == r) && (CAT_TYPE_FILE == type))
            {
                lv_label_set_text(ctx->lbl_bottom, ctx->file);
            }
            else
            {
                PL_LOGE("Player: cannot open file to play\n");
            }
            break;
        }
        case EVT_PLAYER_PLAY_NEXT:
        {
            uint8_t type;
            int r;
            do
            {
                // Get the next file to play
                r = catalog_get_next_entry(me->catalog, false, true, ctx->file, FF_LFN_BUF + 1, &type);
                if ((CAT_OK == r) && (CAT_TYPE_FILE == type))
                    break;
                if (CAT_ERR_EOF == r)
                    break;
            } while (1);
            switch (r)
            {
                case CAT_OK:
                    lv_label_set_text(ctx->lbl_bottom, ctx->file);
                    break;
                case CAT_ERR_EOF:
                    PL_LOGD("Player: No more file\n");
                    break;
                default:
                    break;
            }
            break;
        }
        case EVT_PLAYER_PLAY_PREV:
        {
            uint8_t type;
            int r;
            do
            {
                // Get the previous file to play
                r = catalog_get_prev_entry(me->catalog, false, true, ctx->file, FF_LFN_BUF + 1, &type);
                if ((CAT_OK == r) && (CAT_TYPE_FILE == type))
                    break;
                if (CAT_ERR_EOF == r)
                    break;
            } while (1);
            switch (r)
            {
                case CAT_OK:
                    lv_label_set_text(ctx->lbl_bottom, ctx->file);
                    break;
                case CAT_ERR_EOF:
                    PL_LOGD("Player: No more file\n");
                    break;
                default:
                    break;
            }
            break;
        }
        case EVT_PLAYER_SONG_ENDED:
        {
            STATE_TRAN(me, &(me->browser_disk));
            break;
        }
        case EVT_PLAYER_EXIT_TO_BROWSER:
        {
            me->catalog_history_page[me->catalog_history_index] = me->catalog->cur_page;
            me->catalog_history_selection[me->catalog_history_index] = me->catalog->cur_index;
            me->browser_ctx.skip_first_click = true;
            STATE_TRAN(me, &(me->browser_disk));
            break;
        }
        default:
            r = evt;
            break;
    }
    return r;
}


event_t const *player_exp_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    player_t *ctx = &(me->player_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            PL_LOGD("Player_Exp: entry\n");
            char message[256];
            switch (ctx->exception)
            {
                case PLAYER_ERR_UNSUPPORTED:
                {
                    snprintf(message, 256, "File %s cannot be played", ctx->file);
                    break;
                }
                default:
                {
                    strncpy(message, "Unknown error occurred", 256);
                    break;
                }
            }
            ctx->mbx_alert = lv_msgbox_create(ctx->screen, NULL, message, NULL, false);
            lv_obj_set_width(ctx->mbx_alert, 200);
            lv_obj_center(ctx->mbx_alert);
            ctx->timer_general = tick_arm_timer_event(2000, false, EVT_PLAYER_GENERAL_TIMER, true);        
            break;
        }
        case EVT_EXIT:
        {
            PL_LOGD("Player_Exp: exit\n");
            tick_disarm_timer_event(ctx->timer_general);
            ctx->timer_general = 0;
            break;
        }
        case EVT_PLAYER_GENERAL_TIMER:
        {
            lv_msgbox_close(ctx->mbx_alert);    // will delete msg_alert internally
            ctx->mbx_alert = 0;
            STATE_TRAN(me, &(me->browser_disk));
            break;
        }
        default:
        {
            r = evt;
            break;
        }
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
    default:
        r = evt;
        break;
    }
    return r;
}