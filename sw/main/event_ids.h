#pragma once


enum
{
    EVT_APP_UI_UPDATE = 1,
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

    EVT_AUDIO_PROGRESS,             // Audio progress, parameter is pointer to audio_progress_t
    EVT_AUDIO_SONG_TERMINATED,      // Audio decoding ended (application need call audio_finish_playback())
    EVT_AUDIO_SONG_FINISHED,

    EVT_HEADPHONE_PLUGGED,
    EVT_HEADPHONE_UNPLUGGED,
    
    EVT_BROWSER_PLAY_CLICKED,

    EVT_PLAYER_PLAY_NEXT_OR_ALERT,
    EVT_PLAYER_PLAY_NEXT_OR_BACK
};
