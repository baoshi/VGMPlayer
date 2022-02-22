#pragma once

#include <lvgl.h>
#include <ff.h>
#include "hsm.h"


typedef struct app_s app_t;

// Centralized declaration of all state machine related structure and functions

//
// Browser
//
typedef struct browser_s
{
    lv_obj_t* screen;
    lv_obj_t* lbl_top;    // Top indicators
    lv_obj_t* lbl_bottom; // Bottom label
    lv_obj_t* lst_files;  // File List
    int alarm_ui_update;
    char cur_dir[FF_LFN_BUF + 1] ;      // current selected directory
    char cur_selection[FF_LFN_BUF + 1]; // current selected file
} browser_t;

event_t const *browser_handler(app_t *me, event_t const *evt);
event_t const *browser_disk_handler(app_t *me, event_t const *evt);
event_t const *browser_nodisk_handler(app_t *me, event_t const *evt);
event_t const *browser_baddisk_handler(app_t *me, event_t const *evt);



// Application level state machione
struct app_s
{
    hsm_t super;
    state_t browser, browser_disk, browser_nodisk, browser_baddisk;
    browser_t browser_ctx;
};

