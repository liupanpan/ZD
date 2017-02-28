#include "xsal.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"

#include <sys/timeb.h>

/** Pointer to the SAL Timer's table
 */
SAL_I_Timer_T* SAL_I_Timers;

/** Head of free timers list.
 */
SAL_I_Timer_T* SAL_I_Timers_Free_List;

/** Mutex to synchronize the SAL Timer's table access
 */
SAL_Mutex_T SAL_I_Timers_Mutex;


void SAL_I_Bind_Timer_To_Thread(SAL_I_Timer_T* timer)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[timer->thread_id];

   timer->prev_thread_timer = NULL;
   timer->next_thread_timer = thread_attr->thread_timers;
   if (thread_attr->thread_timers != NULL)
   {
      thread_attr->thread_timers->prev_thread_timer = timer;
   }
   thread_attr->thread_timers = timer;
}

void SAL_I_Unbind_Timer_From_Thread(SAL_I_Timer_T* timer)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[timer->thread_id];

   if (timer->next_thread_timer != NULL)
   {
      timer->next_thread_timer->prev_thread_timer = timer->prev_thread_timer;
   }

   if (timer->prev_thread_timer != NULL)
   {
      timer->prev_thread_timer->next_thread_timer = timer->next_thread_timer;
   }
   else
   {
      thread_attr->thread_timers = timer->next_thread_timer;
   }
}

void SAL_I_Destroy_And_Unbind_Timers_From_Thread(SAL_I_Thread_Attr_T* thread_attr)
{
   SAL_I_Timer_T* timer = thread_attr->thread_timers;

   while(timer != NULL)
   {
      if (SAL_Lock_Mutex(&SAL_I_Timers_Mutex))
      {
         SAL_I_Timer_T* to_delete = timer;
         timer = timer->next_thread_timer;
         SAL_I_Destroy_Timer(to_delete);
         to_delete->event_id = -1;
         to_delete->next_thread_timer = SAL_I_Timers_Free_List;
         SAL_I_Timers_Free_List = to_delete;
         (void)SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
      }
   }
   thread_attr->thread_timers = NULL;
}

void SAL_I_Post_Timer_Event(SAL_Event_Id_T event_id, SAL_Thread_Id_T thread_id, bool use_param, uintptr_t param)
{
   bool send_status;
   SAL_Message_T msg;
   struct timeb current_time;
   SAL_Message_Dropped_Reason_T err;
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[thread_id];

   bool is_urgent = false;

   (void)ftime(&current_time);

   msg.event_id            = (SAL_Event_Id_T)(int16_t)event_id;
   msg.receiver_thread_id  = msg.sender_thread_id = thread_id;
   msg.timestamp_sec       = current_time.time;
   msg.timestamp_msec      = current_time.millitm;
   if (use_param)
   {
      msg.data_size = sizeof(uintptr_t);
      msg.data      = &param;
   }
   else
   {
      msg.data_size = 0;
      msg.data      = NULL;
   }

   send_status = SAL_I_Post_Message(&thread_attr->message_queue, &msg, is_urgent, &err);
   if ((!send_status) && (SAL_I_Message_Dropped_Callback != NULL))
   {
      SAL_I_Message_Dropped_Callback(thread_id, err, &msg);
   }
}








