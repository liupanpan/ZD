#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_thread.h"

bool SAL_Create_Timer(SAL_Event_Id_T event_id, SAL_Timer_Id_T* timer_id)
{
   bool status = false;

   SAL_PRE(timer_id != NULL);

   if (SAL_Lock_Mutex(&SAL_I_Timers_Mutex))
   {
      SAL_I_Timer_T* timer = SAL_I_Timers_Free_List;
      if (timer != NULL)
      {
         SAL_I_Timers_Free_List = timer->next_thread_timer;
      }
      (void)SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);

      if (timer != NULL)
      {
         SAL_PRE(timer->event_id == -1);

         timer->event_id  = event_id;
         timer->thread_id = SAL_I_Get_Thread_Id();
         if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
         {
            status = SAL_I_Create_Timer(timer);
            if (status)
            {
               SAL_I_Bind_Timer_To_Thread(timer);
            }
            (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);

            *timer_id = timer - SAL_I_Timers;
         }
         else
         {
            /* Returns timer to the free list */
            if (SAL_Lock_Mutex(&SAL_I_Timers_Mutex))
            {
               timer->next_thread_timer = SAL_I_Timers_Free_List;
               SAL_I_Timers_Free_List = timer;
               (void)SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
            }
            status = false;
         }
      }
      else
      {
         printf("Too many timers were created."
            "\nIncrease max number of timer is the SAL configuration (see SAL_Init)");
      }
   }
   else
   {
      SAL_PRE_FAILED();
   }
   return status;
}
