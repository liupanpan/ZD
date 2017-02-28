#include <sys/syscall.h>
#include <sys/resource.h>

#include "xsal.h"
#include "xsal_i_thread.h"
#include "xsal_i_assert.h"
#include "xsal_i_engine.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"
#include "xsal_i_timer.h"
#include "xsal_i_publish.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/** Thread Local Storage Key with SAL Thread ID.
 */
SAL_TLS_Key_T SAL_I_Thread_Id_Self;

/** Number of currently running SAL threads started by user.
 */
size_t  SAL_I_Number_Of_Running_User_Threads;

/** Pointer to table with thread attributes
 */
SAL_I_Thread_Attr_T* SAL_I_Thread_Table;

/** Mutex to synchronize access to SAL_I_Thread_Table
 */
SAL_Mutex_T SAL_I_Thread_Table_Mutex;

pid_t SAL_I_Get_Linux_Tid(void)
{
   return (pid_t)syscall(SYS_gettid);
}

bool SAL_I_TLS_Set_Specific(SAL_TLS_Key_T tls_key, const void* value)
{
   bool status = (bool)(pthread_setspecific(tls_key, value) == 0);

   return status;
}

void* SAL_I_TLS_Get_Specific(SAL_TLS_Key_T tls_key)
{
   return pthread_getspecific(tls_key);
}

/** Returns pointer to calling thread data.
 */
SAL_I_Thread_Attr_T* SAL_I_Get_Thread_Attr(void)
{
   return (SAL_I_Thread_Attr_T*)SAL_I_TLS_Get_Specific(SAL_I_Thread_Id_Self);
}

/** Returns SAL Thread Id.
 */
SAL_Thread_Id_T SAL_I_Get_Thread_Id(void)
{
   SAL_I_Thread_Attr_T* thread_attr = (SAL_I_Thread_Attr_T*)SAL_I_TLS_Get_Specific(SAL_I_Thread_Id_Self);

   return (thread_attr != NULL) ? thread_attr->thread_id : SAL_ROUTER_THREAD_ID;
}

bool SAL_I_Set_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T priority)
{
   SAL_Int_T policy;
   struct sched_param sched_p;
   SAL_Int_T status = pthread_getschedparam(SAL_I_Thread_Table[thread_id].os_tid, &policy, &sched_p);
   if (status == EOK)
   {
      if (policy == SCHED_OTHER)
      {
         status = setpriority(PRIO_PROCESS, SAL_I_Thread_Table[thread_id].linux_tid, priority);
      }
      else
      {
         sched_p.sched_priority = priority;
         status = pthread_setschedparam(SAL_I_Thread_Table[thread_id].os_tid, policy, &sched_p);
      }   
   }
   return (bool)(status == EOK);
}

/** Thread terminates itself
 */
void SAL_I_Free_Thread_Resources(SAL_Thread_Id_T thread_id)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[thread_id];

   SAL_I_Destroy_And_Unbind_Timers_From_Thread(thread_attr);

   if (SAL_Lock_Mutex(&SAL_I_Event_Property_Mutex))
   {
      SAL_I_Unsubscribe_Thread_Events(thread_id);
      (void)SAL_Unlock_Mutex(&SAL_I_Event_Property_Mutex);
   }
   else
   {
      SAL_PRE_FAILED();
   }

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      if (thread_attr->is_ready)
      {
         (void)SAL_Wait_Semaphore(&thread_attr->thread_ready_sem);
         thread_attr->is_ready = false;
      }

      thread_attr->thread_id = SAL_UNKNOWN_THREAD_ID;

      if (thread_attr->current_message != NULL)
      {
         SAL_I_Release_Message(thread_attr->current_message);
         thread_attr->current_message = NULL;
      }
      SAL_I_Destroy_Message_Queue(&thread_attr->message_queue);
      thread_attr->selected_count = 0;

      SAL_I_Number_Of_Running_User_Threads--;
      if (SAL_I_Number_Of_Running_User_Threads == 0u)
      {
         SAL_I_Stop_Router_Thread(0);
      }

      thread_attr->thread_fnc = NULL;

      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);
   }

   (void)SAL_Signal_Semaphore(&SAL_I_Thread_Table[thread_id].thread_destroyed_sem);
}







