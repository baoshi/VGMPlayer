#pragma once

#include <lvgl.h>
#include <ff.h>
#include "sw_conf.h"
#include "decoder.h"
#include "hsm.h"
#include "catalog.h"


typedef struct app_s app_t;

// Centralized declaration of all state machine related structure and functions

//
// Top
//
event_t const *top_handler(app_t *app, event_t const *evt);


//
// Browser
//
typedef struct browser_s
{
    lv_obj_t *screen;
    lv_obj_t *lbl_top;          // Top indicators
    lv_obj_t *lbl_bottom;       // Bottom label
    lv_obj_t *lst_file_list;    // List of files
    lv_group_t *keypad_group;   // keypad input group
    int timer_ui_update;
} browser_t;

event_t const *browser_handler(app_t *app, event_t const *evt);
event_t const *browser_disk_handler(app_t *app, event_t const *evt);
event_t const *browser_nodisk_handler(app_t *app, event_t const *evt);
event_t const *browser_baddisk_handler(app_t *app, event_t const *evt);

//
// Player
//
typedef struct player_s
{
    lv_obj_t* screen;
    lv_obj_t* lbl_top;      // Top indicators
    lv_obj_t* lbl_bottom;   // Bottom indicators
    int timer_ui_update;    // UI update timer
    char file[FF_LFN_BUF + 1];
    int nav_dir;            // file navigation direction. 1 = next, -1 = previous, 0 = go back to browser_disk
    bool playing;
    decoder_t* decoder;
} player_t;

event_t const *player_handler(app_t *app, event_t const *evt);
event_t const *player_s16_handler(app_t *app, event_t const *evt);


//
// Brightness Popup
//
typedef struct brightness_s
{
    lv_obj_t *popup;
    lv_group_t *keypad_group;   // keypad input group
    void *prev_input;           // to save/restore input config 
} brightness_t;

void brightness_popup(brightness_t *ctx);
void brightness_close(brightness_t *ctx);


//
// Volume Popup
//
typedef struct volume_s
{
    lv_obj_t *popup;
    lv_group_t *keypad_group;   // keypad input group
    void *prev_input;           // to save/restore input config 
} volume_t;

void volume_popup(volume_t *ctx);
void volume_close(volume_t *ctx);


//
// Alert popup
//
typedef struct alert_s
{
    state_t *creator;
    lv_obj_t *popup;
    lv_group_t *keypad_group;   // keypad input group
    int auto_close_ms;
    int timer_auto_close;
    int exit_event;
    void *prev_input;         // to save/restore input config
} alert_t;

event_t const *alert_handler(app_t *app, event_t const *evt);

void alert_create(app_t *app, const void *icon, const char *text, int auto_close, int exit_event);


// Application level state machine
struct app_s
{
    hsm_t super;

    state_t browser, browser_disk, browser_nodisk, browser_baddisk;
    browser_t browser_ctx;

    state_t player, player_s16, player_s16_playing, player_s16_paused; 
    player_t player_ctx;

    brightness_t brightness_ctx;
    volume_t volume_ctx;

    state_t alert;
    alert_t alert_ctx;
    
    // data shared by all states
    catalog_t *catalog;
    int catalog_history_page[CATALOG_HISTORY_DEPTH];
    int catalog_history_selection[CATALOG_HISTORY_DEPTH];
    int catalog_history_index;
};

