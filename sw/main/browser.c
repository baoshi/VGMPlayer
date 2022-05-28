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


static void chdir_down(browser_t* ctx, const char* leaf);
static void chdir_up(browser_t* ctx);
static void populate_file_list(browser_t* ctx);


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
        event_t evt;
        evt.code = EVT_BROWSER_PLAY_CLICKED;
        evt.param = (void *)btn;
        event_queue_push_back(&evt, true);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        BR_LOGD("Browser: Play long Pressed\n");
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


static void chdir_down(browser_t* ctx, const char* leaf)
{
    FRESULT fr;
    bool success = false;
    char dir[FF_LFN_BUF + 1];
    do
    {
        if (!path_concatenate(ctx->cur_dir, leaf, dir, FF_LFN_BUF + 1, true))
        {
            BR_LOGW("Browser: Path error\n");
            break;
        }
        fr = f_chdir(dir);
        if (FR_OK != fr)
        {
            BR_LOGW("Browser: f_chdir error %s (%d)\n", disk_result_str(fr), fr);
            break;
        }
        strncpy(ctx->cur_dir, dir, sizeof(ctx->cur_dir));   // save new dir
        success = true; // populate_file_list will send EVT_DISK_ERROR internally
        populate_file_list(ctx);
    } while (0);
    if (!success)
    {
        EQ_QUICK_PUSH(EVT_DISK_ERROR);
    }
}


static void chdir_up(browser_t* ctx)
{
    FRESULT fr;
    bool success = false;
    char dir[FF_LFN_BUF + 1];
    do
    {
        if (!path_get_parent(ctx->cur_dir, dir, FF_LFN_BUF + 1))
        {
            BR_LOGW("Browser: Path error\n");
            break;
        }
        fr = f_chdir(dir);
        if (FR_OK != fr)
        {
            BR_LOGW("Browser: f_chdir error %s (%d)\n", disk_result_str(fr), fr);
            break;
        }
        path_get_leaf(ctx->cur_dir, ctx->cur_selection); // save where we were on
        strncpy(ctx->cur_dir, dir, sizeof(ctx->cur_dir));
        success = true; // populate_file_list will send EVT_DISK_ERROR internally
        populate_file_list(ctx);
    } while (0);
    if (!success)
    {
        EQ_QUICK_PUSH(EVT_DISK_ERROR);
    }
}


static void populate_file_list(browser_t* ctx)
{
    lv_obj_t* btn;
    lv_obj_clean(ctx->lst_files);
    // If we are on the fist page of directory lister, add ".." if current directory
    // is a sub directory
    if ((ctx->lister_page == 0) && (!path_is_root_dir(ctx->cur_dir)))
    {
        btn = lv_list_add_btn(ctx->lst_files, 0, "[..]");
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PARENT);
        lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
    }
    int t = LS_OK;
    if (0 == ctx->lister)
    {
        ec_pause_watchdog();
        t = lister_open_dir(ctx->cur_dir, 0, 10, true, &ctx->lister);
        ec_resume_watchdog();
    }
    if (LS_OK == t)
    {
        lister_select_page(ctx->lister, ctx->lister_page);
        if (ctx->lister_page > 0)
        {
            btn = lv_list_add_btn(ctx->lst_files, 0, "[PgUp..]");
            lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEUP);
            lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
        }
        uint8_t type;
        for (int i = 0; i < ctx->lister->page_size; ++i)
        {
            char name[FF_LFN_BUF + 3];
            lv_obj_t* btn;
            if (LS_OK == lister_get_entry(ctx->lister, i, name + 1, FF_LFN_BUF + 1, &type))
            {
                if (type == LS_TYPE_DIRECTORY)
                {
                    name[0] = '[';
                    strcat(name, "]");
                    btn = lv_list_add_btn(ctx->lst_files, LV_SYMBOL_DIRECTORY" ", name);
                    lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_DIR);
                }
                else
                {
                    btn = lv_list_add_btn(ctx->lst_files, LV_SYMBOL_FILE_O" ", name + 1);
                    lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_FILE);
                }
            }
            lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
        }
        if (ctx->lister_page < ctx->lister->pages - 1)
        {
            btn = lv_list_add_btn(ctx->lst_files, 0, "[PgDn..]");
            lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEDOWN);
            lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
        }
    }
    // Add all buttons to input group
    uint32_t cnt = lv_obj_get_child_cnt(ctx->lst_files);
    for (uint32_t i = 0; i < cnt; ++i)
    {
        lv_obj_t* obj = lv_obj_get_child(ctx->lst_files, i);
        lv_group_add_obj(lvi_keypad_group, obj);
    }
    // Update status bar
    lv_label_set_text(ctx->lbl_bottom, ctx->cur_dir);
}


