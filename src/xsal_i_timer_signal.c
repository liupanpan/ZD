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


/** Handler to the OS timer
 */
static timer_t OS_Timer_Id;

/** List of runnig timers
 */
static SAL_I_Timer_T* Running_Timers_List;

/** Mutex to synchronize the Running_Timers_List table access
 */
static SAL_Mutex_T Running_Timers_List_Mutex;

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

bool SAL_I_Create_Timer(SAL_I_Timer_T* timer)
{
   timer = timer; /* remove unused variable warning */
   return true;
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

