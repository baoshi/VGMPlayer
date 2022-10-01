#include <stdio.h>
#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "lvstyle.h"
#include "lvsupp.h"
#include "lvassets.h"
#include "tick.h"
#include "event_ids.h"
#include "event_queue.h"
#include "disk.h"
#include "ec.h"
#include "input.h"
#include "catalog.h"
#include "path_utils.h"
#include "popup.h"
#include "app.h"


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


static void browser_setup_input()
{
    // Button
    input_disable_button_dev();
    // Keypad
    input_disable_keypad_dev();
}


static void browser_on_entry(browser_t *ctx)
{
    // Create screen
    ctx->screen = lv_obj_create(NULL);
    // Self-destruction callback
    lv_obj_add_event_cb(ctx->screen, screen_event_handler, LV_EVENT_ALL, (void *)ctx);
    //
    // UI Elements
    //
    // Top bar: 26px high, right side has battery icon (26 pixels wide, leave 30 pixels at least)
    // Font line height is 21, pad 2 pixel top down
    // Icon is 20x17
    ctx->img_top = lv_img_create(ctx->screen);
    lv_obj_set_pos(ctx->img_top, 2, 5);
    ctx->lbl_top = lv_label_create(ctx->screen);
    lv_obj_set_width(ctx->lbl_top, 180);    // folder image: w20, battery image: w26
    lv_obj_set_pos(ctx->lbl_top, 28, 3);
    lv_obj_set_style_text_align(ctx->lbl_top, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_long_mode(ctx->lbl_top, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(ctx->lbl_top, "");
    // File list
    ctx->lst_file_list = lv_list_create(ctx->screen);
    lv_obj_add_style(ctx->lst_file_list, &lvs_browser_file_list, 0);
    lv_obj_set_size(ctx->lst_file_list, 240, 214);
    lv_obj_set_pos(ctx->lst_file_list, 0, 26);
    // Other UI elements
    ctx->keypad_group = NULL;
    // Calculates all coordinates
    lv_obj_update_layout(ctx->screen);
    // Create virtual buttons
    input_create_buttons(ctx->screen);
    // Setup input
    browser_setup_input();
    // Load screen
    lv_scr_load(ctx->screen);
}


static void browser_on_exit(browser_t *ctx)
{
    input_delete_buttons();
    event_queue_clear();
}


event_t const *browser_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Create screen, buttons
        EVT_EXIT:
            Delete buttons
        EVT_START:
            Start browser_nodisk
        EVT_APP_AUTO_OFF:
            Transit to poweroff state
    */
    event_t const *r = 0;
    browser_t *ctx = &(app->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser: entry\n");
        browser_on_entry(ctx);
        break;
    case EVT_EXIT:
        BR_LOGD("Browser: exit\n");
        browser_on_exit(ctx);
        break;
    case EVT_START:
        // default to nodisk state and wait card insertion
        STATE_START(app, &app->browser_nodisk);
        break;
    case EVT_APP_AUTO_OFF:
        STATE_TRAN((hsm_t *)app, &app->poweroff);
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
        btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_folder, "[ .. ]", LV_LABEL_LONG_DOT);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PARENT);
        lv_obj_add_event_cb(btn, file_list_button_handler, LV_EVENT_ALL, (void *)ctx);
    }
    // 2nd page onwards, add "PgUp" button
    if (page > 0)
    {
        btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_pageup, NULL, LV_LABEL_LONG_CLIP);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEUP);
        lv_obj_add_event_cb(btn, file_list_button_handler, LV_EVENT_ALL, (void *)ctx);
    }
    catalog_move_cursor(me->catalog, page, 0);
    uint8_t type;
    char name[FF_LFN_BUF + 3];
    while (CAT_OK == catalog_get_next_entry(me->catalog, true, false, name + 1, FF_LFN_BUF + 1, false, &type))
    {
        if (type == CAT_TYPE_DIRECTORY)
        {
            name[0] = '[';
            strcat(name, "]");
            btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_folder, name, LV_LABEL_LONG_DOT);
            lv_obj_set_user_data(btn, (void *)(uint32_t)((btn_index << 16) | FILE_LIST_ENTRY_TYPE_DIR));
        }
        else if (type == CAT_TYPE_FILE)
        {
            char ext[4];
            if (path_get_ext(name + 1, ext, 4))
            {
                if (0 == strcasecmp(ext, "vgm"))
                {
                    btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_file_vgm, name + 1, LV_LABEL_LONG_DOT);    
                }
                else if (0 == strcasecmp(ext, "s16"))
                {
                    btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_file_raw, name + 1, LV_LABEL_LONG_DOT);    
                }
                else
                {
                    btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_file, name + 1, LV_LABEL_LONG_DOT);
                }
            }
            else
            {
                btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_file, name + 1, LV_LABEL_LONG_DOT);
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
        btn = lv_list_add_btn_ex(ctx->lst_file_list, &img_browser_pagedown,  NULL, LV_LABEL_LONG_CLIP);
        lv_obj_set_user_data(btn, (void *)FILE_LIST_ENTRY_TYPE_PAGEDOWN);
        lv_obj_add_event_cb(btn, file_list_button_handler, LV_EVENT_ALL, (void *)ctx);
    }
    if (-1 == selection) // select last
    {
        focus = btn; // btn points to the last button added
    }
    // Add all file/directory buttons to input group
    lv_group_remove_all_objs(ctx->keypad_group);
    uint32_t cnt = lv_obj_get_child_cnt(ctx->lst_file_list);
    for (uint32_t i = 0; i < cnt; ++i)
    {
        lv_obj_t *obj = lv_obj_get_child(ctx->lst_file_list, i);
        lv_group_add_obj(ctx->keypad_group, obj);
    }
    // Set focus if necessary
    if (focus != NULL)
    {
        lv_group_focus_obj(focus);
    }
    // Update top bar
    lv_label_set_text(ctx->lbl_top, me->catalog->dir);
}


