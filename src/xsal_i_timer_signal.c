#include "xsal_settings.h"

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"

#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/resource.h>

/** Semaphore to synchronize startup and shutdown of the Timer Thread
 */
static SAL_Semaphore_T Timer_Sem;

/** Flag if if Timer Thread is running
 */
static bool Is_Timer_Thread_Running;

/** Handler to the Timer Thread
 */
static SAL_OS_TID_T Timer_Thread_Id;

static pid_t Timer_Thread_Id_Linux;

/** Handler to the OS timer
 */
static timer_t OS_Timer_Id;

/** List of runnig timers
 */
static SAL_I_Timer_T* Running_Timers_List;

/** Mutex to synchronize the Running_Timers_List table access
 */
static SAL_Mutex_T Running_Timers_List_Mutex;

static int32_t SAL_I_Exit_Value;

static void Update_Expiration_Time(struct timespec* t1, uint32_t t2)
{
   t1->tv_sec += t2/1000;
   t1->tv_nsec += (t2%1000)*1000000;
   if (t1->tv_nsec >= 1000000000)
   {
      t1->tv_nsec -= 1000000000;
      t1->tv_sec++;
   }   
}

static SAL_I_Timer_T* Find_Timer_Position_In_The_List(SAL_I_Timer_T* timer)
{
   /* PRE: Running_Timers_List cannot be empty */
   SAL_I_Timer_T* position = Running_Timers_List;

   do
   {
      if ((timer->expiration_time.tv_sec < position->expiration_time.tv_sec) ||
          ((timer->expiration_time.tv_sec == position->expiration_time.tv_sec) &&
          (timer->expiration_time.tv_nsec < position->expiration_time.tv_nsec)))
      {
         break;
      }
      position = position->next_timer_by_time;
      if (position == Running_Timers_List)
      {
         position = NULL;
      }
   } while(position != NULL);

   return position;
}

static bool Remove_Timer_From_Running_Timer_List(SAL_I_Timer_T* timer)
{
   SAL_I_Timer_T* prev_head = Running_Timers_List;

   if (timer == Running_Timers_List)
   {
      if (Running_Timers_List->next_timer_by_time == Running_Timers_List)
      {
         Running_Timers_List = NULL;
      }
      else
      {
         SAL_I_Timer_T* new_head = Running_Timers_List->next_timer_by_time;
         new_head->prev_timer_by_time = Running_Timers_List->prev_timer_by_time;
         new_head->prev_timer_by_time->next_timer_by_time = new_head;
         Running_Timers_List = new_head;
      }
   }
   else
   {
      timer->next_timer_by_time->prev_timer_by_time = timer->prev_timer_by_time;
      timer->prev_timer_by_time->next_timer_by_time = timer->next_timer_by_time;
   }

   timer->next_timer_by_time = NULL;

   return prev_head != Running_Timers_List;
}

static bool Add_Timer_To_Running_Timers_List(SAL_I_Timer_T* timer)
{
   bool new_head = false;

   if (timer->next_timer_by_time != NULL)
   {
      (void)Remove_Timer_From_Running_Timer_List(timer);
   }

   if (Running_Timers_List == NULL)
   {
      timer->next_timer_by_time = timer;
      timer->prev_timer_by_time = timer;
      Running_Timers_List = timer;
      new_head = true;
   }
   else
   {
      SAL_I_Timer_T* position = Find_Timer_Position_In_The_List(timer);
      if (position == NULL)
      {
         timer->next_timer_by_time = Running_Timers_List;
         timer->prev_timer_by_time = Running_Timers_List->prev_timer_by_time;
         timer->prev_timer_by_time->next_timer_by_time = timer;
         Running_Timers_List->prev_timer_by_time = timer;
      }
      else
      {
         timer->next_timer_by_time = position;
         timer->prev_timer_by_time = position->prev_timer_by_time;
         position->prev_timer_by_time = timer;
         timer->prev_timer_by_time->next_timer_by_time = timer;
         if (position == Running_Timers_List)
         {
            Running_Timers_List = timer;
            new_head = true;
         }
      }      
   }
   return new_head;
}

static void Restart_Timer(void)
{
   struct itimerspec itimer_spec;

   itimer_spec.it_value = Running_Timers_List->expiration_time;
   itimer_spec.it_interval.tv_sec  = 0;
   itimer_spec.it_interval.tv_nsec = 0;

   if (timer_settime(OS_Timer_Id, TIMER_ABSTIME, &itimer_spec, NULL) != 0)
   {
      printf("Timer_Thread/Restart_Timer: timer_settime failed. Error=%s", strerror(errno));
   }
}

