#pragma once


enum
{
    EVT_APP_TICK = 1,
    EVT_EC_FAILED,
    EVT_DISK_INSERTED,
    EVT_DISK_EJECTED,
    EVT_DISK_ERROR,

    EVT_BUTTON_PLAY_CLICKED,
    EVT_BUTTON_UP_CLICKED,
    EVT_BUTTON_DOWN_CLICKED,
    EVT_BUTTON_BACK_CLICKED,

    EVT_OPEN_BRIGHTNESS_POPUP,      // User pressed [SETTING] top open brightness popup (in top state)
    EVT_CLOSE_BRIGHTNESS_POPUP,     // User pressed [BACK] to close brightness popup
    EVT_OPEN_VOLUME_POPUP,          // User pressed [SETTING] to open volume popup (in player state)
    EVT_CLOSE_VOLUME_POPUP_NEXT,    // User pressed [SETTING] when volume popup is active
    EVT_CLOSE_VOLUME_POPUP,         // User pressed [BACK] to close volume popup

    EVT_SETTING_CLOSED,     // Setting popup closed

    EVT_ALERT_MANUAL_CLOSE, // Alert popup manual close
    EVT_ALERT_AUTO_CLOSE,   // Alert popup autoclose
    EVT_ALERT_CLOSED,       // Alert popup closed

    EVT_AUDIO_SONG_ENDED,   // Audio decoding ended (application need call audio_finish_playback())

    EVT_EARPIECE_PLUGGED,
    EVT_EARPIECE_UNPLUGGED,
    
    EVT_BROWSER_UI_UPDATE,
    EVT_BROWSER_PLAY_CLICKED,
    EVT_BROWSER_SETTING_CLOSED,

    EVT_PLAYER_UI_UPDATE,
    EVT_PLAYER_PLAY_NEXT,
};
