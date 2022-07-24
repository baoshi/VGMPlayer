#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvstyle.h"
#include "lvsupp.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "ec.h"
#include "input.h"
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
};


enum
{
    SONG_TYPE_UNKNOWN = 0,
    SONG_TYPE_S16
};


// Setup audio and decoder using current selected file
// Return PLAYER_OK or failure code
static uint8_t check_song(const char *file)
{
    uint8_t r = SONG_TYPE_UNKNOWN;
    do
    {
        char ext[4];
        uint8_t type;
        // Get file extension
        if (!path_get_ext(file, ext, 4))
        {
            break;
        }
        if (0 == strcasecmp(ext, "s16"))
        {
            r = SONG_TYPE_S16;
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


static void player_map_buttons()
{
    // Map buttons
    input_disable_button_dev();
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_SETTING_CLICKED);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_BACK_CLICKED);
    input_map_button(INPUT_KEY_PLAY, 0, 0);
    input_map_button(INPUT_KEY_UP, 0, 0);
    input_map_button(INPUT_KEY_DOWN, 0, 0);
    input_enable_button_dev();
}


static void player_on_entry(player_t *ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void*)ctx);
    // Create virtual buttons
    input_create_buttons(ctx->screen);
    // Map buttons
    player_map_buttons();
    //
    // UI Elements
    //
    // Top label
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
    // Calculates all coordinates
    lv_obj_update_layout(ctx->screen);
    // Load screen
    lv_scr_load(ctx->screen);
    // Arm update timer    
    ctx->timer_ui_update = tick_arm_timer_event(UI_UPDATE_INTERVAL_MS, true, EVT_PLAYER_UI_UPDATE, true);
}


static void player_on_exit(player_t *ctx)
{
    tick_disarm_timer_event(ctx->timer_ui_update);
    ctx->timer_ui_update = 0;
    input_map_button(-1, 0, 0);
    input_disable_button_dev();
    input_delete_buttons();
}


static void player_on_ui_update(player_t *ctx)
{
    char buf[32];
    sprintf(buf, "C=%d B=%.1fv", ec_charge, ec_battery);
    lv_label_set_text(ctx->lbl_top, buf);
}


static void player_on_play_clicked(app_t *app, player_t *ctx)
{
    do
    {
        if (CAT_OK != catalog_get_entry(app->catalog, ctx->file, FF_LFN_BUF + 1, 0))
        {
            // Remove button mapping, alert will setup its own
            PL_LOGD("Player: Unable to load entry from catalog\n");
            input_disable_button_dev();
            alert_create(app, 0, "File not accessible", 2000, 0);
            break;
        }
        lv_label_set_text(ctx->lbl_bottom, ctx->file);
        // save history
        app->catalog_history_page[app->catalog_history_index] = app->catalog->cur_page;
        app->catalog_history_selection[app->catalog_history_index] = app->catalog->cur_index;
        PL_LOGD("Player: setup song %s\n", ctx->file);
        uint8_t type = check_song(ctx->file);
        switch (type)
        {
        case SONG_TYPE_UNKNOWN:
            // Remove button mapping, alert will setup its own
            input_disable_button_dev();
            alert_create(app, 0, "Unknown file type", 2000, 0);
            break;
        case SONG_TYPE_S16:
            STATE_TRAN(app, &(app->player_s16));
            break;
        }
    } while (0);
}


static void player_on_play_next(app_t *app, player_t *ctx)
{
    int r;
    do
    {
        // loop until find a file to play or error occurred
        uint8_t type;
        if (0 == ctx->nav_dir)
            r = catalog_get_next_entry(app->catalog, false, false, ctx->file, FF_LFN_BUF + 1, &type);
        else
            r = catalog_get_prev_entry(app->catalog, false, false, ctx->file, FF_LFN_BUF + 1, &type);
        if ((CAT_OK == r) && (CAT_TYPE_FILE == type))
        {
            // found a file
            break;
        }
        if ((CAT_OK == r) && (CAT_TYPE_DIRECTORY == type))
        {
            // found a directory, go next
            continue;
        }
        // all other errors
        break;
    } while (1);
    switch (r)
    {
    case CAT_OK:
        EQ_QUICK_PUSH(EVT_BUTTON_PLAY_CLICKED);
        break;
    case CAT_ERR_EOF:
        PL_LOGD("Player: no more files\n");
        // restore catalog cursor to the last song we have played / are playing
        catalog_move_cursor(app->catalog,
                            app->catalog_history_page[app->catalog_history_index],
                            app->catalog_history_selection[app->catalog_history_index]);
        catalog_get_entry(app->catalog, 0, 0, 0);   // dummy get_entry to fetch cache
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
        PL_LOGE("Player: unhandled error looking for next file. Error code %d\n", r);
        STATE_TRAN(app, &(app->browser));
        break;
    }
}


