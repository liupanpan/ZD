#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/resource.h>


bool SAL_I_Get_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T* priority, SAL_Sched_Policy_T* sched_policy)
{
   SAL_Int_T policy;
   struct sched_param sched_p;

   SAL_Int_T status = pthread_getschedparam(SAL_I_Thread_Table[thread_id].os_tid, &policy, &sched_p);
   if (status == EOK)
   {
      *sched_policy = (SAL_Sched_Policy_T)policy;
      *priority = (policy == SCHED_OTHER) ? getpriority(PRIO_PROCESS, SAL_I_Thread_Table[thread_id].linux_tid) : sched_p.sched_priority;
   }
   else
   { 
      printf("SAL_I_Get_Thread_Priority failed/pthread_getschedparam [%d], %s", thread_id, strerror(status));
   }
   return (bool)(status == EOK);
}

bool SAL_Get_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T* priority)
{
   bool status;

   SAL_PRE(thread_id >= 0);
   SAL_PRE((size_t)thread_id <= SAL_I_Max_Number_Of_Threads);

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      if (SAL_I_Thread_Table[thread_id].thread_id != SAL_UNKNOWN_THREAD_ID)
      {
         SAL_Sched_Policy_T sched_policy;
         status = SAL_I_Get_Thread_Priority(thread_id, priority, &sched_policy);
      }
      else
      {
         status = false;
         printf("SAL_I_Get_Thread_Priority: thread %d is not running", thread_id);
      }
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);
   }
   else
   {
      status = false;
   }

   return status;
}
