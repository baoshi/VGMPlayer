#undef PLAYER_TIMING_MEASUREMENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pico/mutex.h>
#ifdef PLAYER_TIMING_MEASUREMENT
# include <inttypes.h>
# include <pico/time.h>
#endif
#include "sw_conf.h"
#include "my_debug.h"
#include "lvstyle.h"
#include "lvsupp.h"
#include "lvassets.h"
#include "lvspectrum.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "ec.h"
#include "input.h"
#include "backlight.h"
#include "disk.h"
#include "path_utils.h"
#include "audio.h"
#include "decoder_s16.h"
#include "decoder_vgm.h"
#include "fft_q15.h"
#include "catalog.h"
#include "popup.h"
#include "app.h"


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
    SONG_TYPE_S16,
    SONG_TYPE_VGM
};


// Spectrum related
#define PLAYER_SPECTRUM_BINS    16      // Do not exceed PLAYER_SPECTRUM_MAX_BINS in app.h
#if (PLAYER_SPECTRUM_BINS > PLAYER_SPECTRUM_MAX_BINS)
# error("Too many spectrum bins")
#endif
// AUDIO_FRAME_LENGTH = 2048
// AUDIO_SAMPLE_RATE = 44110
// 2048 FFT points spanning 0-44100 Hz. Useful points is 1024 (0-22050Hz)
// Accumulate 4 samples for 16 bins, we only take first 64 points (1378Hz)
#define PLAYER_SPECTRUM_AVERAGE_SAMPLES  4
// To shrink bin sum to 0-255 range
#define PLAYER_SPECTRUM_MUL          2
#define PLAYER_SPECTRUM_DIV          7


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
        if (0 == strcasecmp(ext, "vgm"))
        {
            r = SONG_TYPE_VGM;
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


static void player_setup_input()
{
    // Buttons
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_OPEN_VOLUME_POPUP);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_BACK_CLICKED);
    input_map_button(INPUT_KEY_PLAY, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_PLAY_CLICKED);
    input_map_button(INPUT_KEY_UP, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_UP_CLICKED);
    input_map_button(INPUT_KEY_DOWN, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_DOWN_CLICKED);
    input_enable_button_dev();
    // Keypad
    input_map_keypad(-1, 0);
    input_disable_keypad_dev();
}


