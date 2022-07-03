#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvinput.h"
#include "lvtheme.h"
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

#define UI_UPDATE_INTERVAL_MS 200

#ifndef BROWSER_DEBUG
#define BROWSER_DEBUG 1
#endif

// Debug log
#if BROWSER_DEBUG
#define BR_LOGD(x, ...) MY_LOGD(x, ##__VA_ARGS__)
#define BR_LOGI(x, ...) MY_LOGI(x, ##__VA_ARGS__)
#define BR_LOGW(x, ...) MY_LOGW(x, ##__VA_ARGS__)
#define BR_LOGE(x, ...) MY_LOGE(x, ##__VA_ARGS__)
#define BR_DEBUGF(x, ...) MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define BR_LOGD(x, ...)
#define BR_LOGI(x, ...)
#define BR_LOGW(x, ...)
#define BR_LOGE(x, ...)
#define BR_DEBUGF(x, ...)
#endif


//
// State Browser
//


static void screen_event_handler(lv_event_t *e)
{
    browser_t *ctx = (browser_t *)lv_event_get_user_data(e);
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


// Callback for on-screen button event
static void button_clicked_handler(lv_event_t *e)
{
    int event_id = (int)lv_event_get_user_data(e);
    EQ_QUICK_PUSH(event_id);
}


static void browser_on_entry(browser_t *ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void *)ctx);
    // Keypad input device not used initially
    lvi_disable_keypad();
    lv_group_remove_all_objs(lvi_keypad_group);
    //
    // Invisible on-screen buttons
    // Cord     Button
    // (0, 0)   [BACK]
    // (1, 0)   [SETTING]
    //
    lv_obj_t *btn;
    // (0, 0)
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void *)EVT_BACK_CLICKED);
    // (1, 0)
    btn = lv_btn_create(ctx->screen);
    lv_obj_add_style(btn, &lvs_invisible_button, 0);
    lv_obj_add_style(btn, &lvs_invisible_button, LV_STATE_PRESSED);
    lv_obj_set_pos(btn, 1, 0);
    lv_obj_set_size(btn, 1, 1);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(btn, button_clicked_handler, LV_EVENT_CLICKED, (void *)EVT_SETTING_CLICKED);
    // NW, SW used as LVGL button input device
    lvi_disable_button();
    lvi_pos_button(LVI_BUTTON_NW, 0, 0); // NW -> BACK (0, 0)
    lvi_pos_button(LVI_BUTTON_SW, 1, 0); // SW -> SETTINGS (1, 0)
    //
    // UI Elements
    //
    // Top label
    ctx->lbl_top = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_top, 200);
    lv_obj_set_style_text_align(ctx->lbl_top, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(ctx->lbl_top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_label_set_text(ctx->lbl_top, "");
    // Status label
    ctx->lbl_bottom = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_bottom, 240);
    lv_obj_align(ctx->lbl_bottom, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text(ctx->lbl_bottom, "");
    lv_label_set_long_mode(ctx->lbl_bottom, LV_LABEL_LONG_SCROLL_CIRCULAR);
    // File list
    ctx->lst_file_list = lv_list_create(ctx->screen);
    lv_obj_add_style(ctx->lst_file_list, &lvs_browser_file_list, 0);
    lv_obj_set_size(ctx->lst_file_list, 240, 192);
    lv_obj_set_pos(ctx->lst_file_list, 0, 24);
    // Other UI elements
    ctx->focused = 0;
    // Calculates all coordinates
    lv_obj_update_layout(ctx->screen);
    // Load screen
    lv_scr_load(ctx->screen);
    // arm update timer
    ctx->alarm_ui_update = tick_arm_timer_event(UI_UPDATE_INTERVAL_MS, true, EVT_BROWSER_UI_UPDATE, true);
}


static void browser_on_ui_update(browser_t *ctx)
{
    char buf[32];
    sprintf(buf, "C=%d B=%.1fv", ec_charge, ec_battery);
    lv_label_set_text(ctx->lbl_top, buf);
}


event_t const *browser_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Create screen, arm UI update timer
        EVT_EXIT:
            Disarm UI update timer
        EVT_START:
            Start browser_nodisk
        EVT_SETTING_CLICKED:
            Create settings state and transit to it
        EVT_SETTING_CLOSED:
            No action
        EVT_BROWSER_UI_UPDATE:
            Update top bar
    */
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser: entry\n");
        browser_on_entry(ctx);
        break;
    case EVT_EXIT:
        BR_LOGD("Browser: exit\n");
        tick_disarm_timer_event(ctx->alarm_ui_update);
        ctx->alarm_ui_update = 0;
        break;
    case EVT_START:
        STATE_START(me, &me->browser_nodisk); // default to nodisk state and wait card insertion
        break;
    case EVT_SETTING_CLICKED:
        settings_create(me);
        STATE_TRAN((hsm_t *)me, &me->settings);
        break;
    case EVT_SETTING_CLOSED:
        break;
    case EVT_BROWSER_UI_UPDATE:
        browser_on_ui_update(ctx);
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


//
// State Browser_Disk
//


// On-screen button callback for the buttons in file list
static void file_list_button_handler(lv_event_t *e)
{
    browser_t *ctx = (browser_t *)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
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
        BR_LOGD("Browser_Disk: PLAY button long pressed\n");
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


enum _file_list_entry_type
{
    FILE_LIST_ENTRY_TYPE_FILE = 0,
    FILE_LIST_ENTRY_TYPE_DIR,
    FILE_LIST_ENTRY_TYPE_PARENT,
    FILE_LIST_ENTRY_TYPE_PAGEUP,
    FILE_LIST_ENTRY_TYPE_PAGEDOWN
};


//  Populate file list from catalog object
//  mode = 0: Populate a newly opened catalog. Restore page and selection if available.
//  mode = 1: Populate an already opened directory. Go to the first entry of next page. (Page Down)
//  mode = 2: Populate an already opened directory. Go to the last entry of previous page. (Page Up)
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

    lv_obj_clean(ctx->lst_file_list);
    // If we are on the fist page of a sub directory, add ".."
    if ((0 == page) && (!path_is_root_dir(me->catalog->dir)))
    {
        btn = lv_list_add_btn_ex(ctx->lst_file_list, 0, "[..]", LV_LABEL_LONG_DOT);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PARENT);
        lv_obj_add_event_cb(btn, file_list_button_handler, LV_EVENT_ALL, (void *)ctx);
    }
    // 2nd page onwards, add "PgUp" button
    if (page > 0)
    {
        btn = lv_list_add_btn_ex(ctx->lst_file_list, 0, "[" LV_SYMBOL_UP "]", LV_LABEL_LONG_DOT);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEUP);
        lv_obj_add_event_cb(btn, file_list_button_handler, LV_EVENT_ALL, (void *)ctx);
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
            btn = lv_list_add_btn_ex(ctx->lst_file_list, &browser_icon_folder, name, LV_LABEL_LONG_DOT);
            lv_obj_set_user_data(btn, (void *)(uint32_t)((btn_index << 16) | FILE_LIST_ENTRY_TYPE_DIR));
        }
        else if (type == CAT_TYPE_FILE)
        {
            char ext[4];
            if (path_get_ext(name + 1, ext, 4))
            {
                if (0 == strcasecmp(ext, "nsf"))
                {
                    btn = lv_list_add_btn_ex(ctx->lst_file_list, &browser_icon_nsf, name + 1, LV_LABEL_LONG_DOT);    
                }
                else
                {
                    btn = lv_list_add_btn_ex(ctx->lst_file_list, &browser_icon_file, name + 1, LV_LABEL_LONG_DOT);
                }
            }
            else
            {
                btn = lv_list_add_btn_ex(ctx->lst_file_list, &browser_icon_file, name + 1, LV_LABEL_LONG_DOT);
            }
            lv_obj_set_user_data(btn, (void *)(uint32_t)((btn_index << 16) | FILE_LIST_ENTRY_TYPE_FILE));
        }
        if (btn_index == selection)
            focus = btn;
        ++btn_index;
        lv_obj_add_event_cb(btn, file_list_button_handler, LV_EVENT_ALL, (void *)ctx);
    }
    if (page < me->catalog->pages - 1)
    {
        btn = lv_list_add_btn_ex(ctx->lst_file_list, 0, "[" LV_SYMBOL_DOWN "]", LV_LABEL_LONG_DOT);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEDOWN);
        lv_obj_add_event_cb(btn, file_list_button_handler, LV_EVENT_ALL, (void *)ctx);
    }
    if (-1 == selection) // select last
    {
        focus = btn; // btn points to the last button added
    }
    // Add all buttons to input group
    lv_group_remove_all_objs(lvi_keypad_group);
    uint32_t cnt = lv_obj_get_child_cnt(ctx->lst_file_list);
    for (uint32_t i = 0; i < cnt; ++i)
    {
        lv_obj_t *obj = lv_obj_get_child(ctx->lst_file_list, i);
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


static void browser_disk_map_keypad()
{
    // PLAY, NE, SE used as keypad input device
    lvi_disable_keypad();
    lvi_map_keypad(LVI_BUTTON_PLAY, LV_KEY_ENTER); // PLAY -> Enter, triggers list button's callback action
    lvi_map_keypad(LVI_BUTTON_NE, LV_KEY_PREV);    // NE -> Prev, used in file list navigation
    lvi_map_keypad(LVI_BUTTON_SE, LV_KEY_NEXT);    // SE -> Next, used in file list navigation
}


static void browser_disk_cleanup(app_t* me, browser_t *ctx)
{
    // unmap keypad and input group
    lv_group_remove_all_objs(lvi_keypad_group);
    lv_obj_clean(ctx->lst_file_list);
    lvi_disable_keypad();
    // clear catalog
    catalog_close(me->catalog);
    me->catalog = 0;
    me->catalog_history_page[0] = 0;
    me->catalog_history_selection[0] = 0;
    me->catalog_history_index = 0;
}


static void browser_disk_on_entry(app_t *me, browser_t *ctx)
{
    // Setup keypad
    browser_disk_map_keypad();
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
        else // disk_check_dir error
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
        t = disk_check_dir(me->catalog->dir);
        ec_resume_watchdog();
        switch (t)
        {
        case 1: // current dir is valid and accessible
            populate_file_list(me, 0);
            break;
        case 2: // current dir is not accessible but disk is ok (TODO: Possible?)
            BR_LOGW("Browser_Disk: current dir not accessible, go to root\n");
            catalog_close(me->catalog);
            me->catalog = 0;
            me->catalog_history_page[0] = 0;
            me->catalog_history_selection[0] = 0;
            me->catalog_history_index = 0;
            ec_pause_watchdog();
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
        default: // disk not accessible
            BR_LOGE("Browser_Disk: disk error\n");
            EQ_QUICK_PUSH(EVT_DISK_ERROR);
            break;
        }
    }
}