static void populate_file_list1(browser_t* ctx)
{
    FRESULT fr;
    const char *p_dir;
    p_dir = ctx->cur_dir;
    lv_obj_clean(ctx->lst_files);
    // Enumerate directory contents and add into list box
    DIR dj;  // Directory object
    FILINFO fno; // File information
    memset(&dj, 0, sizeof(dj));
    memset(&fno, 0, sizeof(fno));
    fr = f_findfirst(&dj, &fno, p_dir, "*");
    if (FR_OK != fr)
    {
        BR_LOGW("Browser: f_findfirst error %s (%d)\n", disk_result_str(fr), fr);
        event_t e;
        EQ_QUICK_PUSH(EVT_DISK_ERROR);
    }
    char dir_name[FF_LFN_BUF + 3];
    int idx_files = 0;                  // index of first file button
    int total = 0;
    lv_obj_t* focus = NULL;
    while (fr == FR_OK && fno.fname[0])  // Repeat while an item is found
    {
        ++total;
        lv_obj_t* btn;
        if (fno.fattrib & AM_DIR)
        {
            sprintf(dir_name, "[%s]", fno.fname);
            btn = lv_list_add_btn(ctx->lst_files, LV_SYMBOL_DIRECTORY" ", dir_name);
            lv_obj_set_user_data(btn, (void *)1);   // User data 1 means directory
            ++idx_files;
            int i = 0;
            for (; i < idx_files - 1; ++i)
            {
                lv_obj_t* btn = lv_obj_get_child(ctx->lst_files, i);
                const char* dir = lv_list_get_btn_text(ctx->lst_files, btn);
                if (strcasecmp(dir, dir_name) > 0)
                    break;
            }
            lv_obj_move_to_index(btn, i);
        }
        else
        {
            btn = lv_list_add_btn(ctx->lst_files, LV_SYMBOL_FILE_O" ", fno.fname);
            lv_obj_set_user_data(btn, (void *)0);   // User data 0 means file
            // Simple sort
            int i = idx_files;
            for (; i < total - 1; ++i)
            {
                lv_obj_t* btn = lv_obj_get_child(ctx->lst_files, i);
                const char* name = lv_list_get_btn_text(ctx->lst_files, btn);
                if (strcasecmp(name, fno.fname) > 0)
                    break;
            }
            lv_obj_move_to_index(btn, i);
        }
        lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
        if (ctx->cur_selection[0] != '\0' && (strcasecmp(ctx->cur_selection, fno.fname) == 0))
        {
            focus = btn;
        }
        if (total >= BROWSER_MAX_LIST_FILES)    // Limit list size
        {
            break;
        }
        fr = f_findnext(&dj, &fno); // Search for next item
    }
    f_closedir(&dj);
    // If current directory is a subdirectory, add '..' the list
    if (!path_is_root_dir(ctx->cur_dir))
    {
        lv_obj_t* btn = lv_list_add_btn(ctx->lst_files, 0, "[..]");
        lv_obj_set_user_data(btn, (void *)1);   // User data 1 means directory
        lv_obj_add_event_cb(btn, play_button_handler, LV_EVENT_ALL, (void*)ctx);
        lv_obj_move_to_index(btn, 0);
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
                // if cur_dir is never set, we are entering browser state for the first time
                path_set_root_dir(ctx->cur_dir);
            }
            ec_pause_watchdog();
            int t = disk_check_dir(ctx->cur_dir);   // Could take several seconds before failure
            ec_resume_watchdog();
            switch (t)
            {
                case 1:
                    // cur_dir is valid and accessible
                    populate_file_list(ctx);
                    break;
                case 2:
                    // cur_dir is not accessible but disk is readable, browse root instead
                    path_set_root_dir(ctx->cur_dir);
                    ctx->cur_selection[0] = '\0';
                    populate_file_list(ctx);
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
                        strcpy(ctx->cur_selection, btn_text);   // save selection
                        EQ_QUICK_PUSH(EVT_BROWSER_PLAY_FILE);   // fire play selection event
                        break;
                    case FILE_LIST_ENTRY_TYPE_DIR:
                        if (path_concatenate(ctx->cur_dir, btn_text, path, FF_LFN_BUF + 1, true))
                        {
                            if (ctx->lister)
                            {
                                lister_close(ctx->lister);
                                ctx->lister = 0;
                                ctx->lister_page = 0;
                            }
                            path_copy(path, ctx->cur_dir, FF_LFN_BUF + 1);
                            populate_file_list(ctx);
                        }
                        break;
                    case FILE_LIST_ENTRY_TYPE_PARENT:
                        if (path_get_parent(ctx->cur_dir, path, FF_LFN_BUF + 1))
                        {
                            if (ctx->lister)
                            {
                                lister_close(ctx->lister);
                                ctx->lister = 0;
                                ctx->lister_page = 0;
                            }
                            path_copy(path, ctx->cur_dir, FF_LFN_BUF + 1);
                            populate_file_list(ctx);
                        }
                        break;
                    case FILE_LIST_ENTRY_TYPE_PAGEUP:
                        --(ctx->lister_page);
                        populate_file_list(ctx);
                        break;
                    case FILE_LIST_ENTRY_TYPE_PAGEDOWN:
                        ++(ctx->lister_page);
                        populate_file_list(ctx);
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
                        ctx->lister_page = 0;
                    }
                    path_copy(path, ctx->cur_dir, FF_LFN_BUF + 1);
                    populate_file_list(ctx);
                }
            }
            break;
        case EVT_BROWSER_PLAY_FILE:
            BR_LOGD("Browser_Disk: selected %s\n", ctx->cur_selection);
            strcpy(me->player_ctx.file, ctx->cur_selection);
            STATE_TRAN((hsm_t*)me, &me->player);
            break;
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
            path_set_null(me->browser_ctx.cur_selection);
            break;
        case EVT_DISK_INSERTED:
            STATE_TRAN((hsm_t*)me, &me->browser_disk);
            break;
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
            me->browser_ctx.cur_selection[0] = '\0';
            break;
        case EVT_DISK_EJECTED:
            STATE_TRAN((hsm_t*)me, &me->browser_nodisk);
            break;
        default:
            r = evt;
    }
    return r;
}


