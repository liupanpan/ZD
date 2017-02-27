#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

void  SAL_Start_Timer(SAL_Timer_Id_T timer_id,uint32_t interval_msec,bool is_periodic)
{
   SAL_I_Timer_T* timer;

   /** Do not start non-existent timer or timer created by another thread
    */
   SAL_PRE(timer_id >= 0);
   SAL_PRE((size_t)timer_id < SAL_I_Max_Number_Of_Timers);
   SAL_PRE((interval_msec != 0) || (!is_periodic));

   timer = &SAL_I_Timers[timer_id];

   SAL_PRE(timer->event_id != -1);
   SAL_PRE(timer->thread_id == SAL_I_Get_Thread_Id());

   if (interval_msec > 0)
   {
      SAL_I_Start_Timer(timer, interval_msec, is_periodic, false, (uintptr_t)NULL);
   }
   else
   {
      SAL_I_Stop_Timer(timer);
      SAL_I_Post_Timer_Event(timer->event_id, timer->thread_id, false, (uintptr_t)NULL);
   }
}


