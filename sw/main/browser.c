#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvinput.h"
#include "lvstyle.h"
#include "lvsupp.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "disk.h"
#include "path_utils.h"
#include "ec.h"
#include "catalog.h"
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


static void screen_event_handler(lv_event_t* e);
// H/W button handler
static void button_setting_handler(lv_event_t* e);
static void button_back_handler(lv_event_t* e);
// Handler for S/W buttons in file list
static void list_button_handler(lv_event_t* e);


enum 
{
    FILE_LIST_ENTRY_TYPE_FILE = 0,
    FILE_LIST_ENTRY_TYPE_DIR,
    FILE_LIST_ENTRY_TYPE_PARENT,
    FILE_LIST_ENTRY_TYPE_PAGEUP,
    FILE_LIST_ENTRY_TYPE_PAGEDOWN
};


//  mode
//  0: Populate a new directory. Restore page and selection if available
//  1: Populate an already opened directory. Go to the first entry of next page. (Page Down)
//  2: Populate an already opened directory. Go to the last entry of previous page. (Page Up)
static void populate_file_list(app_t *me, int mode)
{
    browser_t *ctx = &(me->browser_ctx);
    lv_obj_t *btn, *focus = 0;
    int btn_index = 0;
    int page = 0;
    int selection;
    
    MY_ASSERT(me->catalog != 0);

    switch (mode)
    {
        case 0:
        {
            // If history is available, will read page/selection from history, otherwise  use default
            if (me->catalog_history_index < CATALOG_HISTORY_DEPTH)
            {
                page = me->catalog_history_page[me->catalog_history_index];
                selection = me->catalog_history_selection[me->catalog_history_index];
            }
            else
            {
                page = 0;
                selection = 0;
            }
            break;
        }
        case 1: // Page Down
            page = me->catalog->cur_page;
            if (page + 1 < me->catalog->pages)
            {
                ++page;
                selection = 0;
            }
            break;
        case 2: // Page Up
            page = me->catalog->cur_page;
            if (page > 0)
            {
                --page;
                selection = -1;
            }
            break;
        default:
            break;
    }

    lv_obj_clean(ctx->lst_files);
    
    // If we are on the fist page of a sub directory, add ".."
    if ((0 == page) && (!path_is_root_dir(me->catalog->dir)))
    {
        btn = lv_list_add_btn_ex(ctx->lst_files, 0, "[..]", LV_LABEL_LONG_DOT);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PARENT);
        lv_obj_add_event_cb(btn, list_button_handler, LV_EVENT_ALL, (void*)ctx);
    }

    // 2nd page onwards, add "PgUp" button
    if (page > 0)
    {
        btn = lv_list_add_btn_ex(ctx->lst_files, 0, "[PgUp..]", LV_LABEL_LONG_DOT);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEUP);
        lv_obj_add_event_cb(btn, list_button_handler, LV_EVENT_ALL, (void*)ctx);
    }

    catalog_move_cursor(me->catalog, page, 0);
    uint8_t type;
    char name[FF_LFN_BUF + 3];
    while (CAT_OK == catalog_get_next_entry(me->catalog, true, false, name + 1, FF_LFN_BUF + 1, &type))
    {
        if (type == CAT_TYPE_DIRECTORY)
        {
            name[0] = '[';
            strcat(name, "]");
            btn = lv_list_add_btn_ex(ctx->lst_files, LV_SYMBOL_DIRECTORY" ", name, LV_LABEL_LONG_DOT);
            lv_obj_set_user_data(btn, (void *)(uint32_t)((btn_index << 16) | FILE_LIST_ENTRY_TYPE_DIR));
        }
        else if (type == CAT_TYPE_FILE)
        {
            btn = lv_list_add_btn_ex(ctx->lst_files, LV_SYMBOL_FILE_O" ", name + 1, LV_LABEL_LONG_DOT);
            lv_obj_set_user_data(btn, (void *)(uint32_t)((btn_index << 16) | FILE_LIST_ENTRY_TYPE_FILE));
        }
        if (btn_index == selection)
            focus = btn;
        ++btn_index;
        lv_obj_add_event_cb(btn, list_button_handler, LV_EVENT_ALL, (void*)ctx);
    }
    if (page < me->catalog->pages - 1)
    {
        btn = lv_list_add_btn_ex(ctx->lst_files, 0, "[PgDn..]", LV_LABEL_LONG_DOT);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEDOWN);
        lv_obj_add_event_cb(btn, list_button_handler, LV_EVENT_ALL, (void*)ctx);
    }
    if (-1 == selection)    // select last
    {
        focus = btn;    // btn points to the last button added
    }
    // Add all buttons to input group
    lv_group_remove_all_objs(lvi_keypad_group);
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
    lv_label_set_text(ctx->lbl_bottom, me->catalog->dir);
}


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


