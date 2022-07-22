/*
 * Most of the code is from https://github.com/MadsAndreasen/HierarchicalStateMachine
 * In the original repo author has mentioned corrections on inherited transitions. However,
 * I choose to revert it because of the scenario needed in this project. Consider this example:
 * 
 *    S1        S1 is the parent state and S11, S111 are sub states.
 *     |        S1 has a handler for event e1. S11 does not handle e1.
 *     |        When e1 arrives, we shall transit from current state to S111.
 *    S11
 *     |        Assume we are currently at S11, e1 will be handled by higher S1 handler.
 *     |
 *    S111      In the original implementation, LCA calculation is done from handler's state to the 
 *              desitnation state (S1 to S111), and these steps will happen during transition:
 *              (S11 Exit) -> (S11 Entry) -> (S111 Entry). S11 Exit/Entry seems redundant.
 *
 *              In my version, LCA calculation is done from the current state to the destination state
 *              (S11 to S111), only (S111 Entry) will happen.
 * 
 * I also removed the static LCA calculation. This allows dynamic states attachment. See settings and alert state
 * for example.
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
/* note levels_ is always calculated so this macro can be */        \
/* used with dynamically created states */                          \
#define STATE_TRAN(me_, target_)                                    \
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
