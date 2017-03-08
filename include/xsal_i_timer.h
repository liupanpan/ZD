#ifndef XSAL_I_TIMER_H
#define XSAL_I_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_i_thread.h"

typedef struct SAL_I_Timer_Tag
{
   SAL_Event_Id_T event_id;

   SAL_Thread_Id_T thread_id;

   struct SAL_I_Timer_Tag* prev_thread_timer;

   struct SAL_I_Timer_Tag* next_thread_timer;

   bool use_param;

   uintptr_t param;

   struct SAL_I_Timer_Tag* next_timer_by_time;
   struct SAL_I_Timer_Tag* prev_timer_by_time;

   struct timespec expiration_time;
   uint32_t period;
} SAL_I_Timer_T;

/** Pointer to the SAL Timer's table
 */
extern SAL_I_Timer_T* SAL_I_Timers;

/** Pointer to the SAL Timer's free list
 */
extern SAL_I_Timer_T* SAL_I_Timers_Free_List;

/** Mutex to synchronize access to the SAL Timers table
 *  and SAL Timers free list.
 */
extern SAL_Mutex_T SAL_I_Timers_Mutex;

/** Function initializes XSAL Timer module.
 *  It allocates memory, creates synchronization objects, ...
 */
bool SAL_I_Init_Timer_Module(void);

/** Function starts module/thread for SAL timer.
 */
void SAL_I_Start_Timer_Module(void);

/** Router Thread Light main function.
 */
int32_t SAL_I_RT_Light(void);

/** Function terminates Router Thread Light.
 */
void SAL_I_Stop_RT_Light(int32_t status);

/** Function creates timer with given ID.
 */
bool SAL_I_Create_Timer(SAL_I_Timer_T* timer);


/** Function destroys given timer.
 */
void SAL_I_Destroy_Timer(SAL_I_Timer_T* timer);

/** Function starts given timer.
 */
void SAL_I_Start_Timer(SAL_I_Timer_T* timer,uint32_t interval_msec,bool is_periodic,bool use_param,uintptr_t param);

/** Function stops given timer.
 */
void SAL_I_Stop_Timer(SAL_I_Timer_T* timer);

/** Function creates and post message to the message queue.
 */
void SAL_I_Post_Timer_Event(SAL_Event_Id_T event_id,SAL_Thread_Id_T thread_id,bool use_param,uintptr_t param);

/** Function binds timer with given id to the given thread.
 */
void SAL_I_Bind_Timer_To_Thread(SAL_I_Timer_T* timer);

/**
 *  Function unbinds timer with given id to the given thread.
 */
void SAL_I_Unbind_Timer_From_Thread(SAL_I_Timer_T* timer);

/** Function destroys all timers created by given thread.
 */
void SAL_I_Destroy_And_Unbind_Timers_From_Thread(SAL_I_Thread_Attr_T* thread_attr);

/** Function destroys all timers created by given thread.
 */
void SAL_I_Destroy_And_Unbind_Timers_From_Thread(SAL_I_Thread_Attr_T* thread_attr);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_TIMER_H */



