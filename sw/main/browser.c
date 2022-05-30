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
#include "lister.h"
#include "app.h"

#define UI_UPDATE_INTERVAL_MS   200

#ifndef BROWSER_DEBUG
# define BROWSER_DEBUG 1
#endif


// Debug log
#if BROWSER_DEBUG
#define BR_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define BR_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define BR_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define BR_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define BR_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define BR_LOGD(x, ...)
#define BR_LOGI(x, ...)
#define BR_LOGW(x, ...)
#define BR_LOGE(x, ...)
#define BR_DEBUGF(x, ...)
#endif


enum 
{
    FILE_LIST_ENTRY_TYPE_FILE   = 0,
    FILE_LIST_ENTRY_TYPE_DIR,
    FILE_LIST_ENTRY_TYPE_PARENT,
    FILE_LIST_ENTRY_TYPE_PAGEUP,
    FILE_LIST_ENTRY_TYPE_PAGEDOWN
};


static void populate_file_list(browser_t* ctx, int mode);


static void screen_event_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (ctx)
    {
        switch (code)
        {
            case LV_EVENT_SCREEN_UNLOADED:
                BR_LOGD("Browser: screen unloaded\n");
                lv_obj_del(ctx->screen);
                break;
            default:
                break;
        }
    }
}


static void back_button_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_SHORT_CLICKED) 
    {
        EQ_QUICK_PUSH(EVT_BROWSER_BACK_CLICKED);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        BR_LOGD("Browser: mode button long pressed\n");
    }
}


static void play_button_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ctx->skip_first_click)
        {
            // User can long press play button to exit to browser state. 
            // In this case skip_first_click was set to true and we shall skip first
            // SHORT_CLICKED event
            ctx->skip_first_click = false;
        }
        else
        {
            event_t evt;
            evt.code = EVT_BROWSER_PLAY_CLICKED;
            evt.param = (void *)btn;
            event_queue_push_back(&evt, true);
        }
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        if (ctx->skip_first_click)
        {
            // User can long press play button to exit to browser state. 
            // In this case skip_first_click was set to true and we shall skip first
            // SHORT_CLICKED event
            ctx->skip_first_click = false;
        }
        else
        {
            BR_LOGD("Browser: Play long Pressed\n");
        }
    }
}


