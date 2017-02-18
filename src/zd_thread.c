#include <sys/syscall.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "zd.h"
#include "zd_assert.h"
#include "zd_thread.h"
#include "zd_config.h"

/** Thread Local Storage Key with ZD Thread ID.
 */
ZD_TLS_Key_T ZD_I_Thread_Id_Self;

/** Number of currently running ZD threads started by user.
 */
size_t  ZD_I_Number_Of_Running_User_Threads;


/** Pointer to table with thread attributes
 */
ZD_I_Thread_Attr_T* ZD_I_Thread_Table;

/** Mutex to synchronize access to SAL_I_Thread_Table
 */
ZD_Mutex_T ZD_I_Thread_Table_Mutex;

void ZD_Init_Thread_Attr(ZD_Thread_Attr_T* attr)
{
   ZD_CON(attr != NULL);

   attr->id         = ZD_UNKNOWN_THREAD_ID;
   attr->priority   = ZD_DEFAULT_THREAD_PRIORITY;
   attr->name       = NULL;
   attr->stack_size = ZD_DEFAULT_THREAD_STACK_SIZE;
}

void ZD_Wait_Ready(const int32_t thread_id_list[], size_t number_of_items)
{
   size_t i;

   for(i = 0; i < number_of_items; i++)
   {
      bool status;
      int32_t tid = thread_id_list[i];
   
      ZD_CON(tid >= 0);
      ZD_CON((size_t)tid <= ZD_I_Max_Number_Of_Threads);

      /** 'Decrease' and 'increase' semaphore.
       *  After this operation semaphore will always have 
       *  value 0 or 1 and never exceed the allowable value.
       */
      status = ZD_Wait_Semaphore(&ZD_I_Thread_Table[tid].thread_ready_sem);
      if (status)
      {
         status = ZD_Signal_Semaphore(&ZD_I_Thread_Table[tid].thread_ready_sem);
      }
      ZD_CON(status);
   }
}

pid_t ZD_I_Get_Linux_Tid(void)
{
   return (pid_t)syscall(SYS_gettid);
}

static int32_t ZD_I_Find_Free_Thread_Id(void)
{
   bool searching = true;
   int32_t thread_idx;
   int32_t tid = ZD_UNKNOWN_THREAD_ID; /* TODO: change to: ZD_INVALID_THREAD_ID; */

   for(thread_idx = ZD_First_Unknown_Thread_Id;
      searching && ((size_t)thread_idx <= ZD_I_Max_Number_Of_Threads);
      thread_idx++)
   {
      if (ZD_I_Thread_Table[thread_idx].thread_id == -1)
      {
         tid = thread_idx;
         searching = false;
      }
   }
   return tid;
}

bool ZD_I_Set_Thread_Priority(int32_t thread_id, int32_t priority)
{
   int32_t policy;
   struct sched_param sched_p;
   int32_t status = pthread_getschedparam(ZD_I_Thread_Table[thread_id].os_tid, &policy, &sched_p);
   if (status == 0)
   {
      if (policy == SCHED_OTHER)
      {
         status = setpriority(PRIO_PROCESS, ZD_I_Thread_Table[thread_id].linux_tid, priority);
         printf("setpriority(%d, %d) failed", ZD_I_Thread_Table[thread_id].linux_tid, priority);
      }
      else
      {
         sched_p.sched_priority = priority;
         status = pthread_setschedparam(ZD_I_Thread_Table[thread_id].os_tid, policy, &sched_p);
         printf("SAL_I_Set_Thread_Priority failed: setschedparam [tid=%d, sc=%d, prio=%d], %s",thread_id, policy, priority, strerror(status));
      }   
      sched_p.sched_priority = priority;
      status = pthread_setschedparam(ZD_I_Thread_Table[thread_id].os_tid, policy, &sched_p);
      printf("SAL_I_Set_Thread_Priority failed: setschedparam [tid=%d, sc=%d, prio=%d], %s", thread_id, policy, priority, strerror(status));
   }
   else
   {
      printf("SAL_I_Set_Thread_Priority failed: getschedparam [tid=%d, prio=%d], %s", thread_id, priority, strerror(errno));
   }
   return (bool)(status == 0);
}

bool ZD_I_TLS_Set_Specific(ZD_TLS_Key_T tls_key, const void* value)
{
   bool status = (bool)(pthread_setspecific(tls_key, value) == 0);
   printf("SAL_I_TLS_Set_Specific(%d, %p) error: %s", tls_key, value, strerror(status));

   return status;
}

static void Set_Priority_If_Other_Sched_Policy(pid_t tid, int32_t priority)
{
   int32_t policy;
   struct sched_param sched_p;
   int32_t status = pthread_getschedparam(pthread_self(), &policy, &sched_p);
   if (status == 0)
   {
      if (policy == SCHED_OTHER)
      {
         if (setpriority(PRIO_PROCESS, tid, priority) != 0)
         {
            printf("setpriority(%d, %d) failed", tid, priority);
         }
      }
   }
   else
   {
      printf("Set_Priority_If_Other_Sched_Policy/pthread_getschedparam failed: %d", status);
   }
}