static void player_on_entry(player_t *ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void*)ctx);
    //
    // UI Elements
    //
    // ------------------------------------------------------------------------------------------------------------
    //                                                                                                            0
    //  Top bar 26px
    //                                                                                                           25
    // ------------------------------------------------------------------------------------------------------------
    //                                                                                                           26
    //  Custom Controls 240x120
    //                                                                                                          145
    // ------------------------------------------------------------------------------------------------------------
    //                                                                                                          146
    //  Spectrum
    //                                                                                                          217
    // ------------------------------------------------------------------------------------------------------------
    //                                                                                                          218
    //  Progress Bar 5px
    //                                                                                                          222
    // ------------------------------------------------------------------------------------------------------------
    //                                                                                                          223
    //  Progress Text, line_height = 21, base_line = 4, treat as hight 17 since numbers are not below base line
    //                                                                                                          239
    // ------------------------------------------------------------------------------------------------------------

    // Top bar: 26px high, right side has battery icon (26 pixels wide, leave 30 pixels at least)
    // Font line height is 21, pad 2 pixel top down.
    // Top bar icon is 13x13
    ctx->img_top = lv_img_create(ctx->screen);
    lv_obj_set_pos(ctx->img_top, 1, 7);
    ctx->lbl_top = lv_label_create(ctx->screen);
    lv_obj_set_pos(ctx->lbl_top, 20, 3);
    lv_obj_set_size(ctx->lbl_top, 180, 21);
    lv_obj_set_style_text_align(ctx->lbl_top, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(ctx->lbl_top, LV_LABEL_LONG_DOT);
    
    // Custom controls container
    ctx->pnl_custom_ctrls = lv_obj_create(ctx->screen);
    lv_obj_set_pos(ctx->pnl_custom_ctrls, 0, 26);
    lv_obj_set_size(ctx->pnl_custom_ctrls, 240, 120);
    lv_obj_set_scrollbar_mode(ctx->pnl_custom_ctrls, LV_SCROLLBAR_MODE_OFF);    // never show scrollbar
    
    // Spectrum
    ctx->spectrum = lv_spectrum_create(ctx->screen);
    lv_obj_set_pos(ctx->spectrum, 0, 146);
    lv_obj_set_size(ctx->spectrum, 240, 72);
    lv_obj_set_style_pad_top(ctx->spectrum, 3, 0);
    lv_obj_set_style_pad_left(ctx->spectrum, 5, 0);
    lv_obj_set_style_pad_right(ctx->spectrum, 5, 0);
    lv_obj_set_style_pad_bottom(ctx->spectrum, 3, 0);
    
    // Progress bar
    ctx->bar_progress = lv_bar_create(ctx->screen);
    lv_obj_set_pos(ctx->bar_progress, 5, 218);
    lv_obj_set_size(ctx->bar_progress, 230, 5);
    lv_obj_set_style_pad_all(ctx->bar_progress, 0, 0);
    lv_obj_set_style_border_width(ctx->bar_progress, 0, 0);
    lv_obj_set_style_radius(ctx->bar_progress, 0, 0);
    lv_bar_set_range(ctx->bar_progress, 0, 100);

    // Play time label
    ctx->lbl_play_time = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_play_time, 80);
    lv_obj_set_pos(ctx->lbl_play_time, 5, 223);
    lv_obj_set_style_text_align(ctx->lbl_play_time, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(ctx->lbl_play_time, LV_LABEL_LONG_DOT);
    // Remain time label
    ctx->lbl_remaining_time = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_remaining_time, 80);
    lv_obj_set_pos(ctx->lbl_remaining_time, 155, 223);
    lv_obj_set_style_text_align(ctx->lbl_remaining_time, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_long_mode(ctx->lbl_remaining_time, LV_LABEL_LONG_DOT);
    // Calculates all coordinates
    lv_obj_update_layout(ctx->screen);
    // Create virtual buttons
    input_create_buttons(ctx->screen);
    // Setup input
    player_setup_input();
    // Load screen
    lv_scr_load(ctx->screen);
    // Init other data members
    ctx->played = 0;
    ctx->nav_dir = 1;  // default to play next
    ctx->decoder = 0;
}


static void player_on_start(player_t *ctx)
{
    lv_label_set_text(ctx->lbl_top, "");
    lv_spectrum_set_reset(ctx->spectrum);
    lv_bar_set_value(ctx->bar_progress, 0, LV_ANIM_OFF);
    lv_label_set_text(ctx->lbl_play_time, "00:00");
    lv_label_set_text(ctx->lbl_remaining_time, "00:00");
}


static void player_on_exit(player_t *ctx)
{
    input_delete_buttons();
    event_queue_clear();
}


static void player_on_play_clicked(app_t *app, player_t *ctx)
{
    do
    {
        char fn[FF_MAX_LFN];
        // Don't want any popups on the screen
        close_all_popups(app);
        // Get file name at cursor
        if (CAT_OK != catalog_get_entry(app->catalog, ctx->file, FF_LFN_BUF + 1, true, NULL))
        {
            PL_LOGD("Player: Unable to load entry from catalog\n");
            alert_popup(app, NULL, "File not accessible", 2000, EVT_PLAYER_ALERT_CLOSED);
            break;
        }
        lv_img_set_src(ctx->img_top, &img_player_play);
        // save history
        app->catalog_history_page[app->catalog_history_index] = app->catalog->cur_page;
        app->catalog_history_selection[app->catalog_history_index] = app->catalog->cur_index;
        PL_LOGD("Player: verify song %s\n", ctx->file);
        uint8_t type = check_song(ctx->file);
        switch (type)
        {
        case SONG_TYPE_UNKNOWN:
            alert_popup(app, NULL, "Unknown file type", 2000, EVT_PLAYER_ALERT_CLOSED);
            break;
        case SONG_TYPE_S16:
            STATE_TRAN(app, &(app->player_s16));
            break;
        case SONG_TYPE_VGM:
            STATE_TRAN(app, &(app->player_vgm));
            break;
        }
    } while (0);
}


