#pragma once

#include <stdbool.h>
#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EQ_QUICK_PUSH(c_)               \
do                                      \
{                                       \
    event_t e = {(c_), 0};              \
    event_queue_push_back(&e, true);    \
} while (0)                             \

#define EQ_QUICK_PUSH_IMPORTANT(c_)     \
do                                      \
{                                       \
    event_t e = {(c_), 0};              \
    event_queue_push_back(&e, false);   \
} while (0)

void event_queue_init(int size);
void event_queue_free();
void event_queue_push_back(event_t const* e, bool merge);
bool event_queue_pop(event_t* e);
bool event_queue_peek(event_t* e);
void event_queue_clear();

#ifdef __cplusplus
}
#endif

