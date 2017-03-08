#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"


void SAL_Signal_Ready(void)
{
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();
   SAL_PRE(thread_attr != NULL);

   if (!thread_attr->is_ready)
   {
      if (SAL_Signal_Semaphore(&thread_attr->thread_ready_sem))
      {
         thread_attr->is_ready = true;
      }
      else
      {
         SAL_POST_FAILED();
      }
   }
}
