#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvinput.h"
#include "lvstyle.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "ec.h"
#include "disk.h"
#include "path_utils.h"
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
    PLAYER_ERR_FILE_NOT_ACCESSIBLE,
    PLAYER_ERR_FILE_NOT_SUPPORTED
};


// Setup audio and decoder using current selected file
// Return false if fail. me->exception contains failure reason.
// get file name from catalog
// detect 
static bool setup_song(player_t *ctx)
{
    bool r = false;
    do
    {
        char ext[4];
        uint8_t type;
        // Get file extension
        if (!path_get_ext(ctx->file, ext, 4))
        {
            ctx->exception = PLAYER_ERR_FILE_NOT_SUPPORTED;
            break;
        }
        if (0 == strcasecmp(ext, "s16"))
        {
            // s16 file
            r = true;
            break;
        }
        else
        {
            ctx->exception = PLAYER_ERR_FILE_NOT_SUPPORTED;
            break;
        }
    } while (0);
    return r;
}


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


static void button_clicked_handler(lv_event_t *e)
{
    int event_id = (int)lv_event_get_user_data(e);
    EQ_QUICK_PUSH(event_id);
}


static void create_screen(player_t* ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void*)ctx);
    // All buttons used as "Button" device
    lvi_disable_keypad();
    lv_group_remove_all_objs(lvi_keypad_group);
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
    lv_obj_add_style(btn, &lvs_invisible_btn, 0);
    lv_obj_add_style(btn, &lvs_invisible_btn, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void*)EVT_PLAYER_PLAY_CLICKED);
    // NW / Back
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_btn, 0);
    lv_obj_add_style(btn, &lvs_invisible_btn, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 1, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void*)EVT_BACK_CLICKED);
    // SW / Setting
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_btn, 0);
    lv_obj_add_style(btn, &lvs_invisible_btn, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 2, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void*)EVT_SETTING_CLICKED);
    // NE / Up
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_btn, 0);
    lv_obj_add_style(btn, &lvs_invisible_btn, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 3, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void*)EVT_PLAYER_UP_CLICKED);
    // SE / Down
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_btn, 0);
    lv_obj_add_style(btn, &lvs_invisible_btn, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 4, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void*)EVT_PLAYER_DOWN_CLICKED);
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
            ctx->first_song = true;
            ctx->next_dir = 0;  // default to play next
            ctx->timer_general = 0;
            ctx->decoder = 0;
            EQ_QUICK_PUSH(EVT_PLAYER_PLAY_SONG);
            break;
        }
        case EVT_EXIT:
        {
            PL_LOGD("Player: exit\n");
            tick_disarm_timer_event(ctx->timer_ui_update);
            ctx->timer_ui_update = 0;
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
            if (CAT_OK != catalog_get_entry(me->catalog, ctx->file, FF_LFN_BUF + 1, 0))
            {
                ctx->exception = PLAYER_ERR_FILE_NOT_ACCESSIBLE;
                STATE_TRAN(me, &me->player_exp);
                break;
            }
            lv_label_set_text(ctx->lbl_bottom, ctx->file);
            // save history
            me->catalog_history_page[me->catalog_history_index] = me->catalog->cur_page;
            me->catalog_history_selection[me->catalog_history_index] = me->catalog->cur_index;
            PL_LOGD("Player: setup song %s\n", ctx->file);
            if (!setup_song(ctx))
            {
                // exception was set inside setup_song
                STATE_TRAN(me, &me->player_exp);
                break;
            }
            PL_LOGD("Player: play song %s\n", ctx->file);
            ctx->first_song = false;
            break;
        }
        case EVT_PLAYER_PLAY_NEXT:
        {
            uint8_t type;
            int r;
            do
            {
                // Get the next file to play
                if (0 == ctx->next_dir)
                    r = catalog_get_next_entry(me->catalog, false, false, ctx->file, FF_LFN_BUF + 1, &type);
                else
                    r = catalog_get_prev_entry(me->catalog, false, false, ctx->file, FF_LFN_BUF + 1, &type);
                if ((CAT_OK == r) && (CAT_TYPE_FILE == type))
                {
                    // found a file
                    break;
                }
                if ((CAT_OK == r) && (CAT_TYPE_DIRECTORY == type))
                {
                    // found a directory
                    continue;
                }
                // all other errors
                break;
            } while (1);
            switch (r)
            {
                case CAT_OK:
                    EQ_QUICK_PUSH(EVT_PLAYER_PLAY_SONG);
                    break;
                case CAT_ERR_EOF:
                    PL_LOGD("Player: no more files\n");
                    // restore catalog cursor to the last song we have played / are playing
                    catalog_move_cursor(me->catalog, 
                        me->catalog_history_page[me->catalog_history_index], 
                        me->catalog_history_selection[me->catalog_history_index]);
                    catalog_get_entry(me->catalog, 0, 0, 0);
                    break;
                case CAT_ERR_FATFS:
                    if (disk_present())
                    {
                        PL_LOGD("Player: card error\n");
                        EQ_QUICK_PUSH(EVT_DISK_ERROR);
                    }
                    else
                    {
                        PL_LOGD("Player: card ejected\n");
                        EQ_QUICK_PUSH(EVT_DISK_EJECTED);
                    }
                    break;
                default:
                    PL_LOGD("Player: unhandled error looking for next file. Error code %d\n", r);
                    STATE_TRAN(me, &(me->browser));
                    break;
            }
            break;
        }
        case EVT_PLAYER_UP_CLICKED:
        {
            ctx->next_dir = 1;
            EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT);
            break;
        }
        case EVT_PLAYER_DOWN_CLICKED:
        {
            ctx->next_dir = 0;
            EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT);
            break;
        }
        case EVT_SETTING_CLICKED:
        {
            // Leave NW/SW control by buttons
            lvi_disable_button();
            lvi_pos_button(LVI_BUTTON_NW, 1, 0);
            lvi_pos_button(LVI_BUTTON_SW, 2, 0);
            lvi_disable_keypad();
            setting_create(me);
            STATE_TRAN((hsm_t*)me, &me->setting);
            break;
        }
        case EVT_SETTING_CLOSED:
        {
            // reenable buttons
            lvi_disable_button();
            lvi_pos_button(LVI_BUTTON_PLAY, 0, 0);
            lvi_pos_button(LVI_BUTTON_NW, 1, 0);
            lvi_pos_button(LVI_BUTTON_SW, 2, 0);
            lvi_pos_button(LVI_BUTTON_NE, 3, 0);
            lvi_pos_button(LVI_BUTTON_SE, 4, 0);
            break;
        }
        case EVT_BACK_CLICKED:
        {
            STATE_TRAN(me, &(me->browser_disk));
            break;
        }
        case EVT_DISK_ERROR:
        {
            STATE_TRAN((hsm_t*)me, &me->browser_baddisk);
            break;
        }
        case EVT_DISK_EJECTED:
        {
            STATE_TRAN((hsm_t*)me, &me->browser_nodisk);
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
                case PLAYER_ERR_FILE_NOT_SUPPORTED:
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
            if (ctx->timer_general)
            {
                tick_disarm_timer_event(ctx->timer_general);
                ctx->timer_general = 0;
            }
            break;
        }
        case EVT_PLAYER_GENERAL_TIMER:
        {
            lv_msgbox_close(ctx->mbx_alert); // will also delete msg_alert internally
            ctx->mbx_alert = 0;
            // If fail occurred at first song, fail to browser. Otherwise go to next song
            if (ctx->first_song)
            {
                STATE_TRAN(me, &(me->browser_disk));
            }
            else
            {
                STATE_TRAN(me, &(me->player));
                EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT);
            }
            break;
        }
        case EVT_PLAYER_UP_CLICKED:
        {
            // quickly finish the timer
            if (ctx->timer_general)
            {
                tick_disarm_timer_event(ctx->timer_general);
                ctx->timer_general = 0;
            }
            EQ_QUICK_PUSH(EVT_PLAYER_GENERAL_TIMER);
            ctx->next_dir = 1;
            break;
        }
        case EVT_PLAYER_DOWN_CLICKED:
        {
            // quickly finish the timer
            if (ctx->timer_general)
            {
                tick_disarm_timer_event(ctx->timer_general);
                ctx->timer_general = 0;
            }
            EQ_QUICK_PUSH(EVT_PLAYER_GENERAL_TIMER);
            ctx->next_dir = 0;
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
