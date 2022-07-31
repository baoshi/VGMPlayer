#include <string.h>
#include "sw_conf.h"
#include "my_debug.h"
#include "app.h"


void close_all_popups(app_t *app)
{
    if (app->alert_ctx.popup) alert_close(app);
    if (app->brightness_ctx.popup) brightness_close(app);
    if (app->volume_ctx.popup) volume_close(app);
}


bool is_volume_popup(app_t *app)
{
    return (app->volume_ctx.popup != NULL);
}