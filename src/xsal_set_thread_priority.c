#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"


bool SAL_Set_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T priority)
{
   bool status;
   SAL_PRE(thread_id >= 0);
   SAL_PRE((size_t)thread_id <= SAL_I_Max_Number_Of_Threads);

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      status = SAL_I_Set_Thread_Priority(thread_id, priority);
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);
   }
   else
   {
      SAL_PRE_FAILED();
      status = false;
   }
   return status;
}
