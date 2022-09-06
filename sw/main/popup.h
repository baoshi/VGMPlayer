#pragma once


typedef struct app_s app_t;


void brightness_popup(app_t *app);
void brightness_close(app_t *app);


void volume_popup(app_t *app);
void volume_close(app_t *app);
void volume_popup_refresh(app_t *app);


void alert_popup(app_t *app, const void *icon, const char *text, int auto_close, int exit_event);
void alert_close(app_t *app);


void close_all_popups(app_t *app);

bool is_volume_popup(app_t *app);

