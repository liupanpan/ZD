#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_time.h"
#include "xsal_i_receive.h"

/* ----------------------------
 *  Local function prototypes
 * ---------------------------- */

/*  Functions: SAL_Lock_Mutex and SAL_Wait_Semaphore are not defined in 
 *  XSAL_LIGHT external interface but are used internally by the XSAL Light.
 */
bool SAL_Lock_Mutex(SAL_Mutex_T* mutex);
bool SAL_Wait_Semaphore(SAL_Semaphore_T* sem);

static SAL_Message_T* SAL_I_Wait_For_New_Messages_Timeout(
   SAL_Message_Queue_T* q,
   const SAL_Event_Id_T event_list[],
   size_t number_of_events,
   uint32_t timeout_ms);

/* -----------------------------
 *  Local function definitions
 * ----------------------------- */

static SAL_Message_T* SAL_I_Wait_For_New_Messages_Timeout(
   SAL_Message_Queue_T* q,
   const SAL_Event_Id_T event_list[],
   size_t number_of_events,
   uint32_t timeout_ms)
{
   SAL_Int_T msg_cnt = 0;
   SAL_I_Time_Spec_T time1;
   SAL_Message_T* msg = NULL;
   bool wait_for_msg = true;

   SAL_I_Get_Time(&time1);

   do
   {
      while(SAL_Try_Wait_Semaphore(&q->message_available))
      {
         msg_cnt++;
      }

      /** Unlock the queue and wait for new messages.
       *  Queue is locked on entry.
       */
      (void)SAL_Unlock_Mutex(&q->queue_lock);

      if (SAL_Wait_Semaphore_Timeout(&q->message_available, timeout_ms))
      {
         SAL_Message_Queue_Node_T* n;

         (void)SAL_Lock_Mutex(&q->queue_lock);
         n = SAL_I_Search_Message_Queue(q, event_list, number_of_events);
         if (n != q->end_marker)
         {
            msg = &n->message;
            SAL_I_Remove_Node_From_Queue(n);
            wait_for_msg = false;
         }
         else
         {
            uint32_t diff;
            SAL_I_Time_Spec_T time2;

            SAL_I_Get_Time(&time2);
            msg_cnt++;

            diff = SAL_I_Time_Diff(&time1, &time2);
            if (diff < timeout_ms)
            {
               timeout_ms -= diff;
               time1 = time2;
            }
            else
            {
               /** Set msg to NULL and exit
                */
               wait_for_msg = false;
               msg = NULL;
            }
         }
      }
      else
      {
         (void)SAL_Lock_Mutex(&q->queue_lock);

         /** Set msg to NULL and exit
          */
         wait_for_msg = false;
         msg = NULL;
      }
   } while(wait_for_msg);

   /** Restore semaphore value
    */
   for(; msg_cnt > 0; msg_cnt--)
   {
      (void)SAL_Signal_Semaphore(&q->message_available);
   }
   return msg;
}


/* ------------------------------
 *  Public function definitions
 * ------------------------------ */


const SAL_Message_T* SAL_Receive_Only_Timeout(
   const SAL_Event_Id_T event_list[], 
   size_t number_of_events, 
   uint32_t timeout_ms)
{
   bool signal_sem;
   SAL_Message_T* msg;
   SAL_Message_Queue_T* q;
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   SAL_PRE((event_list != NULL) || (number_of_events == 0));
   SAL_PRE(calling_thread != NULL);

   q = &calling_thread->message_queue;

   /** Because there is no possibility to kill one thread by another, so
    *  operation of releasing "current_message" has not to be synchronized.
    *
    *  If "killing thread" functionality is possible, the "releasing memory"
    *  activity will have to be synchronized.
    */
   if (calling_thread->current_message != NULL)
   {
      SAL_I_Release_Message(calling_thread->current_message);
      calling_thread->current_message = NULL;
   }

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      SAL_Message_Queue_Node_T* n = SAL_I_Search_Message_Queue(q, event_list, number_of_events);
      if (n != q->end_marker)
      {
         msg = &n->message;
         SAL_I_Remove_Node_From_Queue(n);
         signal_sem = true;
      }
      else
      {
         msg = SAL_I_Wait_For_New_Messages_Timeout(q, event_list, number_of_events, timeout_ms);
         signal_sem = false;
      }
      calling_thread->current_message = msg;
      (void)SAL_Unlock_Mutex(&q->queue_lock);

      if (signal_sem)
      {
         (void)SAL_Wait_Semaphore(&q->message_available);
      }
   }
   else
   {
      msg = NULL;
      SAL_PRE_FAILED();
   }

   return msg;
}
