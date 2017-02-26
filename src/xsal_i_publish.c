#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_publish.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_event_property.h"

#include <stdlib.h>
#include <sys/timeb.h>

/**
 *  Function sends given message to all subscribed threads.
 */
void SAL_I_Local_Publish(const SAL_Message_T* msg)
{
   SAL_Event_Id_T ev_id = msg->event_id;

   size_t tid;
   for(tid = 1; tid <= SAL_I_Max_Number_Of_Threads; tid++)
   {
      if (SAL_I_Is_Subscribed(tid, ev_id))
      {
         bool send_status;
         SAL_Message_Dropped_Reason_T err;
         SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[tid];

         send_status = SAL_I_Post_Message(&thread_attr->message_queue, msg, false, &err);

         if (!send_status && (SAL_I_Message_Dropped_Callback != NULL))
         {
            SAL_I_Message_Dropped_Callback((SAL_Thread_Id_T)tid, err, msg);
         }
      }
   }
}


