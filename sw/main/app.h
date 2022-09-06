#pragma once

#include <lvgl.h>
#include <ff.h>
#include "sw_conf.h"
#include "decoder.h"
#include "hsm.h"
#include "catalog.h"
#include "lvsupp.h"


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
    lv_obj_t *img_top;          // Top image
    lv_obj_t *lbl_top;          // Top indicators
    lv_obj_t *lst_file_list;    // List of files
    lv_group_t *keypad_group;   // keypad input group
} browser_t;

event_t const *browser_handler(app_t *app, event_t const *evt);
event_t const *browser_disk_handler(app_t *app, event_t const *evt);
event_t const *browser_nodisk_handler(app_t *app, event_t const *evt);
event_t const *browser_baddisk_handler(app_t *app, event_t const *evt);


//
// Player
//
#define PLAYER_SPECTRUM_MAX_BINS    64
typedef struct player_s
{
    lv_obj_t *screen;
    lv_obj_t *img_top;              // Top image
    lv_obj_t *lbl_top;              // Top indicators
    lv_obj_t *spectrum;             // Spectrum
    lv_obj_t *bar_progress;         // Progress bar
    lv_obj_t *lbl_play_time;        // Play time label
    lv_obj_t *lbl_remaining_time;   // Remaining time label
    char file[FF_LFN_BUF + 1];      // Complete path of file to play
    char name[FF_LFN_BUF + 1];      // Bare name of the file
    int played;                     // Number of songs played
    bool playing;
    int nav_dir;                // file navigation direction. 1 = next, -1 = previous, 0 = go back to browser_disk
    decoder_t* decoder;
    uint8_t spectrum_bin[PLAYER_SPECTRUM_MAX_BINS];
} player_t;

event_t const *player_handler(app_t *app, event_t const *evt);
event_t const *player_s16_handler(app_t *app, event_t const *evt);
event_t const *player_vgm_handler(app_t *app, event_t const *evt);


//
// Brightness Popup
//
typedef struct brightness_s
{
    lv_obj_t *popup;
    lv_group_t *keypad_group;   // keypad input group
    void *prev_input;           // to save/restore input config 
    int timer_auto_close;
} brightness_t;


//
// Volume Popup
//
typedef struct volume_s
{
    lv_obj_t *popup;
    lv_group_t *keypad_group;   // keypad input group
    void *prev_input;           // to save/restore input config
    int timer_auto_close;
} volume_t;


//
// Alert popup
//
typedef struct alert_s
{
    lv_obj_t *popup;
    lv_group_t *keypad_group;   // keypad input group
    void *prev_input;           // to save/restore input config 
    int timer_auto_close;
    int exit_event;
} alert_t;


// Application state machine
struct app_s
{
    hsm_t super;

    state_t browser, browser_disk, browser_nodisk, browser_baddisk;
    browser_t browser_ctx;

    state_t player, player_s16, player_vgm;
    player_t player_ctx;

    // popups
    brightness_t brightness_ctx;
    volume_t volume_ctx;
    alert_t alert_ctx;

    // on-screen objects
    lv_obj_t *img_battery;  // Battery indicator
    
    int timer_ui_update;    // UI update time

    // data shared by all states
    catalog_t *catalog;
    int catalog_history_page[CATALOG_HISTORY_DEPTH];
    int catalog_history_selection[CATALOG_HISTORY_DEPTH];
    int catalog_history_index;
    bool busy;
};