static void* ZD_I_Start_User_Thread(ZD_I_Thread_Attr_T* thread_attr)
{
   static const int32_t wait_ready[] =
   {
      ZD_ROUTER_THREAD_ID
   };

   if (!ZD_I_TLS_Set_Specific(ZD_I_Thread_Id_Self, (const void*)thread_attr))
   {
      /** It is not allowed to print any debug messages
       *  if ZD_I_TLS_Set_Specific failed
       */
      ZD_FAILED();
      return NULL;
   }

   thread_attr->linux_tid = ZD_I_Get_Linux_Tid();
   Set_Priority_If_Other_Sched_Policy(thread_attr->linux_tid, thread_attr->initial_priority);

   ZD_CON(thread_attr->current_message == NULL);
   ZD_CON(thread_attr->selected_count  == 0);
   
   /** Wait until parent thread finishes its job.
    *  Mutex: ZD_I_Thread_Table_Mutex is locked by the parent thread,
    *  so the current thread will lock until its parent unlock it.
    */
   if (!ZD_Lock_Mutex(&ZD_I_Thread_Table_Mutex))
   {
      ZD_FAILED();
      return NULL;
   }
   if (!ZD_Unlock_Mutex(&ZD_I_Thread_Table_Mutex))
   {
      ZD_FAILED();
      return NULL;
   }

   /** Wait for Router Thread
    */
   ZD_Wait_Ready(wait_ready, sizeof(wait_ready)/sizeof(wait_ready[0]));

   /** Call user function
    */
   thread_attr->thread_fnc(thread_attr->thread_param);

   /** User thread treminate.
    *  Now free resources allocated by the thread:
    *  o) "reset" semaphore used in thread startup synchronization
    *  o) timers
    *  o) unsubscribe thread events
    *  o) message queue
    */
   //ZD_I_Free_Thread_Resources(thread_attr->thread_id); /* TODO change to: thread_attr */

   return NULL;
}

static int32_t Create_I_Thread(void (*fnc)(void*), void* param, const ZD_Thread_Attr_T* attr)
{
   pthread_attr_t pt_attr;
   int32_t t_id = (attr == NULL) ? ZD_UNKNOWN_THREAD_ID : attr->id;

   (void)pthread_attr_init(&pt_attr);
   (void)pthread_attr_setdetachstate(&pt_attr, PTHREAD_CREATE_DETACHED);
   (void)pthread_attr_setschedpolicy(&pt_attr, SCHED_RR);
   if ((attr != NULL) && (attr->stack_size > 0u))
   {
      (void)pthread_attr_setstacksize(&pt_attr, attr->stack_size);
   }
   if (ZD_Lock_Mutex(&ZD_I_Thread_Table_Mutex))
   {
      if (t_id == ZD_UNKNOWN_THREAD_ID)
      {
         t_id = ZD_I_Find_Free_Thread_Id();
      }
      if (t_id > 0)
      {
         int create_status;

         ZD_I_Thread_Attr_T* thread_attr = &ZD_I_Thread_Table[t_id];
         ZD_CON(thread_attr->thread_id == ZD_UNKNOWN_THREAD_ID);

         thread_attr->thread_id = t_id;
         thread_attr->thread_fnc = fnc;
         thread_attr->thread_param = param;
         thread_attr->initial_priority = (attr == NULL) ? ZD_DEFAULT_THREAD_PRIORITY : attr->priority;

         /** Copy the thread name given by user to thread table.
          *  If the thread name given by user is longer then
          *  ZD_THREAD_NAME_MAX_LENGTH-1, it will not be terminated by
          *  the function strncpy. In this case string is terminated
          *  by the last byte of the table which was set to 0 by the function
          *  calloc during creation of the thread table.
          */
         if ((attr == NULL) || (attr->name == NULL))
         {
            (void)snprintf(thread_attr->thread_name, ZD_THREAD_NAME_MAX_LENGTH-1, "SAL_%d", t_id);
         }
         else
         {
            (void)strncpy(thread_attr->thread_name, attr->name, ZD_THREAD_NAME_MAX_LENGTH-1);
         }

         /** "Lock" threads calling SAL_Wait_Destroyed();
          */
         (void)ZD_Try_Wait_Semaphore(&thread_attr->thread_destroyed_sem);
		 
         create_status = pthread_create(&thread_attr->os_tid, &pt_attr, (void* (*)(void*))ZD_I_Start_User_Thread, (void*)thread_attr);
         if (create_status == 0)
         {
            if (ZD_SCHED_POLICY != SCHED_OTHER)
            {
               (void)ZD_I_Set_Thread_Priority(t_id, ((attr == NULL) ? ZD_DEFAULT_THREAD_PRIORITY : attr->priority));
            }

            ZD_I_Number_Of_Running_User_Threads++;
         }
         else
         {
            printf("Thread_Create %d failed. Error: %d", t_id, create_status);
            thread_attr->thread_fnc = NULL;
            thread_attr->thread_id = ZD_UNKNOWN_THREAD_ID;
            t_id = ZD_UNKNOWN_THREAD_ID;
            /** "Unlock" threads calling ZD_Wait_Destroyed()
             */
            (void)ZD_Signal_Semaphore(&thread_attr->thread_destroyed_sem);
         }
      }
      (void)ZD_Unlock_Mutex(&ZD_I_Thread_Table_Mutex);
   }
   else
   {
      ZD_FAILED();
   }

   (void)pthread_attr_destroy(&pt_attr);

   return t_id;
}

int32_t ZD_Create_Thread(void (*thread_function)(void*), void* param, const ZD_Thread_Attr_T* attr)
{
   /** attr must be NULL or thread attr->id must be in valid range:
    *  (id > 0 and id <= ZD_I_Max_Number_Of_Threads) or
    *  (id == ZD_UNKNOWN_THREAD_ID)
    */
   ZD_CON((attr == NULL) || (attr->id > 0) || (attr->id == ZD_UNKNOWN_THREAD_ID));
   ZD_CON((attr == NULL) || (attr->id <= (int32_t)ZD_I_Max_Number_Of_Threads));

   return Create_I_Thread(thread_function, param, attr);
}