static void Remove_First_Timer_From_Running_Timer_List(void)
{
   SAL_I_Timer_T* new_head = Running_Timers_List->next_timer_by_time;

   Running_Timers_List->next_timer_by_time = NULL;

   if (new_head != Running_Timers_List)
   {
      new_head->prev_timer_by_time = Running_Timers_List->prev_timer_by_time;
      Running_Timers_List->prev_timer_by_time->next_timer_by_time = new_head;
   }
   else
   {
      new_head = NULL;
   }
   Running_Timers_List = new_head;
}

static void* Timer_Thread(void *arg)
{
   sigset_t sigset;

   arg = arg; /* get rid of unused variable warning */

   if (!SAL_I_TLS_Set_Specific(SAL_I_Thread_Id_Self, &SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID]))
   {
      SAL_PRE_FAILED();
   }

   (void)sigemptyset(&sigset);
   (void)sigaddset(&sigset, SAL_I_Timer_Signal_Id);
   sigprocmask(SIG_BLOCK, &sigset, NULL);
   Timer_Thread_Id_Linux = SAL_I_Get_Linux_Tid();

   Is_Timer_Thread_Running = true;
   (void)SAL_Signal_Semaphore(&Timer_Sem);

   while(Is_Timer_Thread_Running)
   {     
      int sig;
      int status = sigwait(&sigset, &sig);
      if ((status == 0) && (sig == SAL_I_Timer_Signal_Id))
      {
         if (SAL_Lock_Mutex(&Running_Timers_List_Mutex))
         {
            struct timespec now;
            SAL_I_Timer_T* timer = Running_Timers_List;

            (void)clock_gettime(SAL_I_Timer_Signal_Clock_Id, &now);
            while(timer != NULL)
            {
               if ((timer->expiration_time.tv_sec < now.tv_sec) ||
                   ((timer->expiration_time.tv_sec == now.tv_sec) && (timer->expiration_time.tv_nsec <= now.tv_nsec)))
               {
                  Remove_First_Timer_From_Running_Timer_List();
                  SAL_I_Post_Timer_Event(timer->event_id, timer->thread_id, timer->use_param, timer->param);

                  if (timer->period > 0)
                  {
                     Update_Expiration_Time(&timer->expiration_time, timer->period);
                     (void)Add_Timer_To_Running_Timers_List(timer);
                  }
                  timer = Running_Timers_List;
               }
               else
               {
                  timer = NULL;
               }
            }
            if (Running_Timers_List != NULL)
            {
               Restart_Timer();
            }
            (void)SAL_Unlock_Mutex(&Running_Timers_List_Mutex);
         }
      }
   }

   (void)SAL_Signal_Semaphore(&Timer_Sem);

   return NULL;
}

void SAL_I_Stop_RT_Light(int32_t status)
{
   Is_Timer_Thread_Running = false;
   (void)pthread_kill(Timer_Thread_Id, SAL_I_Timer_Signal_Id);
   (void)SAL_Wait_Semaphore(&Timer_Sem);
   SAL_I_Exit_Value = status;
}

bool SAL_I_Init_Timer_Module(void)
{
   bool status = false;

   SAL_PRE(SAL_I_Timers == NULL);

   if (SAL_I_Timers == NULL)
   {
      if (SAL_I_Max_Number_Of_Timers > 0u)
      {
         struct sigevent timer_ev_spec;
         
         timer_ev_spec.sigev_value.sival_int = 0;
         timer_ev_spec.sigev_signo = SAL_I_Timer_Signal_Id;
         timer_ev_spec.sigev_notify = SIGEV_SIGNAL;

         if (timer_create(SAL_I_Timer_Signal_Clock_Id, &timer_ev_spec, &OS_Timer_Id) == EOK)
         {
            if (SAL_Create_Mutex(&SAL_I_Timers_Mutex, NULL))
            {
               if (SAL_Create_Mutex(&Running_Timers_List_Mutex, NULL))
               {
                  if (SAL_Create_Semaphore(&Timer_Sem, NULL))
                  {
                     SAL_I_Timers = (SAL_I_Timer_T*)malloc(SAL_I_Max_Number_Of_Timers*sizeof(SAL_I_Timer_T));
                     if (SAL_I_Timers != NULL)
                     {
                        sigset_t sigset;

                        (void)sigemptyset(&sigset);
                        (void)sigaddset(&sigset, SAL_I_Timer_Signal_Id);
                        (void)sigprocmask(SIG_BLOCK, &sigset, NULL);
                        Running_Timers_List = NULL;
                        status = true;
                     }
                     else
                     {
                        printf("SAL_I_Init_Timer_Module: memory allocation failed");
                     }
                     if (!status)
                     {
                        (void)SAL_Destroy_Semaphore(&Timer_Sem);
                     }
                  }
                  if (!status)
                  {
                     (void)SAL_Destroy_Mutex(&Running_Timers_List_Mutex);
                  }
               }
               if (!status)
               {
                  (void)SAL_Destroy_Mutex(&SAL_I_Timers_Mutex);
               }
            }
            else
            {
               printf("SAL_I_Init_Timer_Module/SAL_Create_Mutex() failed. Error=%s", strerror(errno));
            }
            if (!status)
            {
               (void)timer_delete(OS_Timer_Id);
            }
         }
         else
         {
            printf("SAL_I_Init_Timer_Module: timer_create failed. Error=%s", strerror(errno));
         }
      }
      else
      {
         printf("SAL_I_Init_Timer_Module: max number of timers == 0");
         status = true; 
      }
   }
   else
   {
      printf("SAL_I_Init_Timer_Module: Timer Module initialized second time without deinitialization");
   }

   return status;
}