static void player_on_play_next(app_t *app, player_t *ctx, bool stay)
{
    int r;
    do
    {
        // loop until find a file to play or error occurred
        uint8_t type;
        MY_ASSERT(ctx->nav_dir != 0);   // nav_dir == 0 means go back to browser.
        if (1 == ctx->nav_dir)
            r = catalog_get_next_entry(app->catalog, false, false, ctx->file, FF_LFN_BUF + 1, true, &type);
        else
            r = catalog_get_prev_entry(app->catalog, false, false, ctx->file, FF_LFN_BUF + 1, true, &type);
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
        catalog_get_entry(app->catalog, 0, 0, false, NULL);   // dummy get_entry to fetch cache
        if (stay)
        {
            // If stay is true, show alert (happens if user click down when at last file)
            alert_popup(app, NULL, "No more file", 1000, 0);
        }
        else
        {
            // stay is false, happens if last song is finished natually
            STATE_TRAN(app, &(app->browser_disk));
        }
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
        EQ_QUICK_PUSH(EVT_DISK_ERROR);
        break;
    }
}



static void player_on_progress(app_t *app, player_t *ctx, audio_progress_t *progress)
{   
    char buf[32];
    int sec1, sec2, percent;
    sec1 = progress->played_samples / AUDIO_SAMPLE_RATE;
    sec2 = (progress->total_samples - progress->played_samples) / AUDIO_SAMPLE_RATE;
    sprintf(buf, "%02d:%02d", sec1 / 60, sec1 % 60);
    lv_label_set_text(ctx->lbl_play_time, buf);
    if (progress->total_samples > progress->played_samples)
        sprintf(buf, "-%02d:%02d", sec2 / 60, sec2 % 60);
    else
        sprintf(buf, "%02d:%02d", sec2 / 60, sec2 % 60);
    lv_label_set_text(ctx->lbl_remaining_time, buf);
    percent = progress->played_samples * 100 / progress->total_samples;
    lv_bar_set_value(ctx->bar_progress, percent, LV_ANIM_OFF);
    //PL_LOGD("Player: %lu / %lu\n", progress->played_samples, progress->total_samples);
    // Do not dim screen when playing
    backlight_keepalive(tick_millis());
    // "Trying" FFT on the sampling buffer for spectrum display
    // We lock the sampling buffer here for long time so audio module won't overwrite the samples.
    // It doesn't matter some buffer are lost since we only use these for fancy visual effects.
    if (mutex_try_enter(&(audio_sampling_buffer.lock), NULL))
    {
        if (audio_sampling_buffer.good)
        {
            int index, temp;
#ifdef PLAYER_TIMING_MEASUREMENT            
            absolute_time_t start = get_absolute_time();
#endif
            // audio sample is in int16_t, can be treated as q15_t [-1..1) without conversion for FFT
            fft_q15(audio_sampling_buffer.buffer, AUDIO_FRAME_LENGTH);
            index = 2; // start from 1, skip DC-20Hz
            for (int bin = 0; bin < PLAYER_SPECTRUM_BINS; ++bin)
            {
                temp = 0;
                for (int i = 0; i < PLAYER_SPECTRUM_AVERAGE_SAMPLES; ++i)
                {
                    temp += audio_sampling_buffer.buffer[index];
                    ++index;
                }
                temp = temp * PLAYER_SPECTRUM_MUL / PLAYER_SPECTRUM_DIV;
                if (temp > 255)
                    ctx->spectrum_bin[bin] = 255;
                else
                    ctx->spectrum_bin[bin] = (uint8_t)temp;
            }
            lv_spectrum_set_bin_values(ctx->spectrum, ctx->spectrum_bin, PLAYER_SPECTRUM_BINS);
            audio_sampling_buffer.good = false;
#ifdef PLAYER_TIMING_MEASUREMENT            
            absolute_time_t end = get_absolute_time();
            int64_t us = absolute_time_diff_us(start, end);
            PL_LOGD("Player: FFT finished in %" PRId64 " us\n", us);
#endif
        }
        mutex_exit(&(audio_sampling_buffer.lock));
    }
}



