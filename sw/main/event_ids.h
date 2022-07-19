#pragma once


enum
{
    EVT_APP_TICK = 1,
    EVT_EC_FAILED,
    EVT_DISK_INSERTED,
    EVT_DISK_EJECTED,
    EVT_DISK_ERROR,

    EVT_SETTING_CLICKED,    // Setting button event
    EVT_BACK_CLICKED,       // Back button event

    EVT_SETTING_CLOSED,     // Setting popup closed

    EVT_ALERT_MANUAL_CLOSE, // Alert popup manual close
    EVT_ALERT_AUTO_CLOSE,   // Alert popup autoclose
    EVT_ALERT_CLOSED,       // Alert popup closed

    EVT_EARPIECE_PLUGGED,
    EVT_EARPIECE_UNPLUGGED,
    
    EVT_BROWSER_UI_UPDATE,
    EVT_BROWSER_PLAY_CLICKED,
    EVT_BROWSER_SETTING_CLOSED,

    EVT_PLAYER_UI_UPDATE,
    EVT_PLAYER_PLAY_CLICKED,
    EVT_PLAYER_UP_CLICKED,
    EVT_PLAYER_DOWN_CLICKED,
    EVT_PLAYER_PLAY_NEXT,
    EVT_PLAYER_SONG_ENDED,
};
