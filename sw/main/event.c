#include <string.h>
#include "my_debug.h"
#include "event.h"


// global event queue
#define EVENT_QUEUE_LENGTH 8
static event_t _storage[EVENT_QUEUE_LENGTH + 1];
event_queue_t app_event_queue;


void event_queue_init(event_queue_t *q)
{
    sem_init(&(q->sem), 1, 1);  // initial 1, max 1
    q->out = 0;
    q->in = 0;
    q->size = EVENT_QUEUE_LENGTH;
    q->elems = _storage;
}


void event_queue_push_front(event_queue_t *q, event_t const *e)
{
    sem_acquire_blocking(&(q->sem));
    if ((q->in + 1) % (q->size + 1) != q->out)  // if not full?
    {
        if (q->out > 0)
        {
             q->out -= 1;
        }
        else
        {
            q->out = q->size;
        }
        memcpy(q->elems + q->out, e, sizeof(event_t));
        sem_release(&(q->sem));
    }
    else 
    {
        sem_release(&(q->sem));
        MY_ASSERT("Event queue overflow");
    }
}


void event_queue_push_back(event_queue_t* q, event_t const *e)
{
    sem_acquire_blocking(&(q->sem));
    if ((q->in + 1) % (q->size + 1) != q->out) // if not full?
    {
        memcpy(q->elems + q->in, e, sizeof(event_t));
        q->in = (q->in + 1) % (q->size + 1);
        sem_release(&(q->sem));
    }
    else
    {
        sem_release(&(q->sem));
        MY_ASSERT("Event queue overflow");
    }
}


bool event_queue_pop(event_queue_t* q, event_t* e)
{
    bool ret = false;
    sem_acquire_blocking(&(q->sem));
    if (q->out != q->in)
    {
        memcpy(e, q->elems + q->out, sizeof(event_t));
        q->out = (q->out + 1) % (q->size + 1);
        
        ret = true;
    }
    sem_release(&(q->sem));
    return ret;
}


bool event_queue_peek(event_queue_t *q, event_t* e)
{
    bool ret = false;
    sem_acquire_blocking(&(q->sem));
    if (q->out != q->in)
    {
        memcpy(e, q->elems + q->out, sizeof(event_t));
        ret = true;
    }
    sem_release(&(q->sem));
    return ret;
}


int event_queue_length(event_queue_t* q)
{
    int ret = 0;
    sem_acquire_blocking(&(q->sem));
    ret =  (q->in - q->out + q->size + 1) % (q->size + 1);
    sem_release(&(q->sem));
    return ret;
}


void event_queue_clear(event_queue_t* q)
{
    sem_acquire_blocking(&(q->sem));
    q->in = q->out = 0;
    sem_release(&(q->sem));
}



#ifdef EVENT_SELFTEST

#include <stdio.h>

void event_queue_dump(event_queue_t* q)
{
    int i;
    printf("Length = %d\n", event_queue_length(q));
    if (q->out != q->in)
    {
        i = q->out;
        while (i != q->in)
        {
            if (i == q->out)
                printf("[%02d]: %d\t<- out\n", i, q->elems[i].code);
            else
                printf("[%02d]: %d\n", i, q->elems[i].code);
            i = (i + 1) % (q->size + 1);
        }
        printf("[%02d]: -\t<-in\n", i);
    }
}


DECLARE_EVENT_QUEUE(testqueue, 10)


void enqueue_back(int i)
{
    event_t e;
    e.code = i;
    e.param = 0;
    if (!event_queue_push_back(&testqueue, &e)) printf("Failed to add event\n");
}


void enqueue_front(int i)
{
    event_t e;
    e.code = i;
    e.param = 0;
    if (!event_queue_push_front(&testqueue, &e)) printf("Failed to add event\n");
}


void dequeue()
{
    event_t e;
    if (event_queue_pop(&testqueue, &e))
        printf("Dequeue %d\n", e.code);
}


int main()
{
    int i;
    for (i = 0; i < 12; ++i)
        enqueue_front(i);
    event_queue_dump(&testqueue);
    dequeue();
    dequeue();
    event_queue_dump(&testqueue);
    return 0;
}

#endif