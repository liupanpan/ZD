#include "zd.h"
#include "zd_assert.h"
#include "zd_thread.h"
#include "zd_queue.h"
#include <stdlib.h>
#include <string.h>

bool ZD_I_Create_Message_Queue(ZD_Message_Queue_T* queue, size_t queue_size, size_t buffer_size, void* (*alloc_buf)(size_t size), void (*free_buf)(void *buf))
{
   bool status = false;
   size_t node_size;

   ZD_CON(queue != NULL);
   ZD_CON(queue_size > 0u);
   /** Calculates size for single queue node.
    *  The node size is equal size of SAL_Message_Queue_Node_T plus 
    *  size of pre-allocated buffer.
    *
    *  Queue nodes are part of node_tab, so the size of the single 
    *  node has to be divided by the size of WORD (processor dependent).
    */
   node_size = sizeof(ZD_Message_Queue_Node_T) + buffer_size;
   node_size = (((node_size+sizeof(size_t))-1u)/sizeof(size_t))*sizeof(size_t);

   if (ZD_Lock_Mutex(&queue->queue_lock))
   {
      ZD_CON(queue->node_tab == NULL);

      if (ZD_Create_Semaphore(&queue->message_available, NULL))
      {
         size_t i;
         uint8_t* p_queue_node;

         /** Allocates table of queue nodes. 
          *  Queue_size is increased by 1 for Queue End node.
          */
         queue->node_tab = (ZD_Message_Queue_Node_T*)calloc(queue_size+1u, node_size);
         if (queue->node_tab != NULL)
         {
            p_queue_node = (uint8_t*)(void*)queue->node_tab;

            /** Initialize free node list
             */
            queue->free_head = queue->node_tab;

            /** All but one nodes belong to the free node list (unidirectional list)
             *  Initialize queue and next_node fields of each node structure
             */
            for(i=0; i < queue_size; i++)
            {
               ((ZD_Message_Queue_Node_T*)(void*)p_queue_node)->queue = queue;
               ((ZD_Message_Queue_Node_T*)(void*)p_queue_node)->next_node = 
                  (ZD_Message_Queue_Node_T*)(void*)(&p_queue_node[node_size]);
               p_queue_node = &p_queue_node[node_size];
            }

            /** During last loop iteration the last node was added to the Free Nodes list.
             *  In fact the last node is not a part of free nodes list - it is used 
             *  as end list marker. 
             *  The next instruction unlinks end_marker from free nodes list.
             */
            ((ZD_Message_Queue_Node_T*)(void*)(&p_queue_node[-(int32_t)node_size]))->next_node = NULL;

            /** Initialize "Queue End" marker.
             *  At the begining, when queue is empty both pointers of end_marker:
             *  next and prev points to itself.
             */
            queue->end_marker = queue->urgent_tail = (ZD_Message_Queue_Node_T*)(void*)p_queue_node;
            queue->end_marker->queue = queue;
            queue->end_marker->next_node = queue->end_marker->prev_node = queue->end_marker;

            /** Store memory management functions
             */
            queue->alloc_buf = alloc_buf;
            queue->free_buf  = free_buf;

            /** Initialize queue statistic data
             */
            queue->queue_size  = queue_size;
            queue->buffer_size = buffer_size;
            queue->message_count = 
            queue->peak_message_count = 
            queue->peak_message_size = 0;
            status = true;
         }
         else
         {
            printf("ZD_I_Create_Message_Queue: Couldn't allocate memory for queue data");
            (void)ZD_Destroy_Semaphore(&queue->message_available);
         }
      }
      else
      {
         printf("ZD_I_Create_Message_Queue: Couldn't create semaphore");
      }
      (void)ZD_Unlock_Mutex(&queue->queue_lock);
   }
   else
   {
      ZD_FAILED();
   }
   return status;
}

bool ZD_Create_Queue(size_t queue_size, size_t message_size, void* (*alloc_buf)(size_t size), void (*free_buf)(void* buf))
{
   ZD_I_Thread_Attr_T* thread_attr = ZD_I_Get_Thread_Attr();

   ZD_CON(thread_attr != NULL);

   /** Queue should be able to store queue_size new messages when 
    *  thread is processing a current message. Therefore queue_size 
    *  parameter is increased by 1
    */
   return ZD_I_Create_Message_Queue(&thread_attr->message_queue, queue_size + 1u, message_size, alloc_buf, free_buf);
}
