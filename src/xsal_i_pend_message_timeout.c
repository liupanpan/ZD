#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_time.h"
#include "xsal_i_message_queue.h"

SAL_Message_T* SAL_I_Pend_Message_Timeout(
   SAL_Message_Queue_T* queue,
   uint32_t timeout_ms)
{
   SAL_I_Time_Spec_T time1;
   bool wait_for_msg = true;

   SAL_Message_Queue_Node_T* msg;
   SAL_Message_Queue_Node_T* end_marker;

   SAL_PRE(queue != NULL);
   SAL_PRE(queue->node_tab != NULL);

   end_marker = queue->end_marker;

   SAL_I_Get_Time(&time1);

   do
   {
      if (SAL_Wait_Semaphore_Timeout(&queue->message_available, timeout_ms))
      {
         if (SAL_Lock_Mutex(&queue->queue_lock))
         {
            msg = end_marker->next_node;
            if (msg != end_marker)
            {
               SAL_I_Remove_Node_From_Queue(msg);
               wait_for_msg = false;
            }
            else
            {
               uint32_t diff;
               SAL_I_Time_Spec_T time2;

               SAL_I_Get_Time(&time2);
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
            (void)SAL_Unlock_Mutex(&queue->queue_lock);
         }
         else
         {
            wait_for_msg = false;
            msg = NULL;

            SAL_PRE_FAILED();
         }
      }
      else
      {
         /** Set msg to NULL and exit
          */
         wait_for_msg = false;
         msg = NULL;
      }
   } while(wait_for_msg);

   return (SAL_Message_T*)(void*)msg;
}

