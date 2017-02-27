#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

void SAL_Destroy_Timer(SAL_Timer_Id_T timer_id)
{
   SAL_I_Timer_T* timer;
   SAL_Thread_Id_T tid = SAL_I_Get_Thread_Id();

   /** Do not destroy non-existent timer or timer created by another thread
    */
   SAL_PRE(timer_id >= 0);
   SAL_PRE((size_t)timer_id < SAL_I_Max_Number_Of_Timers);

   timer = &SAL_I_Timers[timer_id];

   SAL_PRE(timer->event_id  != -1);
   SAL_PRE(timer->thread_id == tid);

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      SAL_I_Unbind_Timer_From_Thread(timer);
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);

      SAL_I_Destroy_Timer(timer);
      if (SAL_Lock_Mutex(&SAL_I_Timers_Mutex))
      {
         timer->event_id = -1;
         timer->next_thread_timer = SAL_I_Timers_Free_List;
         SAL_I_Timers_Free_List = timer;
         (void)SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
      }
      else
      {
         SAL_PRE_FAILED();
      }
   }
   else
   {
      SAL_PRE_FAILED();
   }
}
