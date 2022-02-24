#include <string.h>
#include <pico/lock_core.h>
#include "my_debug.h"
#include "my_mem.h"
#include "event_queue.h"

typedef struct event_node_s event_node_t;


struct event_node_s
{
    event_t event;
    event_node_t *prev;
    event_node_t *next;
};


static struct event_queue_s
{
    lock_core_t core;
    int size;
    event_node_t *head, *tail; // doubly linked list of current events
    event_node_t *unused;      // linked list of unused event blocks
    event_node_t* storage;     // for easy release
} _event_queue = { 0 };


void event_queue_init(int size)
{
    event_node_t *storage = MY_MALLOC(sizeof(event_node_t) * size);
    MY_ASSERT(storage != 0);
    // Link all nodes up
    for (int i = 0; i < size - 1; ++i)
    {
        storage[i].next = &(storage[i + 1]);
    }
    storage[size - 1].next = 0;
    lock_init(&(_event_queue.core), next_striped_spin_lock_num());
    _event_queue.size = size;
    _event_queue.storage = storage;
    _event_queue.head = 0;
    _event_queue.tail = 0;
    _event_queue.unused = storage;
}


void event_queue_free()
{
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    MY_FREE(_event_queue.storage);
    _event_queue.size = 0;
    _event_queue.storage = 0;
    _event_queue.head = 0;
    _event_queue.tail = 0;
    _event_queue.unused = 0;
    spin_unlock(_event_queue.core.spin_lock, save);
}


void event_queue_push_back(event_t const *e, bool merge)
{
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    if (merge)
    {
        // in merge mode, traverse the list (backwards). If a same event found,
        // modify it with the current value then move it to the back.
        event_node_t* t = _event_queue.tail;
        while (t && (t->event.code != e->code))
        {
            t = t->prev;
        }
        if (t)
        {
            // found, replace content and move it to the back
            t->event.param = e->param;
            if (t == _event_queue.tail)
            {
                // t is the tail, do nothing
            }
            else if (t == _event_queue.head)
            {
                // t is the head, update new head
                _event_queue.head = t->next;
                _event_queue.head->prev = 0;
                // move t to last
                t->prev = _event_queue.tail;
                _event_queue.tail->next = t;
                t->next = 0;
                _event_queue.tail = t;
            }
            else
            {
                // t is in the middle, remove t
                t->prev->next = t->next;
                t->next->prev = t->prev;
                // move t to last
                t->prev = _event_queue.tail;
                _event_queue.tail->next = t;
                t->next = 0;
                _event_queue.tail = t;
            }
        }
        else
        {
            // no same event found, treat as not merge
            merge = false;
        }
    }
    if (!merge)
    {
        // take one unused node
        MY_ASSERT(_event_queue.unused != 0);
        event_node_t* t = _event_queue.unused;
        _event_queue.unused = t->next;
        t->event.code = e->code;
        t->event.param = e->param;
        t->next = 0;
        // insert into tail
        if (_event_queue.head == 0)
        {
            t->prev = 0;
            _event_queue.head = t;
            _event_queue.tail = t;
        }
        else
        {
            t->prev = _event_queue.tail;
            _event_queue.tail->next = t;
            _event_queue.tail = t;
        }
    }
    spin_unlock(_event_queue.core.spin_lock, save);
}


bool event_queue_pop(event_t* e)
{
    bool ret = false;
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    if (_event_queue.head != 0)
    {
        event_node_t* t = _event_queue.head;
        e->code = t->event.code;
        e->param = t->event.param;
        if (t->next == 0)
        {
            // t is the only one node in the list
            _event_queue.head = 0;
            _event_queue.tail = 0;
        }
        else
        {
            // update head
            _event_queue.head = t->next;
            _event_queue.head->prev = 0;
        }
        // return t to unused
        t->next = _event_queue.unused;
        _event_queue.unused = t;
        ret = true;
    }
    spin_unlock(_event_queue.core.spin_lock, save);
    return ret;

}


bool event_queue_peek(event_t* e)
{
    bool ret = false;
    uint32_t save = spin_lock_blocking(_event_queue.core.spin_lock);
    if (_event_queue.head != 0)
    {
        event_node_t* t = _event_queue.head;
        e->code = t->event.code;
        e->param = t->event.param;
    }
    spin_unlock(_event_queue.core.spin_lock, save);
    return ret;
}
