#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_message_queue.h"
#include "xsal_i_thread.h"

bool SAL_Stat_Queue(SAL_Thread_Id_T thread_id, SAL_Stat_Queue_T* queue_stat)
{
   bool status = false;
   SAL_Message_Queue_T* q;

   SAL_PRE(thread_id > 0);
   SAL_PRE((size_t)thread_id <= SAL_I_Max_Number_Of_Threads);
   SAL_PRE(queue_stat != NULL);

   q = &SAL_I_Thread_Table[thread_id].message_queue;

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      if (q->node_tab != NULL)
      {
         /** Queue size was increased internally during queue creation.
          */
         queue_stat->queue_size         = q->queue_size - 1u;

         queue_stat->buffer_size        = q->buffer_size;
         queue_stat->message_count      = q->message_count;
         queue_stat->peak_message_count = q->peak_message_count;
         queue_stat->peak_message_size  = q->peak_message_size;

         status = true;
      }
      (void)SAL_Unlock_Mutex(&q->queue_lock);
   }
   else
   {
      SAL_PRE_FAILED();
   }

   return status;
}
