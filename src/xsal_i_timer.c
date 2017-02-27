#include "xsal.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"

#include <sys/timeb.h>

/** Pointer to the SAL Timer's table
 */
SAL_I_Timer_T* SAL_I_Timers;

/** Head of free timers list.
 */
SAL_I_Timer_T* SAL_I_Timers_Free_List;

/** Mutex to synchronize the SAL Timer's table access
 */
SAL_Mutex_T SAL_I_Timers_Mutex;


void SAL_I_Bind_Timer_To_Thread(SAL_I_Timer_T* timer)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[timer->thread_id];

   timer->prev_thread_timer = NULL;
   timer->next_thread_timer = thread_attr->thread_timers;
   if (thread_attr->thread_timers != NULL)
   {
      thread_attr->thread_timers->prev_thread_timer = timer;
   }
   thread_attr->thread_timers = timer;
}

void SAL_I_Unbind_Timer_From_Thread(SAL_I_Timer_T* timer)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[timer->thread_id];

   if (timer->next_thread_timer != NULL)
   {
      timer->next_thread_timer->prev_thread_timer = timer->prev_thread_timer;
   }

   if (timer->prev_thread_timer != NULL)
   {
      timer->prev_thread_timer->next_thread_timer = timer->next_thread_timer;
   }
   else
   {
      thread_attr->thread_timers = timer->next_thread_timer;
   }
}









