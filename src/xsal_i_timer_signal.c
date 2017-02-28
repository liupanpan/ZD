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

static bool Remove_Timer_From_Running_Timer_List(SAL_I_Timer_T* timer)
{
   SAL_I_Timer_T* prev_head = Running_Timers_List;

   if (timer == Running_Timers_List)
   {
      if (Running_Timers_List->next_timer_by_time == Running_Timers_List)
      {
         /** It was the only one timer on the list
          */
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

   /** Unattach timer form list
    */
   timer->next_timer_by_time = NULL;

   return prev_head != Running_Timers_List;
}

static bool Add_Timer_To_Running_Timers_List(SAL_I_Timer_T* timer)
{
   bool new_head = false;

   /** If the timer is on the running timer list then remove it from the list
    *  and it next insert on the new position.
    */
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

void SAL_I_Stop_RT_Light(int32_t status)
{
   Is_Timer_Thread_Running = false;
   (void)pthread_kill(Timer_Thread_Id, SAL_I_Timer_Signal_Id);
   (void)SAL_Wait_Semaphore(&Timer_Sem);
   SAL_I_Exit_Value = status;
}

bool SAL_I_Create_Timer(SAL_I_Timer_T* timer)
{
   timer = timer; /* remove unused variable warning */
   return true;
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

