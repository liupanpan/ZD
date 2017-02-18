#ifndef XSAL_TIME_H
#define XSAL_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "zd_data_types_linux.h"
#include <time.h>
#include <stdint.h> 

/** Timer-related data structure
 */
typedef struct ZD_I_Timer_Tag
{
   /** SAL event identifier
    */
   int32_t event_id;

   /** ID of the thread which is the owner of the timer
    */
   int32_t thread_id;

   /** If given timer is used by thread (ZD_Create_Timer was called) then 
    *  this field points to previous timer owned by thread, otherwise it
    *  points to next free timer in the free timers list (with 
    *  ZD_I_Free_Timers_List as the head of the list)
    */
   struct ZD_I_Timer_Tag* prev_thread_timer;

   /** If given timer is used by thread (ZD_Create_Timer was called) then 
    *  this field points to then next timer owned by thread.
    */
   struct ZD_I_Timer_Tag* next_thread_timer;

   /** Variable use_param is true if ZD_Start_Timer_Ex function was called.
    */
   bool use_param;

   /** Value of param variable will be put to the message from timer
    *  if use_param value is true.
    */
   uintptr_t param;

   struct ZD_I_Timer_Tag* next_timer_by_time;
   struct ZD_I_Timer_Tag* prev_timer_by_time;

   struct timespec expiration_time;
   uint32_t period;
} ZD_I_Timer_T;

typedef struct timespec ZD_Time_Spec_T;

/** Function saves current time (seconds and milliseconds)
 */
void ZD_Get_Time(ZD_Time_Spec_T* time_spec);

/** Function increase time_spec by time_offset (in milliseconds).
 */
void ZD_Time_Add_Offset(ZD_Time_Spec_T* time_spec, int32_t time_offset);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_TIME_H */
