#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"


bool SAL_Wait_Ready_Timeout(
   const SAL_Thread_Id_T thread_id_list[], 
   size_t number_of_items, 
   uint32_t timeout_msec)
{
   size_t i;
   bool status = true;

   for(i = 0; status && (i < number_of_items); i++)
   {
      SAL_Thread_Id_T tid = thread_id_list[i];

      SAL_PRE(tid >= 0);
      SAL_PRE((size_t)tid <= SAL_I_Max_Number_Of_Threads);

      if (SAL_Wait_Semaphore_Timeout(
         &SAL_I_Thread_Table[tid].thread_ready_sem, timeout_msec))
      {
         if (!SAL_Signal_Semaphore(
            &SAL_I_Thread_Table[tid].thread_ready_sem))
         {
            status = false;
         }
      }
      else
      {
         status = false;
      }
   }
   return status;
}