static void create_screen(browser_t* ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void*)ctx);
    // Setup Keypad, use NE->Prev, SE->Next, PLAY->Enter
    lvi_clear_keypad_group();
    lvi_disable_keypad();
    lvi_map_keypad(LVI_BUTTON_PLAY, LV_KEY_ENTER);  // LV_KEY_ENTER triggers list button's callback action
    lvi_map_keypad(LVI_BUTTON_NE, LV_KEY_PREV);     // Navigation in the list box
    lvi_map_keypad(LVI_BUTTON_SE, LV_KEY_NEXT);     // Navigation in the list box
    // Setup Buttons, use NW at pos(0,0), SW at pos(1,0), both for "back" function
    lvi_disable_button();
    lv_obj_t* btn;
    // Invisible button at coordinate (0,0)
    lvi_pos_button(LVI_BUTTON_NW, 0, 0);
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, back_button_handler, LV_EVENT_ALL, (void*)ctx);
    // Invisible button at coordinate (1,0)
    lvi_pos_button(LVI_BUTTON_SW, 1, 0);
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 1, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, back_button_handler, LV_EVENT_ALL, (void*)ctx);
    // Create top label
    ctx->lbl_top = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_top, 200);
    lv_obj_set_style_text_align(ctx->lbl_top, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(ctx->lbl_top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_label_set_text(ctx->lbl_top, "");
    // Create status label
    ctx->lbl_bottom = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_bottom, 240);
    lv_obj_align(ctx->lbl_bottom, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text(ctx->lbl_bottom, "");
    lv_label_set_long_mode(ctx->lbl_bottom, LV_LABEL_LONG_SCROLL_CIRCULAR);
    // Create file list
    ctx->lst_files = lv_list_create(ctx->screen);
    lv_obj_set_size(ctx->lst_files, 240, 192);
    lv_obj_set_pos(ctx->lst_files, 0, 24);
    // Load screen
    lv_scr_load(ctx->screen);
}


//  mode
//  0: Populate a new directory. Restore page and selection if available
//  1: Populate an already opened directory. Go to next first entry of next page. (Page Down)
//  2: Populate an already opened directory. Go to the last entry of previous page. (Page Up)
static void populate_file_list(browser_t* ctx, int mode)
{
    lv_obj_t *btn, *focus = 0;
    int btn_index = 0;
    int selection;
    bool dir_opened = false;

    switch (mode)
    {
        case 0:
        {
            int t;
            ec_pause_watchdog();
            t = lister_open_dir(ctx->cur_dir, 0, BROWSER_LISTER_PAGE_SIZE, true, &ctx->lister);
            ec_resume_watchdog();
            if (LS_OK == t)
            {
                dir_opened = true;
                // If history is available, will read page/selection from history, otherwise  use default
                if (ctx->lister_history_index < BROWSER_LISTER_HISTORY_DEPTH)
                {
                    ctx->lister_cur_page = ctx->lister_history_page[ctx->lister_history_index];
                    selection = ctx->lister_history_selection[ctx->lister_history_index];
                }
                else
                {
                    ctx->lister_cur_page = 0;
                    selection = 0;
                }
            }
            else
            {
                BR_LOGE("Browser_Disk: open dir %s failed with %d\n", ctx->cur_dir, t);
                if (LS_ERR_FATFS == t)
                {
                    FRESULT fr = lister_get_fatfs_error();
                    BR_LOGE("Browser_Disk: FatFS error %s (%d)\n", disk_result_str(fr), fr);
                }
            }
            break;
        }
        case 1: // Page Down
            if (ctx->lister)
            {
                dir_opened = true;
                if (ctx->lister_cur_page + 1 < ctx->lister->pages)
                {
                    ++(ctx->lister_cur_page);
                    selection = 0;
                }
            }
            break;
        case 2: // Page Up
            if (ctx->lister)
            {
                dir_opened = true;
                if (ctx->lister_cur_page > 0)
                {
                    --(ctx->lister_cur_page);
                    selection = -1;
                }
            }
            break;
        default:
            break;
    }

    lv_obj_clean(ctx->lst_files);
    
    // If we are on the fist page of a sub directory, add ".."
    if ((0 == ctx->lister_cur_page) && (!path_is_root_dir(ctx->cur_dir)))
    {
        btn = lv_list_add_btn(ctx->lst_files, 0, "[..]");
        ++btn_index;
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PARENT);
        lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
    }

    if (dir_opened)
    {
        lister_select_page(ctx->lister, ctx->lister_cur_page);
        // 2nd page onwards, add "PgUp" button
        if (ctx->lister_cur_page > 0)
        {
            btn = lv_list_add_btn(ctx->lst_files, 0, "[PgUp..]");
            ++btn_index;
            lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEUP);
            lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
        }
        for (int i = 0; i < ctx->lister->page_size; ++i)
        {
            uint8_t type;
            char name[FF_LFN_BUF + 3];
            if (LS_OK == lister_get_entry(ctx->lister, i, name + 1, FF_LFN_BUF + 1, &type))
            {
                if (type == LS_TYPE_DIRECTORY)
                {
                    name[0] = '[';
                    strcat(name, "]");
                    btn = lv_list_add_btn(ctx->lst_files, LV_SYMBOL_DIRECTORY" ", name);
                    lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_DIR);
                }
                else if (type == LS_TYPE_FILE)
                {
                    btn = lv_list_add_btn(ctx->lst_files, LV_SYMBOL_FILE_O" ", name + 1);
                    lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_FILE);
                }
                if (btn_index == selection)
                    focus = btn;
                ++btn_index;
                lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
            }
        }
        if (ctx->lister_cur_page < ctx->lister->pages - 1)
        {
            btn = lv_list_add_btn(ctx->lst_files, 0, "[PgDn..]");
            ++btn_index;
            lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEDOWN);
            lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
        }
        if (-1 == selection)    // select last
        {
            focus = btn;    // btn points to the last button added
        }
    }
    // Add all buttons to input group
    uint32_t cnt = lv_obj_get_child_cnt(ctx->lst_files);
    for (uint32_t i = 0; i < cnt; ++i)
    {
        lv_obj_t* obj = lv_obj_get_child(ctx->lst_files, i);
        lv_group_add_obj(lvi_keypad_group, obj);
    }
    // Set focus if necessary
    if (focus != NULL)
    {
        lv_group_focus_obj(focus);
    }
    // Update status bar
    lv_label_set_text(ctx->lbl_bottom, ctx->cur_dir);
}


