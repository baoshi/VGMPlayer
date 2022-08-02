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
    EVT_OPEN_VOLUME_POPUP,          // User pressed [SETTING] in player state
    EVT_CLOSE_VOLUME_POPUP,         // User pressed [BACK] to close volume popup
    EVT_CLOSE_VOLUME_POPUP_NEXT,    // User pressed [SETTING] when volume popup is active

    EVT_CLOSE_ALERT,                // Alert popup ask to close by timer or manually

    EVT_AUDIO_SAMPLE_READY,         // Audio samples are ready. Parameter is audio_buffer_t pointer
    EVT_AUDIO_SONG_ENDED,           // Audio decoding ended (application need call audio_finish_playback())

    EVT_HEADPHONE_PLUGGED,
    EVT_HEADPHONE_UNPLUGGED,
    
    EVT_BROWSER_UI_UPDATE,
    EVT_BROWSER_PLAY_CLICKED,

    EVT_PLAYER_UI_UPDATE,
    EVT_PLAYER_PLAY_NEXT,
};
