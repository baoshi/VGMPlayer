#include <pico.h>
#include <pico/stdlib.h>
#include <pico/types.h>
#include <pico/time.h>
#include <hardware/i2c.h>
#include "hw_conf.h"
#include "my_debug.h"
#include "ec.h"

#define EC_MAX_FAILURE_COUNT 10

#ifndef EC_DEBUG
# define EC_DEBUG 0
#endif


// Debug log
#if EC_DEBUG
#define EC_LOGD(x, ...)      MY_LOGD(x, ##__VA_ARGS__)
#define EC_LOGW(x, ...)      MY_LOGW(x, ##__VA_ARGS__)
#define EC_LOGE(x, ...)      MY_LOGE(x, ##__VA_ARGS__)
#define EC_LOGI(x, ...)      MY_LOGI(x, ##__VA_ARGS__)
#define EC_DEBUGF(x, ...)    MY_DEBUGF(x, ##__VA_ARGS__)
#else
#define EC_LOGD(x, ...)
#define EC_LOGW(x, ...)
#define EC_LOGE(x, ...)
#define EC_LOGI(x, ...)
#define EC_DEBUGF(x, ...)
#endif


bool ec_charge;      // true if charging
float ec_battery;    // battery voltage
uint8_t ec_buttons;   

static uint8_t _data[2] = { 0, 0 };
static int _failure_count;


// I2C address: 0x13
//
// Master read: return 2 bytes
// 1: Status  [ x x CHG SE NE PLAY SW NW]
//            bit 7: Unimplemented
//            bit 6: Unimplemented
//            bit 5: CHG battery is charging = 1
//            bit 4: SE key status, debounced, pressed = 1
//            bit 3: NE key status, debounced, pressed = 1
//            bit 2: PLAY key status, debounced, pressed = 1
//            bit 1: SW key status, debounced, pressed = 1
//            bit 0: NW key status, debounced, pressed = 1
// 2: Battery [7 6 5 4 3 2 1 0]    
//            V_Battery = Battery / 30.0f


// Decode _data into ec_usb, ec_charge, ec_battery
static void _decode(void)
{
    // Status  [ X X CHG SE NE PLAY SW NW]
    ec_charge = (_data[0] & 0x20) ? true : false;
    ec_buttons = _data[0] & 0x1F;
    ec_battery = (float)(_data[1]) / 30.0f;
}


void ec_init(void)
{
    _failure_count = 0;
    // try 10 times
    i2c_lock();
    do
    {
        if (i2c_read_timeout_us(I2C_INST, 0x13, _data, 2, false, I2C_TIMEOUT_US) == 2)
        {
            _failure_count = 0;
            _decode();
            break;
        }
        ++_failure_count;
    } while (_failure_count < 10);
    i2c_unlock();
    // If fail, _failure_count == 10 when exit
}


/**
 * @brief Perform EC polling
 * 
 * @param now   time
 * @return int  0 if EC is read successfully and nothing in byte 0 has changed since last polling
 *              1 if EC is read sucessfully and something is changed in byte 0
 *             -1 if EC failed for more than EC_MAX_FAILURE_COUNT
 */
int ec_update(uint32_t now)
{
    int ret;
    uint8_t old = _data[0];
    i2c_lock();
    ret = i2c_read_timeout_us(I2C_INST, 0x13, _data, 2, false, I2C_TIMEOUT_US);
    i2c_unlock();
    if (ret == 2)
    {
        _failure_count = 0;
        _decode();
        EC_LOGD("Charger=%d, Button=0x%02x, Batt=%.1fv\n", ec_usb, ec_charge, ec_buttons, ec_battery);
        if (old == _data[0])
            ret = 0;
        else
            ret = 1;
    }
    else
    {
        ret = 0;    // assume no change if failed
        ++_failure_count;
        if (_failure_count >= EC_MAX_FAILURE_COUNT)
            ret = -1;
    }
    return ret;
}