static void browser_disk_on_play(app_t *me, browser_t *ctx, event_t const *evt)
{
    // EVT_BROWSER_PLAY_CLICKED is sent by file list button callback in file_list_button_handler()
    // The firing button object is stored in event parameter
    lv_obj_t *btn = (lv_obj_t *)(evt->param);   
    const char *btn_text = lv_list_get_btn_text(ctx->lst_file_list, btn);   // name of the file
    int t = (int)lv_obj_get_user_data(btn);   
    int index = (int)((uint32_t)t >> 16);     // index of this file in catalog
    int type = (int)((uint32_t)t & 0xffff);   // type of this file (file or directory or special button (PageUp/PageDown/Parent))
    char path[FF_LFN_BUF + 1];
    switch (type)
    {
    case FILE_LIST_ENTRY_TYPE_FILE: // PLAY pressed on a file
        // Save browsing history
        me->catalog_history_page[me->catalog_history_index] = me->catalog->cur_page;
        me->catalog_history_selection[me->catalog_history_index] = index;
        // Move catalog cursor to the current selection and hand over to player
        BR_LOGD("Browser_Disk: play entry %d in page %d\n", index, me->catalog->cur_page);
        catalog_move_cursor(me->catalog, me->catalog->cur_page, index);
        STATE_TRAN((hsm_t *)me, &me->player);
        break;
    case FILE_LIST_ENTRY_TYPE_DIR:  // PLAY pressed on a directory
        if (path_concatenate(me->catalog->dir, btn_text, path, FF_LFN_BUF + 1, true))
        {
            // Save browsing history
            me->catalog_history_page[me->catalog_history_index] = me->catalog->cur_page;
            me->catalog_history_selection[me->catalog_history_index] = index;
            // Create new catalog object
            int save_page = me->catalog->cur_page;
            catalog_close(me->catalog);
            me->catalog = 0;
            lv_label_set_text(ctx->lbl_bottom, "Loading...");
            lv_refr_now(NULL);
            ec_pause_watchdog();
            t = catalog_open_dir(path, 0, CATALOG_PAGER_SIZE, true, &(me->catalog));
            ec_resume_watchdog();
            if (CAT_OK == t)
            {
                ++(me->catalog_history_index);
                // Default select first page and first entry in the new directory
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
        // else cannot enter this directory
        break;
    case FILE_LIST_ENTRY_TYPE_PARENT:
        // Same as back button
        EQ_QUICK_PUSH(EVT_BACK_CLICKED);
        break;
    case FILE_LIST_ENTRY_TYPE_PAGEUP:
        // populate previous page
        populate_file_list(me, 2);
        break;
    case FILE_LIST_ENTRY_TYPE_PAGEDOWN:
        // populate next page
        populate_file_list(me, 1);
        break;
    }
}


static void browser_disk_on_back(app_t *me, browser_t *ctx)
{
    // Go to parent directory
    char path[FF_LFN_BUF + 1];
    if (path_get_parent(me->catalog->dir, path, FF_LFN_BUF + 1))
    {
        catalog_close(me->catalog);
        me->catalog = 0;
        int t;
        lv_label_set_text(ctx->lbl_bottom, "Loading...");
        lv_refr_now(NULL);
        ec_pause_watchdog();
        t = catalog_open_dir(path, 0, CATALOG_PAGER_SIZE, true, &(me->catalog));
        ec_resume_watchdog();
        if (CAT_OK == t)
        {
            // Discard current browsing state
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
    // else no parent directory, stay where we are
}


static void browser_disk_on_setting(browser_t *ctx)
{
    // Disable keypads. They will be remapped in settings
    lvi_disable_keypad();
    // Save current focused file
    ctx->focused = 0;
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(ctx->lst_file_list); ++i)
    {
        lv_obj_t *obj = lv_obj_get_child(ctx->lst_file_list, i);
        if (lv_obj_has_state(obj, LV_STATE_FOCUSED))
        {
            ctx->focused = obj;
            break;
        }
    }
    // keypads nolonger controls file list
    lv_group_remove_all_objs(lvi_keypad_group);
}


static void browser_disk_on_setting_closed(browser_t *ctx)
{
    // restore keypad mapping
    browser_disk_map_keypad();
    // attach input group to file list buttons
    lv_group_remove_all_objs(lvi_keypad_group);
    for (uint32_t i = 0; i < lv_obj_get_child_cnt(ctx->lst_file_list); ++i)
    {
        lv_obj_t *obj = lv_obj_get_child(ctx->lst_file_list, i);
        lv_group_add_obj(lvi_keypad_group, obj);
    }
    // Set focus if necessary
    if (ctx->focused != 0)
    {
        lv_group_focus_obj(ctx->focused);
    }
}


event_t const *browser_disk_handler(app_t *me, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Set keypad map
            Create catalog from root directory or restore catalog history
            Populate file list
        EVT_EXIT:
            Do nothing. Cleanup is done on EVT_DISK_ERROR or EVT_DISK_EJECTED
        EVT_BROWSER_PLAY_CLICKED:
            If clicked on a directory, push history and navigate to that directory
            If clicked on file, transit to player state
            If clilck on [..], enqueue EVT_BROWSER_CLICKED event
            If click on Page Up/Down, navigate page
        EVT_BACK_CLICKED:
            Pop history and navigate to parent directory
        EVT_SETTING_CLICKED:
            Clean keypad map
            Save current focused file
            Create settings state and transit to it
        EVT_SETTING_CLOSED:
            Restore keypad map
            Bind input group to file list
            Restore file list focused item
        EVT_DISK_ERROR:
            Cleanup
            Transit to browser_baddisk state
        EVT_DISK_EJECTED:
            Cleaup
            Transit to browser_error state
    */
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser_Disk: entry\n");
        browser_disk_on_entry(me, ctx);
        break;
    case EVT_EXIT:
        BR_LOGD("Browser_Disk: exit\n");
        break;
    case EVT_BROWSER_PLAY_CLICKED:
        browser_disk_on_play(me, ctx, evt);
        break;
    case EVT_BACK_CLICKED:
        browser_disk_on_back(me, ctx);
        break;
    case EVT_SETTING_CLICKED:
        browser_disk_on_setting(ctx);
        r = evt; // super state browser will process EVT_SETTING_CLICKED further
        break;
    case EVT_SETTING_CLOSED:
        browser_disk_on_setting_closed(ctx);
        r = evt; // super state browser will process EVT_SETTING_CLICKED further
        break;
    case EVT_DISK_ERROR:
        browser_disk_cleanup(me, ctx);
        STATE_TRAN((hsm_t *)me, &me->browser_baddisk);
        break;
    case EVT_DISK_EJECTED:
        browser_disk_cleanup(me, ctx);
        STATE_TRAN((hsm_t *)me, &me->browser_nodisk);
        break;
    default:
        r = evt;
        break;
    }
    return r;
}


//
// State Browser_Nodisk
//


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
    */
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser_Nodisk: entry\n");
        lv_label_set_text(ctx->lbl_bottom, "No card");
        break;
    case EVT_EXIT:
        BR_LOGD("Browser_Nodisk: exit\n");
        break;
    case EVT_DISK_INSERTED:
        STATE_TRAN((hsm_t *)me, &me->browser_disk);
        break;
    default:
        r = evt;
        break;
    }
    return r;
}



//
// State Browser_Baddsk
//


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
    */
    event_t const *r = 0;
    browser_t *ctx = &(me->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser_Baddisk: entry\n");
        lv_label_set_text(ctx->lbl_bottom, "Card error");
        break;
    case EVT_EXIT:
        BR_LOGD("Browser_Baddisk: exit\n");
        break;
    case EVT_DISK_EJECTED:
        STATE_TRAN((hsm_t *)me, &me->browser_nodisk);
        break;
    default:
        r = evt;
        break;
    }
    return r;
}
