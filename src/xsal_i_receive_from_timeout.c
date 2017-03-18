#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_time.h"
#include "xsal_i_event_property.h"
#include "xsal_i_receive.h"

const SAL_Message_T* SAL_I_Receive_From_Timeout(
   SAL_I_Thread_Attr_T* queue_owner_thread,
   uint32_t timeout_ms)
{
   SAL_Message_T* msg;
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   SAL_PRE(queue_owner_thread != NULL);
   SAL_PRE(calling_thread != NULL);

   if (calling_thread->current_message != NULL)
   {
      SAL_I_Release_Message(calling_thread->current_message);
      calling_thread->current_message = NULL;
   }
   msg = SAL_I_Pend_Message_Timeout(&queue_owner_thread->message_queue, timeout_ms);

   calling_thread->current_message = msg;

   return msg;
}
