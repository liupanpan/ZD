#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

void SAL_Wait_Ready(const SAL_Thread_Id_T thread_id_list[], size_t number_of_items)
{
   size_t i;

   for(i = 0; i < number_of_items; i++)
   {
      bool status;
      SAL_Thread_Id_T tid = thread_id_list[i];
   
      SAL_PRE(tid >= 0);
      SAL_PRE((size_t)tid <= SAL_I_Max_Number_Of_Threads);

      status = SAL_Wait_Semaphore(&SAL_I_Thread_Table[tid].thread_ready_sem);
      if (status)
      {
         status = SAL_Signal_Semaphore(&SAL_I_Thread_Table[tid].thread_ready_sem);
      }
      SAL_POST(status);
   }
}
