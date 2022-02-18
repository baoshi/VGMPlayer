#include <string.h>
#include <pico/lock_core.h>
#include "my_debug.h"
#include "my_mem.h"
#include "event_queue.h"


static struct event_queue_s
{
    lock_core_t core;
    int size;
    int in;
    int out;
    event_t* elems;
}  _event_queue = { 0 };


void event_queue_init(int size)
{
    event_t* storage = MY_MALLOC(sizeof(event_t) * (size + 1));
    MY_ASSERT(storage != 0);
    lock_init(&(_event_queue.core), next_striped_spin_lock_num());
    _event_queue.out = 0;
    _event_queue.in = 0;
    _event_queue.size = size;
    _event_queue.elems = storage;
}


void event_queue_free()
{
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    _event_queue.in = _event_queue.out = 0;
    if (_event_queue.elems) MY_FREE(_event_queue.elems);
    spin_unlock(_event_queue.core.spin_lock, save);
}


void event_queue_push_front(event_t const *e)
{
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    bool full = ((_event_queue.in + 1) % (_event_queue.size + 1) == _event_queue.out);
    if (!full)
    {
        if (_event_queue.out > 0)
        {
             _event_queue.out -= 1;
        }
        else
        {
            _event_queue.out = _event_queue.size;
        }
        memcpy(_event_queue.elems + _event_queue.out, e, sizeof(event_t));
    }
    spin_unlock(_event_queue.core.spin_lock, save);
    MY_ASSERT(full == false);
}


void event_queue_push_back(event_t const *e)
{
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    bool full = ((_event_queue.in + 1) % (_event_queue.size + 1) == _event_queue.out);
    if (!full)
    {
        memcpy(_event_queue.elems + _event_queue.in, e, sizeof(event_t));
        _event_queue.in = (_event_queue.in + 1) % (_event_queue.size + 1);
    }
    spin_unlock(_event_queue.core.spin_lock, save);
    MY_ASSERT(full == false);
}


bool event_queue_pop(event_t* e)
{
    bool ret = false;
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    if (_event_queue.out != _event_queue.in)
    {
        memcpy(e, _event_queue.elems + _event_queue.out, sizeof(event_t));
        _event_queue.out = (_event_queue.out + 1) % (_event_queue.size + 1);
        ret = true;
    }
    spin_unlock(_event_queue.core.spin_lock, save);
    return ret;
}


bool event_queue_peek(event_t* e)
{
    bool ret = false;
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    if (_event_queue.out != _event_queue.in)
    {
        memcpy(e, _event_queue.elems + _event_queue.out, sizeof(event_t));
        ret = true;
    }
    spin_unlock(_event_queue.core.spin_lock, save);
    return ret;
}


int event_queue_length()
{
    int ret = 0;
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    ret =  (_event_queue.in - _event_queue.out + _event_queue.size + 1) % (_event_queue.size + 1);
    spin_unlock(_event_queue.core.spin_lock, save);
    return ret;
}


void event_queue_clear()
{
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    _event_queue.in = _event_queue.out = 0;
    spin_unlock(_event_queue.core.spin_lock, save);
}
