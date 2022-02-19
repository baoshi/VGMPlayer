#include <string.h>
#include <hardware/clocks.h>
#include <hardware/structs/systick.h>
#include <pico/time.h>
#include "my_debug.h"
#include "event.h"
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
static bool _slot_used[TICK_MAX_EVENTS];


static uint32_t _millis;


void isr_systick()
{
    _millis += TICK_PERIOD_MS;
    // Call hook if installed
    if (_hook) _hook(_hook_param);
    // Send time events
    for (int i = 0; i < TICK_MAX_EVENTS; ++i)
    {
        if (_slot_used[i])
        {
            if (_slots[i].timeout == 0)   // send along systick event
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
                _slots[i].counter += TICK_PERIOD_MS;
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
                        _slot_used[i] = false;
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
    systick_hw->rvr = sysclk / 1000 * TICK_PERIOD_MS - 1;
    systick_hw->cvr = 0;
    systick_hw->csr = 0x07;     // [31:17]  -
                                // [16]     COUNTFLAG   RO
                                // [15:3]   -
                                // [2]      CLKSOURCE = 1 (Processor clock)
                                // [1]      TICKINC = 1 (Counting down)
                                // [0]      ENABLE = 1
                                // 0000 0000 0000 0000 0000 0000 0000 0111
    for (int i = 0; i < TICK_MAX_EVENTS; ++i)
        _slot_used[i] = false;
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
    if (id >= 0 && id < TICK_MAX_EVENTS)
        _slot_used[id] = false;
}


/**
 * @brief Schedule alarm event using systick. Note there is max +TICK_PERIOD_MS error in the actual delay before alarm
 * @param timeout_ms    Alarm time. Set to 0 to alarm at TICK_PERIOD_MS
 * @param repeat        If the alarm is to be repeated every timeout_ms
 * @param event         Event code. Time event will be posted to the global event queue with millis as param
 * @return              Alarm id (for cancellation). Non-repeating alarm will be cleared automatically after the event is fired
 **/
int tick_arm_time_event(uint32_t timeout_ms, bool repeat, int event)
{
    int ret = -1;
    for (int i = 0; i < TICK_MAX_EVENTS; ++i)
    {
        if (false == _slot_used[i])
        {
            _slots[i].event = event;
            _slots[i].repeat = repeat;
            _slots[i].timeout = timeout_ms;
            // Adjust for already elapsed cycles
            _slots[i].counter = - (TICK_PERIOD_MS - (TICK_PERIOD_MS * systick_hw->cvr / systick_hw->rvr));
            _slot_used[i] = true;
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


uint32_t tick_millis()
{
    return _millis;
}