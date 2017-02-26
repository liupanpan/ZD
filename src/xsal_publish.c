#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_publish.h"

#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

void SAL_Publish(SAL_Event_Id_T event_id, const void* data, size_t data_size)
{
   struct timeb current_time;
   SAL_Message_T msg;

   (void)ftime(&current_time);

   msg.event_id           = (SAL_Event_Id_T)((uint32_t)event_id & 0xffffu);
   msg.sender_thread_id   = SAL_I_Get_Thread_Id();
   msg.receiver_thread_id = 0;
   msg.timestamp_sec      = current_time.time;
   msg.timestamp_msec     = current_time.millitm;
   msg.data_size          = data_size;
   msg.data               = (void*)data;

   SAL_PRE((data != NULL) || (data_size == 0u));

   SAL_I_Local_Publish(&msg);
} 