static void button_setting_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) 
    {
        EQ_QUICK_PUSH(EVT_BROWSER_SETTING_CLICKED);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        BR_LOGD("Browser: Setting button long pressed\n");
    }
}


static void button_back_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED) 
    {
        EQ_QUICK_PUSH(EVT_BROWSER_BACK_CLICKED);
    }
    else if (code == LV_EVENT_LONG_PRESSED) 
    {
        BR_LOGD("Browser: Back button long pressed\n");
    }
}


static void list_button_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    switch (code)
    {
        case LV_EVENT_SHORT_CLICKED:
        {
            event_t evt;
            evt.code = EVT_BROWSER_PLAY_CLICKED;
            evt.param = (void *)btn;
            event_queue_push_back(&evt, true);
            break;
        }
        case LV_EVENT_LONG_PRESSED:
        {
            BR_LOGD("Browser_File: Play long Pressed\n");
            break;
        }
        case LV_EVENT_FOCUSED:
        {
            lv_list_btn_set_label_long_mode(btn, LV_LABEL_LONG_SCROLL_CIRCULAR);
            break;
        }
        case LV_EVENT_DEFOCUSED:
        {
            lv_list_btn_set_label_long_mode(btn, LV_LABEL_LONG_DOT);
            break;
        }
        default:
            break;
    }
}


