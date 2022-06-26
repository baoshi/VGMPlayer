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
// Browser
//
typedef struct browser_s
{
    lv_obj_t *screen;
    lv_obj_t *lbl_top;      // Top indicators
    lv_obj_t *lbl_bottom;   // Bottom label
    lv_obj_t *lst_files;    // List of files
    lv_obj_t *focused;      // focused file in the list
    lv_obj_t *bar_brignthess;
    int alarm_ui_update;
} browser_t;

event_t const *browser_handler(app_t *me, event_t const *evt);
event_t const *browser_disk_handler(app_t *me, event_t const *evt);
event_t const *browser_disk_brightness_handler(app_t *me, event_t const *evt);
event_t const *browser_nodisk_handler(app_t *me, event_t const *evt);
event_t const *browser_nodisk_brightness_handler(app_t *me, event_t const *evt);
event_t const *browser_baddisk_handler(app_t *me, event_t const *evt);
event_t const *browser_baddisk_brightness_handler(app_t *me, event_t const *evt);


//
// Player
//
typedef struct player_s
{
    lv_obj_t* screen;
    lv_obj_t* lbl_top;      // Top indicators
    lv_obj_t* lbl_bottom;   // Bottom indicators
    lv_obj_t* mbx_alert;    // Alert message
    int exception;
    int timer_ui_update;    // UI update timer
    char file[FF_LFN_BUF + 1];
    bool first_song;        // Playing first song?
    int next_dir;           // direction when search next song, 0 = next, 1 = previous
    int timer_general;      // General purpose timer
    decoder_t* decoder;
} player_t;

event_t const *player_handler(app_t *me, event_t const *evt);
event_t const *player_exp_handler(app_t *me, event_t const *evt);
event_t const *player_volume_handler(app_t *me, event_t const *evt);
event_t const *player_visual_handler(app_t *me, event_t const *evt);


//
// Settings
//
typedef struct settings_s
{
    state_t *super;
    lv_obj_t* popup;
} settings_t;

event_t const *settings_handler(app_t *me, event_t const *evt);
event_t const *settings_brightness_handler(app_t *me, event_t const *evt);
event_t const *settings_earpiece_handler(app_t *me, event_t const *evt);
event_t const *settings_speaker_handler(app_t *me, event_t const *evt);


// Application level state machine
struct app_s
{
    hsm_t super;
    state_t browser, browser_disk, browser_nodisk, browser_baddisk;
    browser_t browser_ctx;
    state_t player, player_exp, player_volume, player_visual;
    player_t player_ctx;
    state_t settings, settings_brightness, settings_earpiece, settings_speaker;
    settings_t settings_ctx;
    // data shared by all states
    catalog_t *catalog;
    int catalog_history_page[CATALOG_HISTORY_DEPTH];
    int catalog_history_selection[CATALOG_HISTORY_DEPTH];
    int catalog_history_index;
};