event_t const *player_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Create screen, enable buttons, arm UI update timer
        EVT_EXIT:
            Disarm UI update timer, disable buttons
        EVT_PLAYER_UI_UPDATE:
            Update top bar
        EVT_PLAY_CLICKED:
            Play song
        EVT_PLAYER_PLAY_NEXT:
            Find next file (can be next or previous depend on direction) and play
        EVT_PLAYER_UP_CLICKED:
            Set navigation direction to up (prev) and play next
        EVT_PLAYER_DOWN_CLICKED:
            Set navigation direction to down (next) and play next
        EVT_BUTTON_SETTING_CLICKED:
            Create settings state and transit to it
        EVT_SETTING_CLOSED:
            Save settings
        EVT_BACK_CLICKED:
            Transit to browser_disk
        EVT_ALERT_CLOSED:
            Remap buttons
        EVT_DISK_ERROR:
            Sent by play_next. Transit to browser_baddisk
        EVT_DISK_EJECTED:
            Sent by play_next or disk module. Transit to browser_nodisk
    */
    event_t const *r = 0;
    player_t *ctx = &(app->player_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        PL_LOGD("Player: entry\n");
        player_on_entry(ctx);
        ctx->nav_dir = 0;  // default to play next
        ctx->decoder = 0;
        EQ_QUICK_PUSH(EVT_BUTTON_PLAY_CLICKED);
        break;
    case EVT_EXIT:
        PL_LOGD("Player: exit\n");
        player_on_exit(ctx);
        break;
    case EVT_PLAYER_UI_UPDATE:
        player_on_ui_update(ctx);
        break;
    case EVT_BUTTON_PLAY_CLICKED:
        player_on_play_clicked(app, ctx);
        break;
    case EVT_PLAYER_PLAY_NEXT:
        player_on_play_next(app, ctx);
        break;
    case EVT_BUTTON_UP_CLICKED:
        ctx->nav_dir = 1;
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT);
        break;
    case EVT_BUTTON_DOWN_CLICKED:
        ctx->nav_dir = 0;
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT);
        break;
    case EVT_BUTTON_SETTING_CLICKED:
        // retain SETTING and BACK buttons, leave others to setting
        //input_disable_button(INPUT_KEY_PLAY);
        //input_disable_button(INPUT_KEY_UP);
        //input_disable_button(INPUT_KEY_DOWN);
        setting_create(app);
        break;
    case EVT_SETTING_CLOSED:
        // reenable buttons
        player_map_buttons();
        break;
    case EVT_BUTTON_BACK_CLICKED:
        STATE_TRAN(app, &(app->browser_disk));
        break;
    case EVT_ALERT_CLOSED:
        PL_LOGD("Player: alert closed\n");
        player_map_buttons();
        break;
    case EVT_DISK_ERROR:
        STATE_TRAN((hsm_t*)app, &app->browser_baddisk);
        break;
    case EVT_DISK_EJECTED:
        STATE_TRAN((hsm_t*)app, &app->browser_nodisk);
        break;
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
        PL_LOGD("Player_S16: start\n");
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
    case EVT_BUTTON_PLAY_CLICKED:
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
    case EVT_BUTTON_PLAY_CLICKED:
        audio_unpause_playback();
        STATE_TRAN(me, &(me->player_s16_playing));
        break;
    case EVT_BUTTON_BACK_CLICKED:
        audio_stop_playback();
        STATE_TRAN(me, &(me->browser_disk));
        break;
    default:
        r = evt;
        break;
    }
    return r;
}