static void create_screen(browser_t* ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void*)ctx);
    // Cleanup Keypad
    lvi_disable_keypad();
    lv_group_remove_all_objs(lvi_keypad_group);
    // Setup Buttons, use NW/Back at pos(0,0), SW/Setting at pos(1,0)
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
    lv_obj_add_event_cb(btn, button_back_handler, LV_EVENT_ALL, (void*)ctx);
    // Invisible button at coordinate (1,0)
    lvi_pos_button(LVI_BUTTON_SW, 1, 0);
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 1, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_setting_handler, LV_EVENT_ALL, (void*)ctx);
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
    // Other ui elements
    ctx->focused = 0;
    ctx->bar_brignthess = 0;
    // Calculates all coordinates
    lv_obj_update_layout(ctx->screen);
    // Load screen
    lv_scr_load(ctx->screen);
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
            me->browser_ctx.alarm_ui_update = tick_arm_timer_event(UI_UPDATE_INTERVAL_MS, true, EVT_BROWSER_UI_UPDATE, true);
            break;
        case EVT_START:
            STATE_START(me, &me->browser_nodisk);   // default to nodisk state and wait card insertion
            break;
        case EVT_EXIT:
            BR_LOGD("Browser: exit\n");
            tick_disarm_timer_event(me->browser_ctx.alarm_ui_update);
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
    /* Events
        EVT_ENTRY:
            Set keypad map
            Create catalog from root directory or restore catalog history
            Populate file list
        EVT_EXIT:
            No action
        EVT_BROWSER_PLAY_CLICKED:
            If clicked on a directory, push history and navigate to that directory
            If clicked on file, transit to player state
            If clilck on [..], enqueue EVT_BROWSER_CLICKED event
            If click on Page Up/Down, navigate page
        EVT_BROWSER_BACK_CLICKED:
            Pop history and navigate to parent directory
        EVT_BROWSER_SETTING_CLICKED:
            Transit to browser_disk_brightness
        EVT_BROWSER_SETTING_CLOSED:
            Restore keypad map
            Bind input group to file list
            Restore file list focused item
        EVT_DISK_ERROR:
            Close catalog and clear history
            Transit to browser_baddisk state
        EVT_DISK_EJECTED:
            Close catalog and clear history
            Transit to browser_error state
    */
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            BR_LOGD("Browser_Disk: entry\n");
            // Map keypad keys
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_PLAY, LV_KEY_ENTER);  // PLAY -> Enter, triggers list button's callback action
            lvi_map_keypad(LVI_BUTTON_NE, LV_KEY_PREV);     // NE -> Prev, used in file list navigation
            lvi_map_keypad(LVI_BUTTON_SE, LV_KEY_NEXT);     // SE -> Next, used in file list navigation
            // build up file list box
            int t;
            if (0 == me->catalog)
            {
                // no directory catalog available, we must be entering from nodisk state
                ec_pause_watchdog();
                lv_label_set_text(ctx->lbl_bottom, "Loading...");
                lv_refr_now(NULL);
                t = disk_check_dir("/");
                ec_resume_watchdog();
                if ((1 == t) || (2 == t)) // root dir is accessible
                {
                    me->catalog_history_page[0] = 0;
                    me->catalog_history_selection[0] = 0;
                    me->catalog_history_index = 0;
                    ec_pause_watchdog();
                    lv_label_set_text(ctx->lbl_bottom, "Loading...");
                    lv_refr_now(NULL);
                    t = catalog_open_dir("/", 0, CATALOG_PAGER_SIZE, true, &(me->catalog));
                    ec_resume_watchdog();
                    if (CAT_OK == t)
                    {
                        populate_file_list(me, 0);
                    }
                    else
                    {
                        BR_LOGE("Browser_Disk: open catalog at \"/\" failed with %d\n", t);
                        if (CAT_ERR_FATFS == t)
                        {
                            FRESULT fr = catalog_get_fatfs_error();
                            BR_LOGE("Browser_Disk: FatFS error %s (%d)\n", disk_result_str(fr), fr);
                        }
                        EQ_QUICK_PUSH(EVT_DISK_ERROR);
                    }
                }
                else    // disk_check_dir error
                {
                    BR_LOGE("Browser_Disk: open root directory failed\n");
                    EQ_QUICK_PUSH(EVT_DISK_ERROR);
                }
            }
            else
            {
                // we already have directory catalog object (transit from player state)
                // make sure our directory is still valid
                ec_pause_watchdog();
                lv_label_set_text(ctx->lbl_bottom, "Loading...");
                lv_refr_now(NULL);
                t = disk_check_dir(me->catalog->dir);
                ec_resume_watchdog();
                switch (t)
                {
                case 1:     // current dir is valid and accessible
                    populate_file_list(me, 0);
                    break;
                case 2:     // current dir is not accessible but disk is ok (TODO: Possible?)
                    BR_LOGW("Browser_Disk: current dir not accessible, go to root\n");
                    catalog_close(me->catalog);
                    me->catalog = 0;
                    me->catalog_history_page[0] = 0;
                    me->catalog_history_selection[0] = 0;
                    me->catalog_history_index = 0;
                    ec_pause_watchdog();
                    lv_label_set_text(ctx->lbl_bottom, "Loading...");
                    lv_refr_now(NULL);
                    t = catalog_open_dir("/", 0, CATALOG_PAGER_SIZE, true, &(me->catalog));
                    ec_resume_watchdog();
                    if (CAT_OK == t)
                    {
                        populate_file_list(me, 0);
                    }
                    else
                    {
                        BR_LOGE("Browser_Disk: open catalog at \"/\" failed with %d\n", t);
                        if (CAT_ERR_FATFS == t)
                        {
                            FRESULT fr = catalog_get_fatfs_error();
                            BR_LOGE("Browser_Disk: FatFS error %s (%d)\n", disk_result_str(fr), fr);
                        }
                        EQ_QUICK_PUSH(EVT_DISK_ERROR);
                    }
                    break;
                default:    // disk not accessible
                    BR_LOGE("Browser_Disk: disk error\n");
                    EQ_QUICK_PUSH(EVT_DISK_ERROR);
                    break;
                }
            }
            break;
        }
        case EVT_EXIT:
        {
            BR_LOGD("Browser_Disk: exit\n");
            break;
        }
        case EVT_BROWSER_PLAY_CLICKED:
        {
            lv_obj_t* btn = (lv_obj_t*)(evt->param);
            const char* btn_text = lv_list_get_btn_text(ctx->lst_files, btn);
            uint32_t t = (uint32_t)lv_obj_get_user_data(btn);
            int entry_index = (int)(t >> 16);
            int type = (int)(t & 0xffff);
            char path[FF_LFN_BUF + 1];
            switch (type)
            {
                case FILE_LIST_ENTRY_TYPE_FILE:
                {
                    // Push state
                    me->catalog_history_page[me->catalog_history_index] = me->catalog->cur_page;
                    me->catalog_history_selection[me->catalog_history_index] = entry_index;
                    // Move catalog cursor to current selection and hand over to player
                    BR_LOGD("Browser_Disk: play entry %d in page %d\n", entry_index, me->catalog->cur_page);
                    catalog_move_cursor(me->catalog, me->catalog->cur_page, entry_index);
                    STATE_TRAN((hsm_t*)me, &me->player);
                    break;
                }
                case FILE_LIST_ENTRY_TYPE_DIR:
                {
                    // Enter new directory. Save current browsing history and create new catalog object
                    if (path_concatenate(me->catalog->dir, btn_text, path, FF_LFN_BUF + 1, true))
                    {
                        int save_page = me->catalog->cur_page;
                        catalog_close(me->catalog);
                        me->catalog = 0;
                        int t;
                        ec_pause_watchdog();
                        lv_label_set_text(ctx->lbl_bottom, "Loading...");
                        lv_refr_now(NULL);
                        t = catalog_open_dir(path, 0, CATALOG_PAGER_SIZE, true, &(me->catalog));
                        ec_resume_watchdog();
                        if (CAT_OK == t)
                        {
                            // Push state
                            me->catalog_history_page[me->catalog_history_index] = save_page;
                            me->catalog_history_selection[me->catalog_history_index] = entry_index;
                            ++(me->catalog_history_index);
                            // Select first page and first entry for the new directory
                            if (me->catalog_history_index < CATALOG_HISTORY_DEPTH)
                            {
                                me->catalog_history_page[me->catalog_history_index] = 0;
                                me->catalog_history_selection[me->catalog_history_index] = 0;
                            }
                            populate_file_list(me, 0);
                        }
                        else
                        {
                            BR_LOGE("Browser_Disk: open catalog at \"/\" failed with %d\n", t);
                            if (CAT_ERR_FATFS == t)
                            {
                                FRESULT fr = catalog_get_fatfs_error();
                                BR_LOGE("Browser_Disk: FatFS error %s (%d)\n", disk_result_str(fr), fr);
                            }
                            EQ_QUICK_PUSH(EVT_DISK_ERROR);
                        }
                    }
                    break;
                }
                case FILE_LIST_ENTRY_TYPE_PARENT:
                    // Same as back button
                    EQ_QUICK_PUSH(EVT_BROWSER_BACK_CLICKED);
                    break;
                case FILE_LIST_ENTRY_TYPE_PAGEUP:
                    populate_file_list(me, 2);
                    break;
                case FILE_LIST_ENTRY_TYPE_PAGEDOWN:
                    populate_file_list(me, 1);
                    break;
            }
            break;
        }
        case EVT_BROWSER_BACK_CLICKED:
        {
            // Go to parent directory
            char path[FF_LFN_BUF + 1];
            if (path_get_parent(me->catalog->dir, path, FF_LFN_BUF + 1))
            {
                catalog_close(me->catalog);
                me->catalog = 0;
                int t;
                ec_pause_watchdog();
                lv_label_set_text(ctx->lbl_bottom, "Loading...");
                lv_refr_now(NULL);
                t = catalog_open_dir(path, 0, CATALOG_PAGER_SIZE, true, &(me->catalog));
                ec_resume_watchdog();
                if (CAT_OK == t)
                {
                    // Pop history
                    --(me->catalog_history_index);
                    populate_file_list(me, 0);
                }
                else
                {
                    BR_LOGE("Browser_Disk: open catalog at \"/\" failed with %d\n", t);
                    if (CAT_ERR_FATFS == t)
                    {
                        FRESULT fr = catalog_get_fatfs_error();
                        BR_LOGE("Browser_Disk: FatFS error %s (%d)\n", disk_result_str(fr), fr);
                    }
                    EQ_QUICK_PUSH(EVT_DISK_ERROR);
                }
            }
            break;
        }
        case EVT_BROWSER_SETTING_CLICKED:
        {
            // before entering setting, save current focused file
            ctx->focused = 0;
            for (uint32_t i = 0; i < lv_obj_get_child_cnt(ctx->lst_files); ++i)
            {
                lv_obj_t* obj = lv_obj_get_child(ctx->lst_files, i);
                if (lv_obj_has_state(obj, LV_STATE_FOCUSED))
                {
                    ctx->focused = obj;
                }
            }
            STATE_TRAN(me, &(me->browser_disk_brightness));
            break;
        }
        case EVT_BROWSER_SETTING_CLOSED:
        {
            // restore keypad mapping and input group
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_PLAY, LV_KEY_ENTER);  // PLAY -> Enter, triggers list button's callback action
            lvi_map_keypad(LVI_BUTTON_NE, LV_KEY_PREV);     // NE -> Prev, used in file list navigation
            lvi_map_keypad(LVI_BUTTON_SE, LV_KEY_NEXT);     // SE -> Next, used in file list navigation
            lv_group_remove_all_objs(lvi_keypad_group);
            uint32_t cnt = lv_obj_get_child_cnt(ctx->lst_files);
            for (uint32_t i = 0; i < cnt; ++i)
            {
                lv_obj_t* obj = lv_obj_get_child(ctx->lst_files, i);
                lv_group_add_obj(lvi_keypad_group, obj);
            }
            // Set focus if necessary
            if (ctx->focused != NULL)
            {
                lv_group_focus_obj(ctx->focused);
            }
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
            r = evt;
            break;
    }
    return r;
}


