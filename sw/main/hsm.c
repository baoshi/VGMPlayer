#include "hsm.h"


static event_t const evt_start = { EVT_START, 0 };
static event_t const evt_entry = { EVT_ENTRY, 0 };
static event_t const evt_exit = { EVT_EXIT, 0 };


#define MAX_STATE_NESTING 8


void state_ctor(state_t* me, char const* name, state_t* super, event_handler_t handler)
{
    me->name = name;
    me->super = super;
    me->handler = handler;
}


void hsm_ctor(hsm_t* me, char const* name, event_handler_t top_handler)
{
    state_ctor(&me->top, "top", 0, top_handler);
    me->name = name;
}


void hsm_on_start(hsm_t* me)
{
    me->curr = &me->top;
    me->next = 0;
    HANDLE_EVENT(me->curr, me, &evt_entry);   /* call top state handler with ENTRY event */
    /* if next state is a sub state, revursively call super states' entry action */
    while (HANDLE_EVENT(me->curr, me, &evt_start), me->next)
    {
        state_t* path[MAX_STATE_NESTING];
        register state_t** trace = path;
        register state_t* s;
        *trace = 0;
        for (s = me->next; s != me->curr; s = s->super) /* list of states from next state to current - 1 */
        {
            *(++trace) = s;
        }
        while (s = *trace--)    /* invoke all the entry actions from curr - 1 to next*/
        {
            HANDLE_EVENT(s, me, &evt_entry);
        }
        me->curr = me->next;    /* settle at next state */
        me->next = 0;
    }
}


void hsm_on_event(hsm_t* me, event_t const* evt)
{
    state_t* path[MAX_STATE_NESTING];
    register state_t** trace;
    register state_t* s;
    for (s = me->curr; s; s = s->super)     /* travel upwards */
    {
        me->source = s;                     /* level of outermost event handler */
        evt = HANDLE_EVENT(s, me, evt);
        if (evt == 0)                       /* HANDLE_EVENT returns 0 if event is processed */
        {
            if (me->next)                   /* Is state transition happening ? */
            {
                trace = path;
                *trace = 0;
                for (s = me->next; s != me->curr; s = s->super) 
                {
                    *(++trace) = s;         /* trace path to next state  */
                }
                while (s = *trace--)        /* invoke all entry action include next state */
                {
                    HANDLE_EVENT(s, me, &evt_entry);
                }
                me->curr = me->next;        /* set next state */
                me->next = 0;
                while (HANDLE_EVENT(me->curr, me, &evt_start), me->next)      /* start next state (may transit again) */
                {
                    trace = path;
                    *trace = 0;
                    for (s = me->next; s != me->curr; s = s->super) 
                    {
                        *(++trace) = s;     /* trace path to next state */
                    }
                    while (s = *trace--)    /* invoke all entry action include next state */
                    {
                        HANDLE_EVENT(s, me, &evt_entry);
                    }
                    me->curr = me->next;    /* recursive */
                    me->next = 0;
                }
            }
            break;                          /*event processed, don't esculate to superstate */
        }
    }
}


/* find # of levels to Least Common Ancestor */
uint8_t hsm_to_lca_(hsm_t* me, state_t* target) 
{
    state_t *s, *t;
    uint8_t levels = 0;
    if (me->source == target) 
    {
        return 1;
    }
    for (s = me->source; s; ++levels, s = s->super) 
    {
        for (t = target; t; t = t->super) 
        {
            if (s == t) 
            {
                return levels;
            }
        }
    }
    return 0;
}


/* exit current states and all superstates up to LCA */
void hsm_exit_(hsm_t* me, uint8_t levels) 
{
    register state_t* s = me->curr;
    while (s != me->source) 
    {
        HANDLE_EVENT(s, me, &evt_exit);
        s = s->super;
    }
    while (levels--) 
    {
        HANDLE_EVENT(s, me, &evt_exit);
        s = s->super;
    }
    me->curr = s;
}

