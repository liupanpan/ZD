#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"
#include "xsal_i_engine.h"
#include "xsal_i_thread.h"
#include "xsal_i_timer.h"
#include "xsal_i_publish.h"
#include "xsal_i_event_property.h"

#include <stdlib.h>


bool SAL_Init(const SAL_Config_T* sal_config)
{
   bool status = true;

   SAL_PRE(SAL_I_App_Id == 0);
   SAL_PRE(sal_config != NULL);
   SAL_PRE(sal_config->app_id > 0);
   SAL_PRE(sal_config->max_number_of_threads > 0u);

   SAL_I_Max_Number_Of_Threads = sal_config->max_number_of_threads;
   SAL_I_Max_Number_Of_Timers = sal_config->max_number_of_timers;
   SAL_I_Max_Number_Of_Buffer_Pools = sal_config->max_number_of_buffer_pools;

   if (sal_config->first_unknown_thread_id == SAL_UNKNOWN_THREAD_ID)
   {
      SAL_First_Unknown_Thread_Id = (SAL_Thread_Id_T)sal_config->max_number_of_threads+1;
   }
   else
   {
      SAL_PRE(sal_config->first_unknown_thread_id > 0);
      SAL_PRE(sal_config->first_unknown_thread_id <= (SAL_Thread_Id_T)sal_config->max_number_of_threads);

      SAL_First_Unknown_Thread_Id = sal_config->first_unknown_thread_id;
   }
   status = status && SAL_I_Init_Thread_Module();
   status = status && SAL_I_Init_Buffer_Pools();
   status = status && SAL_I_Init_Timer_Module();
   status = status && SAL_I_Init_Event_Property_Module();
   if (status)
   {
      SAL_I_Configure_RT_Sched_Policy();
   }
   if (status)
   {
      SAL_I_App_Id = sal_config->app_id;
   }
   return status;
}
