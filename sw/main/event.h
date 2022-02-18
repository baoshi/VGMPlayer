#ifndef EVENT_H
#define EVENT_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    int   code;     /* event code */
    void* param;    /* event parameter */
} event_t;


#ifdef __cplusplus
}
#endif


#endif

