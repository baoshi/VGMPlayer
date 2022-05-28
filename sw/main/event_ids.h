#pragma once


enum
{
    EVT_APP_TICK = 1,
    EVT_EC_FAILED,
    EVT_DISK_INSERTED,
    EVT_DISK_EJECTED,
    EVT_DISK_ERROR,
    EVT_BROWSER_UI_UPDATE,
    EVT_BROWSER_PLAY_CLICKED,   // used in browser_disk state, param is the lv_list button pointer
    EVT_BROWSER_BACK_CLICKED,   // used in browser_disk state, to navigate back
    EVT_BROWSER_PLAY_FILE,
    EVT_EARPIECE_PLUGGED,
    EVT_EARPIECE_UNPLUGGED,
    EVT_PLAYER_UI_UPDATE,
    EVT_PLAYER_PLAY_CLICKED,
    EVT_PLAYER_MODE_CLICKED,
    EVT_PLAYER_UP_CLICKED,
    EVT_PLAYER_DOWN_CLICKED,
    EVT_PLAYER_SONG_ENDED
};
