#pragma once

#include <stdbool.h>
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EQ_PUSH_BACK(c_)        \
{                               \
    event_t e = {(c_), 0};      \
    event_queue_push_back(&e);  \
}

void event_queue_init(int size);
void event_queue_free();
void event_queue_push_front(event_t const *e);
void event_queue_push_back(event_t const *e);
bool event_queue_pop(event_t* e);
bool event_queue_peek(event_t* e);
int event_queue_length();
void event_queue_clear();

#ifdef __cplusplus
}
#endif

