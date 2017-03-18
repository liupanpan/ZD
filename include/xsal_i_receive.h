#ifndef XSAL_I_RECEIVE_H
#define XSAL_I_RECEIVE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_i_message_queue.h"
#include "xsal_i_thread.h"

const SAL_Message_T* SAL_I_Receive_From_Timeout(
   SAL_I_Thread_Attr_T* queue_owner_thread,
   uint32_t timeout_ms);

SAL_Message_Queue_Node_T* SAL_I_Search_Message_Queue(
   const SAL_Message_Queue_T* q, 
   const SAL_Event_Id_T event_list[],
   size_t number_of_events);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_RECEIVE_H */