bool SAL_I_Create_Timer(SAL_I_Timer_T* timer)
{
   timer = timer; /* remove unused variable warning */
   return true;
}

void SAL_I_Start_Timer_Module(void)
{
   if (SAL_I_Timers != NULL)
   {
      size_t tm_id;

      SAL_I_Timers_Free_List = &SAL_I_Timers[0];
      for(tm_id = 0; tm_id < SAL_I_Max_Number_Of_Timers-1; tm_id++)
      {
         SAL_I_Timers[tm_id].event_id = -1;
         SAL_I_Timers[tm_id].next_thread_timer = &SAL_I_Timers[tm_id + 1];
         SAL_I_Timers[tm_id].next_timer_by_time = NULL;
      }
      SAL_I_Timers[SAL_I_Max_Number_Of_Timers-1].event_id = -1;
      SAL_I_Timers[SAL_I_Max_Number_Of_Timers-1].next_thread_timer = NULL;
      SAL_I_Timers[SAL_I_Max_Number_Of_Timers-1].next_timer_by_time = NULL;
   }
}

void SAL_I_Stop_Timer_Module(void)
{ 
}

int32_t SAL_I_RT_Light(void)
{
   Timer_Thread_Id = pthread_self();
   (void)SAL_Signal_Ready();
   Timer_Thread(NULL);
   return SAL_I_Exit_Value;
}

void SAL_I_Start_Timer(SAL_I_Timer_T* timer,uint32_t interval_msec, bool is_periodic,bool use_param, uintptr_t param)
{
   clock_gettime(SAL_I_Timer_Signal_Clock_Id, &timer->expiration_time);
   Update_Expiration_Time(&timer->expiration_time, interval_msec);
   timer->period = is_periodic ? interval_msec : 0;
   timer->use_param = use_param;
   timer->param = param;
   if (SAL_Lock_Mutex(&Running_Timers_List_Mutex))
   {
      if (Add_Timer_To_Running_Timers_List(timer))
      {
         Restart_Timer();
      }
      (void)SAL_Unlock_Mutex(&Running_Timers_List_Mutex);
   }
   else
   {
      SAL_PRE_FAILED();
   }
}

void SAL_I_Stop_Timer(SAL_I_Timer_T* timer)
{
   /** If the timer is on the running timer list then remove it from the list
    */
   if (timer->next_timer_by_time != NULL)
   {
      if (SAL_Lock_Mutex(&Running_Timers_List_Mutex))
      {
         if (Remove_Timer_From_Running_Timer_List(timer) && (Running_Timers_List != NULL))
         {
            Restart_Timer();
         }
         (void)SAL_Unlock_Mutex(&Running_Timers_List_Mutex);
      }
      else
      {
         SAL_PRE_FAILED();
      }
   }
}

void SAL_I_Destroy_Timer(SAL_I_Timer_T* timer)
{
   /** If the timer is on the running timer list then remove it from the list
    */
   if (timer->next_timer_by_time != NULL)
   {
      if (SAL_Lock_Mutex(&Running_Timers_List_Mutex))
      {
         if (Remove_Timer_From_Running_Timer_List(timer) && (Running_Timers_List != NULL))
         {
            Restart_Timer();
         }
         (void)SAL_Unlock_Mutex(&Running_Timers_List_Mutex);
      }
      else
      {
         SAL_PRE_FAILED();
      }
   }
}

