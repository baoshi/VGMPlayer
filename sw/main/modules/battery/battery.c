#include "ec.h"
#include "battery.h"

#define BATTERY_LEVEL_100   4.0f
#define BATTERY_LEVEL_75    3.8f
#define BATTERY_LEVEL_50    3.6f
#define BATTERY_LEVEL_25    3.4f
#define BATTERY_LEVEL_0     3.2f
#define BATTERY_LEVEL_BAD   2.0f
#define BATTERY_LEVEL_HYST  0.05f

battery_state_t battery_get_state()
{
    static battery_state_t state = BATTERY_STATE_UNKNOWN;

    switch (state)
    {
    case BATTERY_STATE_UNKNOWN:
        if (ec_charge)
        {
            state = BATTERY_STATE_CHARGING;
        }
        else
        {
            if (ec_battery > BATTERY_LEVEL_100)
                state = BATTERY_STATE_100;
            else if (ec_battery > BATTERY_LEVEL_75)
                state = BATTERY_STATE_75;
            else if (ec_battery > BATTERY_LEVEL_50)
                state = BATTERY_STATE_50;
            else if (ec_battery > BATTERY_LEVEL_25)
                state = BATTERY_STATE_25;
            else if (ec_battery > BATTERY_LEVEL_0)
                state = BATTERY_STATE_0;
        }
        break;
    case BATTERY_STATE_CHARGING:
        if (!ec_charge)
        {
             if (ec_battery > BATTERY_LEVEL_100)
                state = BATTERY_STATE_100;
            else if (ec_battery > BATTERY_LEVEL_75)
                state = BATTERY_STATE_75;
            else if (ec_battery > BATTERY_LEVEL_50)
                state = BATTERY_STATE_50;
            else if (ec_battery > BATTERY_LEVEL_25)
                state = BATTERY_STATE_25;
            else if (ec_battery > BATTERY_LEVEL_0)
                state = BATTERY_STATE_0;
            else
                state = BATTERY_STATE_UNKNOWN;
        }
        break;
    case BATTERY_STATE_100:
        if (ec_charge)
        {
            state = BATTERY_STATE_CHARGING;
        }
        else
        {
            if (ec_battery < (BATTERY_LEVEL_75 - BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_75;
        }
        break;
    case BATTERY_STATE_75:
        if (ec_charge)
        {
            state = BATTERY_STATE_CHARGING;
        }
        else
        {
            if (ec_battery > (BATTERY_LEVEL_100 + BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_100;
            else if (ec_battery < (BATTERY_LEVEL_50 - BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_50;
        }
        break;
    case BATTERY_STATE_50:
        if (ec_charge)
        {
            state = BATTERY_STATE_CHARGING;
        }
        else
        {
            if (ec_battery > (BATTERY_LEVEL_75 + BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_75;
            else if (ec_battery < (BATTERY_LEVEL_25 - BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_25;
        }
        break;
    case BATTERY_STATE_25:
        if (ec_charge)
        {
            state = BATTERY_STATE_CHARGING;
        }
        else
        {
            if (ec_battery > (BATTERY_LEVEL_50 + BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_50;
            else if (ec_battery < (BATTERY_LEVEL_0 - BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_0;
        }
        break;
    case BATTERY_STATE_0:
        if (ec_charge)
        {
            state = BATTERY_STATE_CHARGING;
        }
        else
        {
            if (ec_battery > (BATTERY_LEVEL_25 + BATTERY_LEVEL_HYST))
                state = BATTERY_STATE_25;
            else if (ec_battery < BATTERY_LEVEL_BAD)
                state = BATTERY_STATE_UNKNOWN;
        }
        break;
    }
    return state;
}
