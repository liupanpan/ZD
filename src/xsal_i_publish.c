#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_publish.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_event_property.h"

#include <stdlib.h>
#include <sys/timeb.h>

/** Function is called when given thread has finished its job 
 *  or has been terminated.
 *  It unsubscribes from LRT events subscribed by given thread.
 *  If any event was subscribed only by the given thread (it isn't
 *  subscribed by any other thread), then it is also unsubscribed 
 *  from Naming Service.
 */
void SAL_I_Unsubscribe_Thread_Events(SAL_Thread_Id_T thread_id)
{
   SAL_Event_Id_T ev_id;
   uint8_t* event_properties = SAL_I_Thread_Event_Property(thread_id);

   /*for(ev_id = EV_FIRST_EVENT_ID; ev_id < EV_LAST_EVENT_ID; ev_id++)
   {
      if (event_properties[ev_id] & SAL_I_Subscribe_Bit_Mask)
      {
         event_properties[ev_id] &= ~SAL_I_Subscribe_Bit_Mask;
      }
   }*/
}

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


