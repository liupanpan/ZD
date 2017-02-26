#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_publish.h"

#include <sys/timeb.h>

bool SAL_Send(SAL_App_Id_T app_id,SAL_Thread_Id_T thread_id,SAL_Event_Id_T event_id,const void* data,size_t data_size)
{
   bool send_status = false;
   SAL_Message_T msg;
   struct timeb current_time;

   (void)ftime(&current_time);

   msg.event_id           = (SAL_Event_Id_T)((uint32_t)event_id & 0xffffu);
   msg.sender_thread_id   = SAL_I_Get_Thread_Id();
   msg.receiver_thread_id = thread_id;
   msg.timestamp_sec      = current_time.time;
   msg.timestamp_msec     = current_time.millitm;
   msg.data_size          = data_size;
   msg.data               = (void*)data;

   SAL_PRE(thread_id >= 0);
   SAL_PRE(msg.event_id >= 0);
   SAL_PRE((data != NULL) || (data_size == 0u));

   if ((app_id == 0) || (app_id == SAL_I_App_Id))
   {
      if (thread_id == 0)
      {
         SAL_I_Local_Publish(&msg);
         send_status = true;
      }
      else
      {
         bool is_urgent;
         SAL_Message_Dropped_Reason_T err;
         SAL_I_Thread_Attr_T* thread_attr;

         SAL_PRE((size_t)thread_id <= SAL_I_Max_Number_Of_Threads);

         is_urgent = false;

         thread_attr = &SAL_I_Thread_Table[thread_id];
         send_status = SAL_I_Post_Message(&thread_attr->message_queue, &msg, is_urgent, &err);
         if ((!send_status) && (SAL_I_Message_Dropped_Callback != NULL))
         {
            SAL_I_Message_Dropped_Callback(thread_id, err, &msg);
         }
      }
   }

   return send_status;
}
