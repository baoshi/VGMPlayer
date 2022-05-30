#pragma once

#include <lvgl.h>
#include <ff.h>
#include "sw_conf.h"
#include "decoder.h"
#include "hsm.h"
#include "lister.h"


typedef struct app_s app_t;

// Centralized declaration of all state machine related structure and functions

//
// Browser
//
typedef struct browser_s
{
    lv_obj_t *screen;
    lv_obj_t *lbl_top;    // Top indicators
    lv_obj_t *lbl_bottom; // Bottom label
    lv_obj_t *lst_files;  // File List
    int alarm_ui_update;
    char cur_dir[FF_LFN_BUF + 1] ;      // current browsing directory
    bool skip_first_click;
    lister_t *lister;
    int lister_cur_page;
    int lister_history_page[BROWSER_LISTER_HISTORY_DEPTH];
    int lister_history_selection[BROWSER_LISTER_HISTORY_DEPTH];
    int lister_history_index;
} browser_t;

event_t const *browser_handler(app_t *me, event_t const *evt);
event_t const *browser_disk_handler(app_t *me, event_t const *evt);
event_t const *browser_nodisk_handler(app_t *me, event_t const *evt);
event_t const *browser_baddisk_handler(app_t *me, event_t const *evt);


//
// Player
//
typedef struct player_s
{
    lv_obj_t* screen;
    lv_obj_t* lbl_top;    // Top indicators
    lv_obj_t* lbl_bottom; // Bottom indicators
    int alarm_ui_update;
    char file[FF_LFN_BUF + 1]; // file to play
    decoder_t* decoder;
} player_t;

event_t const *player_handler(app_t *me, event_t const *evt);
event_t const *player_s16_handler(app_t *me, event_t const *evt);
event_t const *player_s16_playing_handler(app_t *me, event_t const *evt);
event_t const *player_s16_paused_handler(app_t *me, event_t const *evt);


// Application level state machine
struct app_s
{
    hsm_t super;
    state_t browser, browser_disk, browser_nodisk, browser_baddisk;
    browser_t browser_ctx;
    state_t player, player_s16, player_s16_playing, player_s16_paused;
    player_t player_ctx;
};

