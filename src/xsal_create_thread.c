#include <sys/resource.h>

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

#include <stdio.h>

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef void*  SAL_I_Thread_Return_Value_T;

static SAL_Thread_Id_T SAL_I_Find_Free_Thread_Id(void)
{
   bool searching = true;
   SAL_Thread_Id_T thread_idx;
   SAL_Thread_Id_T tid = SAL_UNKNOWN_THREAD_ID; /* TODO: change to: SAL_INVALID_THREAD_ID; */

   for(
      thread_idx = SAL_First_Unknown_Thread_Id;
      searching && ((size_t)thread_idx <= SAL_I_Max_Number_Of_Threads);
      thread_idx++)
   {
      if (SAL_I_Thread_Table[thread_idx].thread_id == -1)
      {
         tid = thread_idx;
         searching = false;
      }
   }
   return tid;
}

static void Set_Priority_If_Other_Sched_Policy(pid_t tid,SAL_Priority_T priority)
{
   SAL_Int_T policy;
   struct sched_param sched_p;
   SAL_Int_T status = pthread_getschedparam(pthread_self(), &policy, &sched_p);
   if (status == EOK)
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

static SAL_I_Thread_Return_Value_T  SAL_I_Start_User_Thread(SAL_I_Thread_Attr_T* thread_attr)
{
   static const SAL_Thread_Id_T wait_ready[] =
   {
      SAL_ROUTER_THREAD_ID
   };

   if (!SAL_I_TLS_Set_Specific(SAL_I_Thread_Id_Self, (const void*)thread_attr))
   {
      SAL_PRE_FAILED();
      return NULL;
   }

   thread_attr->linux_tid = SAL_I_Get_Linux_Tid();
   Set_Priority_If_Other_Sched_Policy(thread_attr->linux_tid, thread_attr->initial_priority);

   SAL_PRE(thread_attr->current_message == NULL);
   SAL_PRE(thread_attr->selected_count  == 0);
   
   if (!SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      SAL_POST_FAILED();
      return NULL;
   }
   if (!SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      SAL_POST_FAILED();
      return NULL;
   }

   /** Wait for Router Thread
    */
   SAL_Wait_Ready(wait_ready, sizeof(wait_ready)/sizeof(wait_ready[0]));

   /** Call user function
    */
   thread_attr->thread_fnc(thread_attr->thread_param);

   SAL_I_Free_Thread_Resources(thread_attr->thread_id); /* TODO change to: thread_attr */

   return NULL;
}

static SAL_Thread_Id_T Create_I_Thread(void (*fnc)(void*),void* param,const SAL_Thread_Attr_T* attr)
{
   pthread_attr_t pt_attr;
   SAL_Thread_Id_T t_id = (attr == NULL) ? SAL_UNKNOWN_THREAD_ID : attr->id;

   (void)pthread_attr_init(&pt_attr);
   (void)pthread_attr_setdetachstate(&pt_attr, PTHREAD_CREATE_DETACHED);
   (void)pthread_attr_setschedpolicy(&pt_attr, XSAL_SCHED_POLICY);
   if ((attr != NULL) && (attr->stack_size > 0u))
   {
      (void)pthread_attr_setstacksize(&pt_attr, attr->stack_size);
   }
   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      if (t_id == SAL_UNKNOWN_THREAD_ID)
      {
         t_id = SAL_I_Find_Free_Thread_Id();
      }
      if (t_id > 0)
      {
         int create_status;

         SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[t_id];
         SAL_PRE(thread_attr->thread_id == SAL_UNKNOWN_THREAD_ID);

         thread_attr->thread_id = t_id;
         thread_attr->thread_fnc = fnc;
         thread_attr->thread_param = param;
         thread_attr->initial_priority = (attr == NULL) ? SAL_DEFAULT_THREAD_PRIORITY : attr->priority;

         if ((attr == NULL) || (attr->name == NULL))
         {
            (void)snprintf(thread_attr->thread_name, SAL_THREAD_NAME_MAX_LENGTH-1, "SAL_%d", t_id);
         }
         else
         {
            (void)strncpy(thread_attr->thread_name, attr->name, SAL_THREAD_NAME_MAX_LENGTH-1);
         }

         /** "Lock" threads calling SAL_Wait_Destroyed();
          */
         (void)SAL_Try_Wait_Semaphore(&thread_attr->thread_destroyed_sem);
		 
         create_status = pthread_create(
            &thread_attr->os_tid,
            &pt_attr, 
            (void* (*)(void*))SAL_I_Start_User_Thread,
            (void*)thread_attr);
         if (create_status == EOK)
         {
            if (XSAL_SCHED_POLICY != SCHED_OTHER)
            {
               (void)SAL_I_Set_Thread_Priority(t_id, ((attr == NULL) ? SAL_DEFAULT_THREAD_PRIORITY : attr->priority));
            }

            SAL_I_Number_Of_Running_User_Threads++;
         }
         else
         {
            printf("Thread_Create %d failed. Error: %d", t_id, create_status);
            thread_attr->thread_fnc = NULL;
            thread_attr->thread_id = SAL_UNKNOWN_THREAD_ID;
            t_id = SAL_UNKNOWN_THREAD_ID;

            /** "Unlock" threads calling SAL_Wait_Destroyed()
             */
            (void)SAL_Signal_Semaphore(&thread_attr->thread_destroyed_sem);
         }
      }
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);
   }
   else
   {
      SAL_POST_FAILED();
   }

   (void)pthread_attr_destroy(&pt_attr);

   return t_id;
}

SAL_Thread_Id_T  SAL_Create_Thread(void (*thread_function)(void*),void* param,const SAL_Thread_Attr_T* attr)
{
   SAL_PRE((attr == NULL) || (attr->id > 0) || (attr->id == SAL_UNKNOWN_THREAD_ID));
   SAL_PRE((attr == NULL) || (attr->id <= (SAL_Thread_Id_T)SAL_I_Max_Number_Of_Threads));

   return Create_I_Thread(thread_function, param, attr);
}

