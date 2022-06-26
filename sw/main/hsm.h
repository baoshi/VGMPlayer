/*
 * https://github.com/MadsAndreasen/HierarchicalStateMachine
 */ 

#ifndef HSM_H
#define HSM_H

#include <assert.h>
#include <stdint.h>

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* Reserved event ids */
#define EVT_START   -1
#define EVT_ENTRY   -2
#define EVT_EXIT    -3


typedef struct hsm_s hsm_t;

typedef event_t const * (*event_handler_t)(hsm_t *, event_t const *);

typedef struct state_s state_t;

struct state_s
{
    char const *name;           /* state name */
    state_t *super;             /* pointer to superstate */
    event_handler_t handler;    /* state's handler function */
};


void state_ctor(state_t *me, char const *name, state_t *super, event_handler_t handler);


#define HANDLE_EVENT(me_, ctx_, evt_) (*(me_)->handler)((ctx_), (evt_))


struct hsm_s
{
    char const *name;             /* state machine name */
    state_t *curr;                /* current state */
    state_t *next;                /* next state */
    state_t top;                  /* top-level state object */
};


/* Construct HSM */
void hsm_ctor(hsm_t *me, char const *name, event_handler_t top_handler);

/* Enter and start top state */
void hsm_on_start(hsm_t *me);

/* Main HSM event engine */
void hsm_on_event(hsm_t *me, event_t const *evt);


/* protected: */
uint8_t hsm_to_lca_(hsm_t *me, state_t *target);
void hsm_exit_(hsm_t *me, uint8_t levels);

    /* get current state */
#define STATE_CURR(me_) (((hsm_t *)me_)->curr)


/* take start transition without exist any superstates */
#define STATE_START(me_, target_) (((hsm_t *)me_)->next = (target_))

/* state transition including exit superstates up to LCA */
/* note levels_ is static so need to calc once */
#define STATE_TRAN(me_, target_)                                \
    do                                                          \
    {                                                           \
        static uint8_t levels_ = 0xFF;                          \
        assert(((hsm_t *)me_)->next == 0);                      \
        if (levels_ == 0xFF)                                    \
            levels_ = hsm_to_lca_((hsm_t *)(me_), (target_));   \
        hsm_exit_((hsm_t *)(me_), levels_);                     \
        ((hsm_t *)(me_))->next = (target_);                     \
    } while (0)


/* state transition including exit superstates up to LCA */
/* note levels_ is always calculated so this macro can be */        \
/* used with dynamically created states */                          \
#define STATE_TRAN_DYNAMIC(me_, target_)                            \
    do                                                              \
    {                                                               \
        assert(((hsm_t *)me_)->next == 0);                          \
        uint8_t levels_ = hsm_to_lca_((hsm_t *)(me_), (target_));   \
        hsm_exit_((hsm_t *)(me_), levels_);                         \
        ((hsm_t *)(me_))->next = (target_);                         \
    } while (0)



#ifdef __cplusplus
}
#endif


#endif /* HSM_H */