event_t const *player_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Create screen, setup input
            Send EVT_PLAY_CLICKED
        EVT_EXIT:
            Disable buttons
        EVT_START:
            Set UI controls to empty value
        EVT_BUTTON_PLAY_CLICKED:
            Play song
        EVT_PLAYER_PLAY_NEXT_OR_STAY:
            Find next file (can be next or previous depend on direction) and play
        EVT_PLAYER_PLAY_NEXT_OR_BACK:
            Find next file and play, or if not found, go back browser_disk
        EVT_BUTTON_UP_CLICKED:
            Set navigation direction to up (prev) and play next
        EVT_BUTTON_DOWN_CLICKED:
            Set navigation direction to down (next) and play next
        EVT_OPEN_VOLUME_POPUP:
            Open volume popup
        EVT_CLOSE_VOLUME_POPUP:
            Close volume popup
        EVT_CLOSE_VOLUME_POPUP_CONT:
            Close volume popup and enqueue EVT_OPEN_BRIGHTNESS_POPUP (to be handled by top state)
        EVT_BUTTON_BACK_CLICKED:
            Transit to browser_disk
        EVT_PLAYER_ALERT_CLOSED:
            If alert(error) occurs during playing of first song, exit to Browser_Disk.
            Otherwise send EVT_PLAYER_PLAY_NEXT_OR_STAY
        EVT_DISK_ERROR:
            Sent by play_next. Transit to browser_baddisk
        EVT_DISK_EJECTED:
            Sent by play_next or disk module. Transit to browser_nodisk
        EVT_HEADPHONE_PLUGGED:
        EVT_HEADPHONE_UNPLUGGED:
            Refresh volume popup if it is already shown
        EVT_AUDIO_PROGRESS:
            Draw spectrum, update play time
    */
    event_t const *r = 0;
    player_t *ctx = &(app->player_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        PL_LOGD("Player: entry\n");
        player_on_entry(ctx);
        // Simulate first play button click
        // Not sending this during EVT_START because EVT_START is also
        // received when exit from Player_xxx state to Player state.
        EQ_QUICK_PUSH(EVT_BUTTON_PLAY_CLICKED);
        break;
    case EVT_EXIT:
        player_on_exit(ctx);
        PL_LOGD("Player: exit\n");
        break;
    case EVT_START:
        PL_LOGD("Player: start\n");
        player_on_start(ctx);
        break;
    case EVT_BUTTON_PLAY_CLICKED:
        player_on_play_clicked(app, ctx);
        break;
    case EVT_PLAYER_PLAY_NEXT_OR_STAY:
        player_on_play_next(app, ctx, true);
        break;
    case EVT_PLAYER_PLAY_NEXT_OR_BACK:
        player_on_play_next(app, ctx, false);
        break;
    case EVT_BUTTON_UP_CLICKED:
        ctx->nav_dir = -1;
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT_OR_STAY);
        break;
    case EVT_BUTTON_DOWN_CLICKED:
        ctx->nav_dir = 1;
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT_OR_STAY);
        break;
    case EVT_OPEN_VOLUME_POPUP:
        volume_popup(app);
        break;
    case EVT_CLOSE_VOLUME_POPUP:
        volume_close(app);
        break;
    case EVT_CLOSE_VOLUME_POPUP_CONT:
        volume_close(app);
        EQ_QUICK_PUSH(EVT_OPEN_BRIGHTNESS_POPUP);
        break;
    case EVT_BUTTON_BACK_CLICKED:
        STATE_TRAN(app, &(app->browser_disk));
        break;
    case EVT_PLAYER_ALERT_CLOSED:
        if (ctx->played > 0)
            EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT_OR_STAY);
        else
            STATE_TRAN(app, &(app->browser_disk));
        break;
    case EVT_DISK_ERROR:
        STATE_TRAN((hsm_t*)app, &app->browser_baddisk);
        break;
    case EVT_DISK_EJECTED:
        STATE_TRAN((hsm_t*)app, &app->browser_nodisk);
        break;
    case EVT_HEADPHONE_PLUGGED:
        // no break
    case EVT_HEADPHONE_UNPLUGGED:
        if (is_volume_popup(app))
        {
            volume_popup_refresh(app);
        }
        break;
    case EVT_AUDIO_PROGRESS:
        MY_ASSERT(evt->param != NULL);
        player_on_progress(app, ctx, (audio_progress_t *)(evt->param));
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


static void player_s16_setup_input()
{
    // Buttons
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_OPEN_VOLUME_POPUP);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_BACK_CLICKED);
    input_map_button(INPUT_KEY_PLAY, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_PLAY_CLICKED);
    input_map_button(INPUT_KEY_UP, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_UP_CLICKED);
    input_map_button(INPUT_KEY_DOWN, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_DOWN_CLICKED);
    input_enable_button_dev();
    // Keypad
    input_map_keypad(-1, 0);
    input_disable_keypad_dev();
}