static void brightness_barbox_event_handler(lv_event_t* e)
{
    browser_t* ctx = (browser_t*)lv_event_get_user_data(e);
    int32_t c = *((int32_t *)lv_event_get_param(e));
    switch (c)
    {
        case 'U':
            BR_LOGD("Brightness: Up\n");
            break;
        case 'D':
            BR_LOGD("Brigntness: Down\n");
            break;
        default:
            BR_LOGD("Brigntness: (%d)\n", c);
            break;
    }
}


event_t const *browser_disk_brightness_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            BR_LOGD("Browser_Disk_Brigntness: entry\n");
            ctx->bar_brignthess = lv_barbox_create(ctx->screen, 0, 100, 50);
            lv_obj_add_event_cb(ctx->bar_brignthess, brightness_barbox_event_handler, LV_EVENT_KEY, (void*)ctx);
            // Remap keypad and attach input group to the barbox
            lv_group_remove_all_objs(lvi_keypad_group);
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_NE, 'U');  // Remap NE -> character 'U'
            lvi_map_keypad(LVI_BUTTON_SE, 'D');  // Remap SE -> character 'D'
            lv_group_add_obj(lvi_keypad_group, ctx->bar_brignthess);
            break;
        }
        case EVT_EXIT:
        {
            BR_LOGD("Browser_Disk_Brigntness: exit\n");
            lv_barbox_close(ctx->bar_brignthess);
            ctx->bar_brignthess = 0;
            break;
        }
        case EVT_BROWSER_SETTING_CLICKED:
        case EVT_BROWSER_BACK_CLICKED:
        {
            STATE_TRAN(me, &(me->browser_disk));
            EQ_QUICK_PUSH(EVT_BROWSER_SETTING_CLOSED);
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


event_t const *browser_nodisk_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Clear file list
            Disable keypad
        EVT_EXIT:
            No action
        EVT_DISK_INSERTED:
            Transit to browser_disk state
        EVT_BROWSER_SETTING_CLICKED:
            Transit to browser_nodisk_brightness
        EVT_BROWSER_SETTING_CLOSED:
            Disable keypad
    */
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            BR_LOGD("Browser_Nodisk: entry\n");
            lv_label_set_text(me->browser_ctx.lbl_bottom, "No card");
            lv_obj_clean(me->browser_ctx.lst_files);
            lv_group_remove_all_objs(lvi_keypad_group);
            lvi_disable_keypad();
            catalog_close(me->catalog);
            me->catalog = 0;
            me->catalog_history_page[0] = 0;
            me->catalog_history_selection[0] = 0;
            me->catalog_history_index = 0;
            break;
        }
        case EVT_EXIT:
        {
            BR_LOGD("Browser_Nodisk: exit\n");
            break;
        }
        case EVT_DISK_INSERTED:
        {
            // when cur_dir is null browser_disk will start browser from root
            STATE_TRAN((hsm_t*)me, &me->browser_disk);
            break;
        }
        case EVT_BROWSER_SETTING_CLICKED:
        {
            STATE_TRAN(me, &(me->browser_nodisk_brightness));
            break;
        }
        case EVT_BROWSER_SETTING_CLOSED:
        {
            // restore keypad mapping and input group
            lvi_disable_keypad();
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


event_t const *browser_nodisk_brightness_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            BR_LOGD("Browser_Nodisk_Brigntness: entry\n");
            ctx->bar_brignthess = lv_barbox_create(ctx->screen, 0, 100, 50);
            lv_obj_add_event_cb(ctx->bar_brignthess, brightness_barbox_event_handler, LV_EVENT_KEY, (void*)ctx);
            // Remap keypad and attach input group to the barbox
            lv_group_remove_all_objs(lvi_keypad_group);
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_NE, 'U');  // Remap NE -> character 'U'
            lvi_map_keypad(LVI_BUTTON_SE, 'D');  // Remap SE -> character 'D'
            lv_group_add_obj(lvi_keypad_group, ctx->bar_brignthess);
            break;
        }
        case EVT_EXIT:
        {
            BR_LOGD("Browser_Nodisk_Brigntness: exit\n");
            lv_barbox_close(ctx->bar_brignthess);
            ctx->bar_brignthess = 0;
            break;
        }
        case EVT_BROWSER_SETTING_CLICKED:
        case EVT_BROWSER_BACK_CLICKED:
        {
            STATE_TRAN(me, &(me->browser_nodisk));
            EQ_QUICK_PUSH(EVT_BROWSER_SETTING_CLOSED);
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



event_t const *browser_baddisk_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Clear file list
            Disable keypad
        EVT_EXIT:
            No action
        EVT_DISK_EJECTED:
            Transit to browser_nodisk state
        EVT_BROWSER_SETTING_CLICKED:
            Transit to browser_baddisk_brightness
        EVT_BROWSER_SETTING_CLOSED:
            Disable keypad
    */
    event_t const *r = 0;
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            BR_LOGD("Browser_Baddisk: entry\n");
            lv_label_set_text(me->browser_ctx.lbl_bottom, "Card error");
            lv_obj_clean(me->browser_ctx.lst_files);
            lv_group_remove_all_objs(lvi_keypad_group);
            lvi_disable_keypad();
            catalog_close(me->catalog);
            me->catalog = 0;
            me->catalog_history_page[0] = 0;
            me->catalog_history_selection[0] = 0;
            me->catalog_history_index = 0;
            break;
        }
        case EVT_EXIT:
        {
            BR_LOGD("Browser_Baddisk: exit\n");
            break;
        }
        case EVT_DISK_EJECTED:
        {
            STATE_TRAN((hsm_t*)me, &me->browser_nodisk);
            break;
        }
        case EVT_BROWSER_SETTING_CLICKED:
        {
            STATE_TRAN(me, &(me->browser_baddisk_brightness));
            break;
        }
        case EVT_BROWSER_SETTING_CLOSED:
        {
            // restore keypad mapping and input group
            lvi_disable_keypad();
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


event_t const *browser_baddisk_brightness_handler(app_t *me, event_t const *evt)
{
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
        case EVT_ENTRY:
        {
            BR_LOGD("Browser_Baddisk_Brigntness: entry\n");
            ctx->bar_brignthess = lv_barbox_create(ctx->screen, 0, 100, 50);
            lv_obj_add_event_cb(ctx->bar_brignthess, brightness_barbox_event_handler, LV_EVENT_KEY, (void*)ctx);
            // Remap keypad and attach input group to the barbox
            lv_group_remove_all_objs(lvi_keypad_group);
            lvi_disable_keypad();
            lvi_map_keypad(LVI_BUTTON_NE, 'U');  // Remap NE -> character 'U'
            lvi_map_keypad(LVI_BUTTON_SE, 'D');  // Remap SE -> character 'D'
            lv_group_add_obj(lvi_keypad_group, ctx->bar_brignthess);
            break;
        }
        case EVT_EXIT:
        {
            BR_LOGD("Browser_Baddisk_Brigntness: exit\n");
            lv_barbox_close(ctx->bar_brignthess);
            ctx->bar_brignthess = 0;
            break;
        }
        case EVT_BROWSER_SETTING_CLICKED:
        case EVT_BROWSER_BACK_CLICKED:
        {
            STATE_TRAN(me, &(me->browser_baddisk));
            EQ_QUICK_PUSH(EVT_BROWSER_SETTING_CLOSED);
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
