#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"

bool SAL_Create_Queue(size_t queue_size, size_t message_size, void* (*alloc_buf)(size_t size), void (*free_buf)(void* buf))
{
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();

   SAL_PRE(thread_attr != NULL);

   /** Queue should be able to store queue_size new messages when 
    *  thread is processing a current message. Therefore queue_size 
    *  parameter is increased by 1
    */
   return SAL_I_Create_Message_Queue(&thread_attr->message_queue, queue_size + 1u, message_size, alloc_buf, free_buf);
}  




