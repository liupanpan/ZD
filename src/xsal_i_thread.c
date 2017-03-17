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

#define SAL_I_Get_OS_TID()  pthread_self()

bool SAL_I_TLS_Key_Create(SAL_TLS_Key_T* tls_key)
{
   SAL_Int_T status = pthread_key_create(tls_key, NULL);
   return (bool)(status == 0);
}

pid_t SAL_I_Get_Linux_Tid(void)
{
   return (pid_t)syscall(SYS_gettid);
}

bool SAL_I_TLS_Set_Specific(SAL_TLS_Key_T tls_key, const void* value)
{
   bool status = (bool)(pthread_setspecific(tls_key, value) == 0);

   return status;
}


bool SAL_I_TLS_Key_Delete(SAL_TLS_Key_T tls_key)
{
   SAL_Int_T status = pthread_key_delete(tls_key);

   return (bool)(status == 0);
}


void* SAL_I_TLS_Get_Specific(SAL_TLS_Key_T tls_key)
{
   return pthread_getspecific(tls_key);
}

void SAL_I_Configure_RT_Sched_Policy(void)
{
   SAL_Int_T policy;
   struct sched_param sched_p;

   if (pthread_getschedparam(pthread_self(), &policy, &sched_p) == EOK)
   {
      if (policy != XSAL_SCHED_POLICY)
      {
         sched_p.sched_priority = SAL_DEFAULT_THREAD_PRIORITY;
    	int ret = pthread_setschedparam(pthread_self(), XSAL_SCHED_POLICY, &sched_p);
		if(ret < EOK) 
         {
			printf("can not set schedule parameters: %s\n", strerror(ret));
            printf("SAL_I_Configure_RT_Sched_Policy/pthread_setschedparam failed\n");
         }
      }
   }
   else
   {
      printf("SAL_I_Configure_RT_Sched_Policy/pthread_getschedparam failed");
   }
}

bool SAL_I_Init_Thread_Module(void)
{
   size_t thread_idx;
   static const char* Router_Thread_Name = "XSAL-RT";
   SAL_Semaphore_Attr_T sem_attr;

   if (!SAL_Create_Mutex(&SAL_I_Thread_Table_Mutex, NULL))
   {
      printf("SAL_I_Init_Threads_Module: SAL_Create_Mutex");
      return false;
   }

   /* Add 1 to the Max_Number_Of_Threads, because there is one "system"
    * thread - RouterThread.
    */
   SAL_I_Thread_Table = (SAL_I_Thread_Attr_T*)calloc(
      SAL_I_Max_Number_Of_Threads+1u,
      sizeof(SAL_I_Thread_Attr_T));
   if (SAL_I_Thread_Table == NULL)
   {
      printf("Cannot allocate memory for thread attributes table");
      return false;
   }

   SAL_Init_Semaphore_Attr(&sem_attr);
   sem_attr.initial_value = 1;

   for(
      thread_idx = 0;
      thread_idx <= SAL_I_Max_Number_Of_Threads;
      thread_idx++)
   {
      SAL_I_Thread_Table[thread_idx].thread_id = SAL_UNKNOWN_THREAD_ID;
      SAL_I_Thread_Table[thread_idx].is_ready = false;

      if (!SAL_Create_Semaphore(&SAL_I_Thread_Table[thread_idx].thread_ready_sem, 0) ||
         !SAL_Create_Semaphore(&SAL_I_Thread_Table[thread_idx].thread_destroyed_sem, &sem_attr))
      {
         printf("Cannot create semaphore for thread startup synchronization");
         return false;
      }

      if (thread_idx != (size_t)SAL_ROUTER_THREAD_ID)
      {
         if (!SAL_I_Init_Queue_Structure(
            &SAL_I_Thread_Table[thread_idx].message_queue))
         {
            printf("Cannot initialize queue structure");
            return false;
         }
      }
      SAL_I_Thread_Table[thread_idx].current_message = NULL;

      SAL_I_Thread_Table[thread_idx].thread_timers = NULL;
   }

   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].os_tid = SAL_I_Get_OS_TID();
   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].linux_tid = SAL_I_Get_Linux_Tid();

   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_id = SAL_ROUTER_THREAD_ID;

   (void)strncpy(
      SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_name,
      Router_Thread_Name,
      SAL_THREAD_NAME_MAX_LENGTH-1);

   if (!SAL_I_TLS_Key_Create(&SAL_I_Thread_Id_Self) ||
      !SAL_I_TLS_Set_Specific(SAL_I_Thread_Id_Self, SAL_I_Thread_Table))
   {
      printf("Init_Threads_Module: couldn't create or initialize TLS");
      return false;
   }

   return true;
}

void SAL_I_Deinit_Thread_Module(void)
{
   size_t thread_idx;

   (void)SAL_I_TLS_Key_Delete(SAL_I_Thread_Id_Self);

   for(
      thread_idx = 0;
      thread_idx <= SAL_I_Max_Number_Of_Threads;
      thread_idx++)
   {
      (void)SAL_Destroy_Semaphore(&SAL_I_Thread_Table[thread_idx].thread_ready_sem);
      (void)SAL_Destroy_Semaphore(&SAL_I_Thread_Table[thread_idx].thread_destroyed_sem);

      if (thread_idx != (size_t)SAL_ROUTER_THREAD_ID)
      {
         SAL_I_Deinit_Queue_Structure(&SAL_I_Thread_Table[thread_idx].message_queue);
      }
   }

   free(SAL_I_Thread_Table);
   (void)SAL_Destroy_Mutex(&SAL_I_Thread_Table_Mutex);
}

void SAL_I_Exit_Thread(void)
{
	pthread_exit(NULL);
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


