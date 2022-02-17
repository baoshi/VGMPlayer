#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <pico/sem.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    int   code;     /* event code */
    void* param;    /* event parameter */
} event_t;


typedef struct event_queue_s
{
    semaphore_t sem;
    int size;
    int in;
    int out;
    event_t* elems;
} event_queue_t;


void event_queue_init(event_queue_t *q);
void event_queue_push_front(event_queue_t *q, event_t const *e);
void event_queue_push_back(event_queue_t* q, event_t const *e);
bool event_queue_pop(event_queue_t* q, event_t* e);
bool event_queue_peek(event_queue_t *q, event_t* e);

int event_queue_length(event_queue_t* q);
void event_queue_clear(event_queue_t* q);

extern event_queue_t app_event_queue;

#ifdef __cplusplus
}
#endif


#endif