static void clean_file_list(app_t* me, browser_t *ctx)
{
    // Clean file list
    lv_obj_clean(ctx->lst_file_list);
    // Clear catalog
    catalog_close(me->catalog);
    me->catalog = 0;
    me->catalog_history_page[0] = 0;
    me->catalog_history_selection[0] = 0;
    me->catalog_history_index = 0;
}


static void browser_disk_setup_input(browser_t *ctx)
{
    // Button
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_OPEN_BRIGHTNESS_POPUP);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_BACK_CLICKED);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_LONG_PRESSED, EVT_BUTTON_BACK_LONG_PRESSED);
    input_enable_button_dev();
    // Keypad
    input_disable_keypad_dev();
    input_map_keypad(-1, 0);
    input_map_keypad(INPUT_KEY_PLAY, LV_KEY_ENTER);
    input_map_keypad(INPUT_KEY_UP, LV_KEY_PREV);
    input_map_keypad(INPUT_KEY_DOWN, LV_KEY_NEXT);
    ctx->keypad_group = lv_group_create();
    lv_group_set_wrap(ctx->keypad_group, false);
    lv_indev_set_group(indev_keypad, ctx->keypad_group);
    input_enable_keypad_dev();
}


static void browser_disk_on_entry(app_t *me, browser_t *ctx)
{
    // Setup keypad
    browser_disk_setup_input(ctx);
    // Show icon
    lv_img_set_src(ctx->img_top, &img_microsd);
    // build up file list box
    int t;
    if (0 == me->catalog)
    {
        // no directory catalog available, we must be entering from nodisk state
        ec_pause_watchdog();
        lv_label_set_text(ctx->lbl_top, "Loading...");
        lv_refr_now(NULL);
        t = disk_check_dir("/");
        ec_resume_watchdog();
        if ((1 == t) || (2 == t)) // root dir is accessible
        {
            me->catalog_history_page[0] = 0;
            me->catalog_history_selection[0] = 0;
            me->catalog_history_index = 0;
            ec_pause_watchdog();
            t = catalog_open_dir("/", config_supported_file_exts, CATALOG_PAGER_SIZE, true, &(me->catalog));
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
            t = catalog_open_dir("/", config_supported_file_exts, CATALOG_PAGER_SIZE, true, &(me->catalog));
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


static void browser_disk_on_exit(browser_t *ctx)
{
    lv_indev_set_group(indev_keypad, NULL);
    lv_group_remove_all_objs(ctx->keypad_group);
    lv_group_del(ctx->keypad_group);
    ctx->keypad_group = NULL;
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
            lv_label_set_text(ctx->lbl_top, "Loading...");
            lv_refr_now(NULL);
            ec_pause_watchdog();
            t = catalog_open_dir(path, config_supported_file_exts, CATALOG_PAGER_SIZE, true, &(me->catalog));
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
        EQ_QUICK_PUSH(EVT_BUTTON_BACK_CLICKED);
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
        lv_label_set_text(ctx->lbl_top, "Loading...");
        lv_refr_now(NULL);
        ec_pause_watchdog();
        t = catalog_open_dir(path, config_supported_file_exts, CATALOG_PAGER_SIZE, true, &(me->catalog));
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


event_t const *browser_disk_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Setup inputs
            Create catalog from root directory or restore catalog history
            Populate file list
        EVT_EXIT:
            Delete input
        EVT_BROWSER_PLAY_CLICKED:
            If clicked on a directory, push history and navigate to that directory
            If clicked on file, transit to player state
            If clilck on [..], enqueue EVT_BROWSER_CLICKED event
            If click on Page Up/Down, navigate page
        EVT_BUTTON_BACK_CLICKED:
            Pop history and navigate to parent directory
        EVT_DISK_ERROR:
            Cleanup
            Transit to browser_baddisk state
        EVT_DISK_EJECTED:
            Cleaup
            Transit to browser_error state
        EVT_BUTTON_BACK_LONG_PRESSED:
            If we are at root dir then transit to off state
    */
    event_t const *r = 0;
    browser_t *ctx = &(app->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser_Disk: entry\n");
        browser_disk_on_entry(app, ctx);
        break;
    case EVT_EXIT:
        browser_disk_on_exit(ctx);
        BR_LOGD("Browser_Disk: exit\n");
        break;
    case EVT_BROWSER_PLAY_CLICKED:
        browser_disk_on_play(app, ctx, evt);
        break;
    case EVT_BUTTON_BACK_CLICKED:
        browser_disk_on_back(app, ctx);
        break;
    case EVT_DISK_ERROR:
        STATE_TRAN((hsm_t *)app, &app->browser_baddisk);
        break;
    case EVT_DISK_EJECTED:
        STATE_TRAN((hsm_t *)app, &app->browser_nodisk);
        break;
    case EVT_BUTTON_BACK_LONG_PRESSED:
        if (app->catalog && path_is_root_dir(app->catalog->dir))
        {
            STATE_TRAN((hsm_t *)app, &app->poweroff);
        }
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


static void browser_nodisk_setup_input()
{
    // Button
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_OPEN_BRIGHTNESS_POPUP);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_BACK_CLICKED);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_LONG_PRESSED, EVT_BUTTON_BACK_LONG_PRESSED);
    input_enable_button_dev();
    // Keypad
    input_disable_keypad_dev();
    input_map_keypad(-1, 0);
}


event_t const *browser_nodisk_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Clear file list
            Disable keypad
        EVT_EXIT:
            No action
        EVT_DISK_INSERTED:
            Transit to browser_disk state
        EVT_BUTTON_BACK_LONG_PRESSED:
            Transit to off state
    */
    event_t const *r = 0;
    browser_t *ctx = &(app->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser_Nodisk: entry\n");
        browser_nodisk_setup_input();
        lv_img_set_src(ctx->img_top, &img_microsd_empty);
        lv_label_set_text(ctx->lbl_top, "No card");
        clean_file_list(app, ctx);
        break;
    case EVT_EXIT:
        BR_LOGD("Browser_Nodisk: exit\n");
        break;
    case EVT_DISK_INSERTED:
        STATE_TRAN((hsm_t *)app, &app->browser_disk);
        break;
    case EVT_BUTTON_BACK_LONG_PRESSED:
        STATE_TRAN((hsm_t *)app, &app->poweroff);
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

static void browser_baddisk_setup_input()
{
    // Button
    input_disable_button_dev();
    input_map_button(-1, 0, 0);
    input_map_button(INPUT_KEY_SETTING, LV_EVENT_SHORT_CLICKED, EVT_OPEN_BRIGHTNESS_POPUP);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_SHORT_CLICKED, EVT_BUTTON_BACK_CLICKED);
    input_map_button(INPUT_KEY_BACK, LV_EVENT_LONG_PRESSED, EVT_BUTTON_BACK_LONG_PRESSED);
    input_enable_button_dev();
    // Keypad
    input_disable_keypad_dev();
    input_map_keypad(-1, 0);
}


event_t const *browser_baddisk_handler(app_t *app, event_t const *evt)
{
    /* Events
        EVT_ENTRY:
            Clear file list
            Disable keypad
        EVT_EXIT:
            No action
        EVT_DISK_EJECTED:
            Transit to browser_nodisk state
        EVT_BUTTON_BACK_LONG_PRESSED:
            Transit to off state
    */
    event_t const *r = 0;
    browser_t *ctx = &(app->browser_ctx);
    switch (evt->code)
    {
    case EVT_ENTRY:
        BR_LOGD("Browser_Baddisk: entry\n");
        browser_baddisk_setup_input();
        lv_img_set_src(ctx->img_top, &img_microsd_bad);
        lv_label_set_text(ctx->lbl_top, "Card error");
        clean_file_list(app, ctx);
        break;
    case EVT_EXIT:
        BR_LOGD("Browser_Baddisk: exit\n");
        break;
    case EVT_DISK_EJECTED:
        STATE_TRAN((hsm_t *)app, &app->browser_nodisk);
        break;
    case EVT_BUTTON_BACK_LONG_PRESSED:
        STATE_TRAN((hsm_t *)app, &app->poweroff);
        break;
    default:
        r = evt;
        break;
    }
    return r;
}
