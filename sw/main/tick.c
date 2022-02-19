#include <string.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/time.h>
#include "my_debug.h"
#include "sw_conf.h"
#include "event_queue.h"
#include "tick.h"


// Tick hooks
tick_hook_t _hook = 0;
void * _hook_param = 0;

// Time event slots
static struct 
{
    int event;
    uint32_t timeout;
    int32_t  counter;
    bool repeat;
} _slots[TICK_MAX_EVENTS];

#define ALARM_UNUSED    0x00
#define ALARM_ARMED     0x01
#define ALARM_PAUSED    0x02
static uint8_t _slot_status[TICK_MAX_EVENTS]; // 0-unused; 1-used; 2-paused


static uint32_t _millis;


void isr_systick()
{
    _millis += APP_TICK_PERIOD_MS;
    // Call hook if installed
    if (_hook) _hook(_hook_param);
    // Send time events
    for (int i = 0; i < TICK_MAX_EVENTS; ++i)
    {
        if (ALARM_ARMED == _slot_status[i])
        {
            if (0 == _slots[i].timeout)   // send along systick event
            {
                event_t e = 
                {
                    .code = _slots[i].event,
                    .param = (void *)_millis
                };
                event_queue_push_back(&e);
            }
            else
            {
                _slots[i].counter += APP_TICK_PERIOD_MS;
                if (_slots[i].counter >= _slots[i].timeout)
                {
                    event_t e = 
                    {
                        .code = _slots[i].event,
                        .param = (void *)_millis
                    };
                    event_queue_push_back(&e);
                    // reset counter for repeated event or delete this client
                    if (_slots[i].repeat)
                    {
                        _slots[i].counter -= _slots[i].timeout;
                    }
                    else
                    {
                        _slot_status[i] = ALARM_UNUSED;
                    }
                }
            }
        }
    }
}


/**
 * @brief Initialize SysTick
 * 
 */
void tick_init()
{
    uint32_t sysclk = clock_get_hz(clk_sys);
    systick_hw->rvr = sysclk / 1000 * APP_TICK_PERIOD_MS - 1;
    systick_hw->cvr = 0;
    systick_hw->csr = 0x07;     // [31:17]  -
                                // [16]     COUNTFLAG   RO
                                // [15:3]   -
                                // [2]      CLKSOURCE = 1 (Processor clock)
                                // [1]      TICKINC = 1 (Counting down)
                                // [0]      ENABLE = 1
                                // 0000 0000 0000 0000 0000 0000 0000 0111
    for (int i = 0; i < TICK_MAX_EVENTS; ++i)
        _slot_status[i] = 0;
    _millis = 0;
}


/**
 * @brief Register a hook function that is called within SysTick ISR
 * 
 * @param hook      Hook function
 * @param param     Parameter for the hook function
 */
void tick_register_hook(tick_hook_t hook, void* param)
{
    _hook = hook;
    _hook_param = param;
}


/**
 * @brief De-register time event
 * @param id    Alarm id
 **/
void tick_disarm_time_event(int id)
{
    MY_ASSERT((id >= 0) && (id < TICK_MAX_EVENTS));
    _slot_status[id] = ALARM_UNUSED;
}


/**
 * @brief Schedule alarm event using systick. Note there is max +TICK_PERIOD_MS error in the actual delay before alarm
 * @param timeout_ms    Alarm time. Set to 0 to alarm at TICK_PERIOD_MS
 * @param repeat        If the alarm is to be repeated every timeout_ms
 * @param event         Event code. Time event will be posted to the global event queue with millis as param
 * @param start         If counting is to start immediately. If false, use tick_resume_time_event() to start counting
 * @return              Alarm id (for cancellation). Non-repeating alarm will be cleared automatically after the event is fired
 **/
int tick_arm_time_event(uint32_t timeout_ms, bool repeat, int event, bool start)
{
    int ret = -1;
    for (int i = 0; i < TICK_MAX_EVENTS; ++i)
    {
        if (ALARM_UNUSED == _slot_status[i])
        {
            _slots[i].event = event;
            _slots[i].repeat = repeat;
            _slots[i].timeout = timeout_ms;
            // Adjust for already elapsed cycles
            _slots[i].counter = - (APP_TICK_PERIOD_MS - (APP_TICK_PERIOD_MS * systick_hw->cvr / systick_hw->rvr));
            _slot_status[i] = start ? ALARM_ARMED : ALARM_PAUSED;
            ret = i;
            break;
        }
    }
    if (ret == -1)
    {
        MY_ASSERT("Too many systick time event");
    }
    return ret;
}


/**
 * @brief Reset an alarm (restart counting from 0), will enable this alarm too
 * 
 * @param id    Alarm id
 */
void tick_reset_time_event(int id)
{
    MY_ASSERT((id >= 0) && (id < TICK_MAX_EVENTS));
    _slots[id].counter = - (APP_TICK_PERIOD_MS - (APP_TICK_PERIOD_MS * systick_hw->cvr / systick_hw->rvr));
    _slot_status[id] = ALARM_ARMED;
}


/**
 * @brief Pause an alarm
 * 
 * @param id    Alarm id
 */
void tick_pause_time_event(int id)
{
    MY_ASSERT((id >= 0) && (id < TICK_MAX_EVENTS));
    _slot_status[id] = ALARM_PAUSED;
}


/**
 * @brief Resume alarm counting
 * 
 * @param id    Alarm id
 */
void tick_resume_time_event(int id)
{
    MY_ASSERT((id >= 0) && (id < TICK_MAX_EVENTS));
    _slot_status[id] = ALARM_ARMED;
}


uint32_t tick_millis()
{
    return _millis;
}

