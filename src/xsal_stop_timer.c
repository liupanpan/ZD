#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

void SAL_Stop_Timer(SAL_Timer_Id_T timer_id)
{
   SAL_I_Timer_T* timer;

   /** Do not stop non-existent timer or timer created by another thread
    */
   SAL_PRE(timer_id >= 0);
   SAL_PRE((size_t)timer_id < SAL_I_Max_Number_Of_Timers);
   
   timer = &SAL_I_Timers[timer_id];

   SAL_PRE(timer->event_id != -1);
   SAL_PRE(timer->thread_id == SAL_I_Get_Thread_Id());

   SAL_I_Stop_Timer(timer);
}