event_t const *player_s16_handler(app_t *app, event_t const *evt)
{
    event_t const *r = 0;
    player_t *ctx = &(app->player_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        PL_LOGD("Player_S16: entry\n");
        player_s16_setup_input();
        path_get_leaf(ctx->file, ctx->name, FF_LFN_BUF);
        lv_label_set_text(ctx->lbl_top, ctx->name);
        ctx->playing = false;
        app->busy = false;
        break;
    case EVT_START:
        PL_LOGD("Player_S16: start\n");
        MY_ASSERT(ctx->decoder == 0);
        ctx->decoder = (decoder_t *)decoder_s16_create(ctx->file);
        // TODO: Handle error here
        MY_ASSERT(ctx->decoder != NULL);
        audio_start_playback(ctx->decoder);
        ++ctx->played;
        ctx->playing = true;
        ctx->nav_dir = 1;   // default next song direction
        app->busy = true;
        break;
    case EVT_EXIT:
        audio_finish_playback();
        if (ctx->decoder)
        {
            decoder_s16_destroy((decoder_s16_t *)(ctx->decoder));
            ctx->decoder = 0;
        }
        lv_obj_clean(ctx->pnl_custom_ctrls);
        app->busy = false;
        if (config_is_dirty())
        {
            config_save();
        }
        PL_LOGD("Player_S16: audio finished\n");
        PL_LOGD("Player_S16: exit\n");
        break;
    case EVT_BUTTON_PLAY_CLICKED:
        PL_LOGD("Player_S16: play clicked\n");
        if (ctx->playing)
        {
            lv_img_set_src(ctx->img_top, &img_player_pause);
            audio_pause_playback();
            ctx->playing = false;
        } 
        else
        {
            lv_img_set_src(ctx->img_top, &img_player_play);
            audio_resume_playback();
            ctx->playing = true;
        }
        break;
    case EVT_BUTTON_BACK_CLICKED:
        PL_LOGD("Player_S16: back clicked\n");
        audio_stop_playback();
        // Set navigation back, wait event EVT_AUDIO_SONG_ENDED to go back to browser_disk
        ctx->nav_dir = 0;
        break;
    case EVT_BUTTON_UP_CLICKED:
        PL_LOGD("Player_S16: up clicked\n");
        audio_stop_playback();
        // Set navigation direction to -1, wait event EVT_AUDIO_SONG_ENDED to play next
        ctx->nav_dir = -1;
        break;
    case EVT_BUTTON_DOWN_CLICKED:
        PL_LOGD("Player_S16: down clicked\n");
        audio_stop_playback();
        // Set navigation direction to 1, wait event EVT_AUDIO_SONG_ENDED to play next
        ctx->nav_dir = 1;
        break;
    case EVT_AUDIO_SONG_FINISHED:
        PL_LOGD("Player_S16: song finished\n");
        PL_LOGD("Player_S16: go to next song\n");
        STATE_TRAN(app, &(app->player));
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT_OR_BACK);
        break;
    case EVT_AUDIO_SONG_TERMINATED:
        PL_LOGD("Player_S16: song terminated\n");
        // Song is terminiated if press back/up/down
        if (0 == ctx->nav_dir)
        {
            // back pressed
            PL_LOGD("Player_S16: go back to browser_disk\n");
            STATE_TRAN(app, &(app->browser_disk));
        }
        else
        {
            // up/down pressed
            PL_LOGD("Player_S16: go to next song\n");
            STATE_TRAN(app, &(app->player));
            EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT_OR_STAY);
        }
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


static void player_vgm_setup_input()
{
    // Buttons
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_OPEN_VOLUME_POPUP);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_BACK_CLICKED);
    input_map_button(INPUT_KEY_PLAY, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_PLAY_CLICKED);
    input_map_button(INPUT_KEY_UP, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_UP_CLICKED);
    input_map_button(INPUT_KEY_DOWN, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_DOWN_CLICKED);
    input_enable_button_dev();
    // Keypad
    input_map_keypad(-1, 0);
    input_disable_keypad_dev();
}