event_t const *browser_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
            BR_LOGD("Browser: entry\n");
            create_screen(ctx);
            me->browser_ctx.alarm_ui_update = tick_arm_time_event(UI_UPDATE_INTERVAL_MS, true, EVT_BROWSER_UI_UPDATE, true);
            break;
        case EVT_START:
            STATE_START(me, &me->browser_nodisk);   // default to nodisk state and wait card insertion
            break;
        case EVT_EXIT:
            tick_disarm_time_event(me->browser_ctx.alarm_ui_update);
            ctx->alarm_ui_update = -1;
            break;
        case EVT_BROWSER_UI_UPDATE:
        {
            char buf[32];
            sprintf(buf, "C=%d B=%.1fv", ec_charge, ec_battery);
            lv_label_set_text(ctx->lbl_top, buf);
            break;
        }
        default:
            r = evt;
            break;
    }
    return r;
}


event_t const *browser_disk_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
            BR_LOGD("Browser_Disk: entry\n");
            if (path_is_null(ctx->cur_dir))
            {
                // if cur_dir is empty, we are entering browser state for the first time
                // browser to root directory
                path_set_root_dir(ctx->cur_dir);
                ctx->lister_cur_page = 0;
                ctx->lister_history_page[0] = 0;
                ctx->lister_history_selection[0] = 0;
                ctx->lister_history_index = 0;
            }
            ec_pause_watchdog();
            int t = disk_check_dir(ctx->cur_dir);   // Could take several seconds before failure
            ec_resume_watchdog();
            switch (t)
            {
                case 1:
                    // cur_dir is valid and accessible
                    populate_file_list(ctx, 0);
                    break;
                case 2:
                    // cur_dir is not accessible but disk is readable, browse root instead
                    path_set_root_dir(ctx->cur_dir);
                    ctx->lister_cur_page = 0;
                    ctx->lister_history_page[0] = 0;
                    ctx->lister_history_selection[0] = 0;
                    ctx->lister_history_index = 0;
                    populate_file_list(ctx, 0);
                    break;
                default:
                    // disk not accessible
                    EQ_QUICK_PUSH(EVT_DISK_ERROR);
                    break;
            }
            break;
        case EVT_EXIT:
            if (ctx->lister)
            {
                lister_close(ctx->lister);
                ctx->lister = 0;
            }
            break;
        case EVT_BROWSER_PLAY_CLICKED:
        {
            lv_obj_t* btn = (lv_obj_t*)(evt->param);
            const char* btn_text = lv_list_get_btn_text(ctx->lst_files, btn);
            char path[FF_LFN_BUF + 1];
            if (btn)
            {
                int type = (int)lv_obj_get_user_data(btn);
                switch (type)
                {
                    case FILE_LIST_ENTRY_TYPE_FILE:
                    {
                        // save state
                        ctx->lister_history_page[ctx->lister_history_index] = ctx->lister_cur_page;
                        ctx->lister_history_selection[ctx->lister_history_index] = lv_obj_get_index(btn);
                        event_t evt;
                        evt.code = EVT_BROWSER_PLAY_FILE;
                        evt.param = (void *)btn;
                        event_queue_push_back(&evt, true);
                        break;
                    }
                    case FILE_LIST_ENTRY_TYPE_DIR:
                        if (path_concatenate(ctx->cur_dir, btn_text, path, FF_LFN_BUF + 1, true))
                        {
                            if (ctx->lister)
                            {
                                lister_close(ctx->lister);
                                ctx->lister = 0;
                            }
                            // push history
                            ctx->lister_history_selection[ctx->lister_history_index] = lv_obj_get_index(btn);
                            ++(ctx->lister_history_index);
                            // set new directory and select to first page first entry
                            path_copy(path, ctx->cur_dir, FF_LFN_BUF + 1);
                            if (ctx->lister_history_index < BROWSER_LISTER_HISTORY_DEPTH)
                            {
                                ctx->lister_history_page[ctx->lister_history_index] = 0;
                                ctx->lister_history_selection[ctx->lister_history_index] = 0;
                            }
                            populate_file_list(ctx, 0);
                        }
                        break;
                    case FILE_LIST_ENTRY_TYPE_PARENT:
                        if (path_get_parent(ctx->cur_dir, path, FF_LFN_BUF + 1))
                        {
                            if (ctx->lister)
                            {
                                lister_close(ctx->lister);
                                ctx->lister = 0;
                            }
                            // pop history
                            --(ctx->lister_history_index);
                            path_copy(path, ctx->cur_dir, FF_LFN_BUF + 1);
                            populate_file_list(ctx, 0);
                        }
                        break;
                    case FILE_LIST_ENTRY_TYPE_PAGEUP:
                        populate_file_list(ctx, 2);
                        break;
                    case FILE_LIST_ENTRY_TYPE_PAGEDOWN:
                        populate_file_list(ctx, 1);
                        break;
                }
            }
            break;
        }
        case EVT_BROWSER_BACK_CLICKED:
            if (!path_is_root_dir(ctx->cur_dir))
            {
                char path[FF_LFN_BUF + 1];
                if (path_get_parent(ctx->cur_dir, path, FF_LFN_BUF + 1))
                {
                    if (ctx->lister)
                    {
                        lister_close(ctx->lister);
                        ctx->lister = 0;
                    }
                    // pop history
                    --(ctx->lister_history_index);
                    path_copy(path, ctx->cur_dir, FF_LFN_BUF + 1);
                    populate_file_list(ctx, 0);
                }
            }
            break;
        case EVT_BROWSER_PLAY_FILE:
        {
            lv_obj_t* btn = (lv_obj_t*)(evt->param);
            const char* file = lv_list_get_btn_text(ctx->lst_files, btn);
            if (path_concatenate(ctx->cur_dir, file, me->player_ctx.file, FF_LFN_BUF + 1, false))
            {
                BR_LOGD("Browser_Disk: selected %s\n", file);
                STATE_TRAN((hsm_t*)me, &me->player);
            }
            else
            {
                BR_LOGE("Browser_Disk: file path too deep to play: %s\n", file);
            }
            break;
        }
        case EVT_DISK_ERROR:
            STATE_TRAN((hsm_t*)me, &me->browser_baddisk);
            break;
        case EVT_DISK_EJECTED:
            STATE_TRAN((hsm_t*)me, &me->browser_nodisk);
            break;
        default:
            r = evt;
            break;
    }
    return r;
}


event_t const *browser_nodisk_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
            BR_LOGD("Browser_Nodisk: entry\n");
            lv_label_set_text(me->browser_ctx.lbl_bottom, "No card");
            lv_obj_clean(me->browser_ctx.lst_files);
            path_set_null(me->browser_ctx.cur_dir);
            break;
        case EVT_DISK_INSERTED:
        {
            // when cur_dir is null browser_disk will start browser from root
            path_set_null(me->browser_ctx.cur_dir);
            STATE_TRAN((hsm_t*)me, &me->browser_disk);
            break;
        }
        default:
            r = evt;
            break;
    }
    return r;
}


event_t const *browser_baddisk_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
            BR_LOGD("Browser_Baddisk: entry\n");
            lv_label_set_text(me->browser_ctx.lbl_bottom, "Card error");
            lv_obj_clean(me->browser_ctx.lst_files);
            path_set_root_dir(me->browser_ctx.cur_dir);
            break;
        case EVT_DISK_EJECTED:
            STATE_TRAN((hsm_t*)me, &me->browser_nodisk);
            break;
        default:
            r = evt;
    }
    return r;
}