static void player_vgm_on_entry(app_t *app, player_t *ctx)
{
    decoder_vgm_t *vd = NULL;
    PL_LOGD("Player_VGM: entry\n");
    player_vgm_setup_input();
    ctx->playing = false;
    app->busy = false;
    MY_ASSERT(ctx->decoder == 0);
    vd = decoder_vgm_create(ctx->file);
    MY_ASSERT(vd != NULL);
    //TODO: Handle error
    // space for VGM controls in pnl_custom_ctrls (230x92)
    /*
    // track name label
    lv_obj_t *lbl_vgm_track = lv_label_create(ctx->pnl_custom_ctrls);
    lv_obj_set_size(lbl_vgm_track, 230, 23);    // full width, font line height + 2
    lv_obj_set_pos(lbl_vgm_track, 0, 0);
    lv_obj_set_style_pad_all(lbl_vgm_track, 1, 0);
    lv_obj_set_style_bg_color(lbl_vgm_track, COLOR_DARK, 0);
    lv_obj_set_style_bg_opa(lbl_vgm_track, LV_OPA_COVER, 0);
    lv_obj_set_style_text_align(lbl_vgm_track, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(lbl_vgm_track, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(lbl_vgm_track, vd->vgm->track_name_en);
    */
    lv_label_set_text(ctx->lbl_top, vd->vgm->track_name_en);
    // game name label, have space 92-23=69, minus gap 5 at top, 64
    /*
    lv_obj_t *space = lv_obj_create(ctx->pnl_custom_ctrls);
    lv_obj_set_size(space, 230, 64);
    lv_obj_set_pos(space, 0, 23 + 5);
    lv_obj_set_scrollbar_mode(space, LV_SCROLLBAR_MODE_OFF);    // never show scrollbar
    lv_obj_t *lbl_vgm_game = lv_label_create(space);
    lv_obj_set_width(lbl_vgm_game, 230);
    lv_obj_set_style_text_align(lbl_vgm_game, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(lbl_vgm_game, LV_LABEL_LONG_WRAP);
    lv_label_set_text(lbl_vgm_game, vd->vgm->game_name_en);
    lv_obj_align(lbl_vgm_game, LV_ALIGN_CENTER, 0, 0);
    lv_obj_refr_size(space);
    // Check if game name label exceed game name panel. If true, cut game name text to fit
    int hl = lv_obj_get_height(lbl_vgm_game);
    int hr = lv_obj_get_height(space);
    if (hl > hr)
    {
        lv_point_t pos;
        pos.x = 0; pos.y = hr;
        uint32_t index = lv_label_get_letter_on(lbl_vgm_game, &pos);
        lv_label_cut_text(lbl_vgm_game, index, UINT32_MAX);   // Undocumented! pass in max number so lvgl will cut till end
    }
    */
    // Try find if there is a thumbnail image avaliable
    char temp[FF_LFN_BUF + 1], sjpg[FF_LFN_BUF + 1];
    bool have_thumb = false;
    // 1. If there is a file with same vgm file name but extension is ".sjpg"
    if (path_concatenate("0:", ctx->file, temp, FF_LFN_BUF + 1, false) && path_change_ext(temp, "sjpg", sjpg, FF_LFN_BUF + 1))
    {
        FIL fd;
        if (FR_OK == f_open(&fd, sjpg, FA_READ | FA_OPEN_EXISTING))
        {
            f_close(&fd);
            have_thumb = true;
        }
    }
    // 2. If there is a file named "thumb.sjpg" in the same folder as the vgm file
    if (!have_thumb)
    {
        if (path_concatenate("0:", ctx->file, sjpg, FF_LFN_BUF + 1, false) 
            &&
            path_get_parent(sjpg, temp, FF_LFN_BUF + 1)
            &&
            path_concatenate(temp, "thumb.sjpg", sjpg, FF_LFN_BUF + 1, false)
        )
        {
            FIL fd;
            if (FR_OK == f_open(&fd, sjpg, FA_READ | FA_OPEN_EXISTING))
            {
                f_close(&fd);
                have_thumb = true;
            }  
        }
    }
    if (have_thumb)
    {
        lv_obj_t *thumb = lv_img_create(ctx->pnl_custom_ctrls);
        lv_img_set_src(thumb, sjpg);
        lv_obj_align(thumb, LV_ALIGN_CENTER, 0, 0);
        lv_obj_refr_size(ctx->pnl_custom_ctrls);
    }
    else
    {
        lv_obj_t *space = ctx->pnl_custom_ctrls;
        lv_obj_t *lbl_vgm_game = lv_label_create(space);
        lv_obj_set_width(lbl_vgm_game, 230);
        lv_obj_set_style_text_align(lbl_vgm_game, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(lbl_vgm_game, LV_LABEL_LONG_WRAP);
        lv_label_set_text(lbl_vgm_game, vd->vgm->game_name_en);
        lv_obj_align(lbl_vgm_game, LV_ALIGN_CENTER, 0, 0);
        lv_obj_refr_size(space);
        // If game name label exceed panel, cut short the name
        int hl = lv_obj_get_height(lbl_vgm_game);
        int hr = lv_obj_get_height(space);
        if (hl > hr)
        {
            lv_point_t pos;
            pos.x = 0; pos.y = hr;
            uint32_t index = lv_label_get_letter_on(lbl_vgm_game, &pos);
            lv_label_cut_text(lbl_vgm_game, index, UINT32_MAX);   // Undocumented! pass in max number so lvgl will cut till end
        }
    }
    // start play
    ++ctx->played;
    ctx->playing = true;
    ctx->nav_dir = 1;   // default next song direction
    app->busy = true;
    ctx->decoder = (decoder_t *)vd;
    audio_start_playback(ctx->decoder);
}


static void player_vgm_on_exit(app_t *app, player_t *ctx)
{
    audio_finish_playback();
    if (ctx->decoder)
    {
        decoder_vgm_destroy((decoder_vgm_t *)(ctx->decoder));
        ctx->decoder = 0;
    }
    lv_obj_clean(ctx->pnl_custom_ctrls);
    app->busy = false;
    if (config_is_dirty())
    {
        config_save();
    }
    PL_LOGD("Player_VGM: audio finished\n");
    PL_LOGD("Player_VGM: exit\n");
}


event_t const *player_vgm_handler(app_t *app, event_t const *evt)
{
    event_t const *r = 0;
    player_t *ctx = &(app->player_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        player_vgm_on_entry(app, ctx);
        break;
    case EVT_EXIT:
        player_vgm_on_exit(app, ctx);
        break;
    case EVT_BUTTON_PLAY_CLICKED:
        PL_LOGD("Player_VGM: play clicked\n");
        if (ctx->playing)
        {
            lv_img_set_src(ctx->img_top, &img_player_pause);
            audio_pause_playback();
            ctx->playing = false;
        } 
        else
        {
            lv_img_set_src(ctx->img_top, &img_player_play);
            audio_resume_playback();
            ctx->playing = true;
        }
        break;
    case EVT_BUTTON_BACK_CLICKED:
        PL_LOGD("Player_VGM: back clicked\n");
        audio_stop_playback();
        // Set navigation back, wait event EVT_AUDIO_SONG_TERMINATED to go back to browser_disk
        ctx->nav_dir = 0;
        break;
    case EVT_BUTTON_UP_CLICKED:
        PL_LOGD("Player_VGM: up clicked\n");
        audio_stop_playback();
        // Set navigation direction to -1, wait event EVT_AUDIO_SONG_TERMINATED to play next
        ctx->nav_dir = -1;
        break;
    case EVT_BUTTON_DOWN_CLICKED:
        PL_LOGD("Player_VGM: down clicked\n");
        audio_stop_playback();
        // Set navigation direction to 1, wait event EVT_AUDIO_SONG_TERMINATED to play next
        ctx->nav_dir = 1;
        break;
    case EVT_AUDIO_SONG_FINISHED:
        PL_LOGD("Player_VGM: song finished\n");
        PL_LOGD("Player_VGM: go to next song\n");
        STATE_TRAN(app, &(app->player));
        EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT_OR_BACK);
        break;
    case EVT_AUDIO_SONG_TERMINATED:
        PL_LOGD("Player_VGM: song terminated\n");
        // Song is terminiated if press back/up/down
        if (0 == ctx->nav_dir)
        {
            // back pressed
            PL_LOGD("Player_VGM: go back to browser_disk\n");
            STATE_TRAN(app, &(app->browser_disk));
        }
        else
        {
            // up/down pressed
            PL_LOGD("Player_VGM: go to next song\n");
            STATE_TRAN(app, &(app->player));
            EQ_QUICK_PUSH(EVT_PLAYER_PLAY_NEXT_OR_STAY);
        }
        break;
    default:
        r = evt;
        break;
    }
    return r;
}