#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_message_queue.h"
#include <stdlib.h>
#include <string.h>

SAL_Message_Dropped_Callback_T SAL_I_Message_Dropped_Callback;

static void Insert_Node_To_List(SAL_Message_Queue_Node_T* pos, SAL_Message_Queue_Node_T* node);

static SAL_Message_Queue_Node_T* Get_Free_Node(SAL_Message_Queue_T* q,bool is_urgent);

static void Insert_Node_To_List(
   SAL_Message_Queue_Node_T* pos, 
   SAL_Message_Queue_Node_T* node)
{
   node->next_node = pos;
   node->prev_node = pos->prev_node;
   pos->prev_node->next_node = node;
   pos->prev_node = node;
}

void SAL_I_Remove_Node_From_List(SAL_Message_Queue_Node_T* node)
{
   node->prev_node->next_node = node->next_node;
   node->next_node->prev_node = node->prev_node;
}

void SAL_I_Free_Message_Data_Buffer(SAL_Message_Queue_Node_T* node)
{
   if ((node->message.data_size > node->queue->buffer_size) && (node->queue->free_buf != NULL))
   {
      node->queue->free_buf(node->message.data);
   }
   node->message.data = NULL;
}

void SAL_I_Add_Node_To_Free_List(SAL_Message_Queue_Node_T* node)
{
   node->next_node = node->queue->free_head;
   node->queue->free_head = node;
}

static SAL_Message_Queue_Node_T* Get_Free_Node(
   SAL_Message_Queue_T* q, 
   bool is_urgent)
{
   SAL_Message_Queue_Node_T* free_node = q->free_head;
   if (free_node != NULL)
   {
      q->free_head = free_node->next_node;
   }
   else
   {
      if (is_urgent)
      {
         if (q->end_marker->prev_node != q->urgent_tail)
         {
            printf("Queue is full and a new urgent message has arived. "
               "The last non-urgent message is removed from queue");

            SAL_I_Free_Message_Data_Buffer(q->end_marker->prev_node);
            free_node =  q->end_marker->prev_node;
            SAL_I_Remove_Node_From_List(q->end_marker->prev_node);

            q->message_count--;
            (void)SAL_Wait_Semaphore(&q->message_available);
         }
         else
         {
            printf("Queue is full of urgent message. The new urgent message is dropped.");
         }
      }
   }
   return free_node;
}

bool SAL_I_Init_Queue_Structure(SAL_Message_Queue_T* queue)
{
   SAL_PRE(queue != NULL);

   queue->node_tab = NULL;
   queue->free_head = NULL;
   return SAL_Create_Mutex(&queue->queue_lock, NULL);
}

void SAL_I_Deinit_Queue_Structure(SAL_Message_Queue_T* queue)
{
   SAL_PRE(queue != NULL);

   queue->node_tab = NULL;
   queue->free_head = NULL;
   (void)SAL_Destroy_Mutex(&queue->queue_lock);
}

SAL_Message_Queue_Node_T* SAL_I_Get_Free_Node(SAL_Message_Queue_T* queue, bool is_urgent, size_t data_size,SAL_Message_Dropped_Reason_T* err)
{
   SAL_Message_Queue_Node_T* n = Get_Free_Node(queue, is_urgent);
   if (n != NULL)
   {
      if (data_size > 0u)
      {
         if (data_size > n->queue->buffer_size)
         {
            if (n->queue->alloc_buf != NULL)
            {
               n->message.data = n->queue->alloc_buf(data_size);
               if (n->message.data == NULL)
               {
                  SAL_I_Add_Node_To_Free_List(n);
                  *err = SAL_No_Memory;
                  n = NULL;
               }
            }
            else
            {
               SAL_I_Add_Node_To_Free_List(n);
               *err = SAL_Too_Small_Buffer;
               n = NULL;
            }
         }
         else
         {
            n->message.data = &n[1];
         }
      }
      else
      {
         n->message.data = NULL;
      }
   }
   else
   {
      *err = (queue->node_tab == NULL) ? SAL_No_Queue : SAL_Queue_Full;
   }
   return n;
}

bool SAL_I_Create_Message_Queue(SAL_Message_Queue_T* queue, size_t queue_size,size_t buffer_size,void* (*alloc_buf)(size_t size),void  (*free_buf)(void *buf))
{
   bool status = false;
   size_t node_size;

   SAL_PRE(queue != NULL);
   SAL_PRE(queue_size > 0u);
   
   node_size = sizeof(SAL_Message_Queue_Node_T) + buffer_size;
   node_size = (((node_size+sizeof(size_t))-1u)/sizeof(size_t))*sizeof(size_t);

   if (SAL_Lock_Mutex(&queue->queue_lock))
   {
      SAL_PRE(queue->node_tab == NULL);

      if (SAL_Create_Semaphore(&queue->message_available, NULL))
      {
         size_t i;
         uint8_t* p_queue_node;

         queue->node_tab = (SAL_Message_Queue_Node_T*)calloc(queue_size+1u, node_size);
         if (queue->node_tab != NULL)
         {
            p_queue_node = (uint8_t*)(void*)queue->node_tab;

            queue->free_head = queue->node_tab;
            for(i=0; i < queue_size; i++)
            {
               ((SAL_Message_Queue_Node_T*)(void*)p_queue_node)->queue = queue;
               ((SAL_Message_Queue_Node_T*)(void*)p_queue_node)->next_node = 
                  (SAL_Message_Queue_Node_T*)(void*)(&p_queue_node[node_size]);
               p_queue_node = &p_queue_node[node_size];
            }

            ((SAL_Message_Queue_Node_T*)(void*)(&p_queue_node[-(SAL_Int_T)node_size]))->next_node = NULL;

            queue->end_marker = queue->urgent_tail = (SAL_Message_Queue_Node_T*)(void*)p_queue_node;
            queue->end_marker->queue = queue;
            queue->end_marker->next_node = queue->end_marker->prev_node = queue->end_marker;
            queue->alloc_buf = alloc_buf;
            queue->free_buf  = free_buf;
            queue->queue_size  = queue_size;
            queue->buffer_size = buffer_size;
            queue->message_count = 
            queue->peak_message_count = 
            queue->peak_message_size = 0;
            status = true;
         }
         else
         {
            printf("SAL_I_Create_Message_Queue: Couldn't allocate memory for queue data");
            (void)SAL_Destroy_Semaphore(&queue->message_available);
         }
      }
      else
      {
         printf("SAL_I_Create_Message_Queue: Couldn't create semaphore");
      }
      (void)SAL_Unlock_Mutex(&queue->queue_lock);
   }
   else
   {
      SAL_PRE_FAILED();
   }
   return status;
}

void SAL_I_Destroy_Message_Queue(SAL_Message_Queue_T* queue)
{
   SAL_PRE(queue != NULL);

   if (SAL_Lock_Mutex(&queue->queue_lock))
   {
      if (queue->node_tab != NULL)
      {
         SAL_Message_Queue_Node_T* n = queue->end_marker->next_node;
         while(n != queue->end_marker)
         {
            SAL_I_Free_Message_Data_Buffer(n);
            n = n->next_node;
         }
        
         free(queue->node_tab);

         queue->node_tab = NULL;
         queue->free_head = NULL;
         (void)SAL_Destroy_Semaphore(&(queue->message_available));
      }
      (void)SAL_Unlock_Mutex(&queue->queue_lock);
   }
   else
   {
      SAL_PRE_FAILED();
   }
}

bool SAL_I_Post_Message(SAL_Message_Queue_T* queue, const SAL_Message_T* message, bool is_urgent, SAL_Message_Dropped_Reason_T* err)
{
   SAL_Message_Queue_Node_T* new_node = NULL;

   SAL_PRE(queue != NULL);
   SAL_PRE(message != NULL);
   SAL_PRE(err != NULL);

   if (SAL_Lock_Mutex(&queue->queue_lock))
   {
      new_node = SAL_I_Get_Free_Node(queue, is_urgent, message->data_size, err);
      if (new_node != NULL)
      {
         void* p_data = new_node->message.data;
         new_node->message = *message;
         new_node->message.data = p_data;

         if (message->data_size > 0u)
         {
            (void)memcpy(new_node->message.data, message->data, message->data_size);
            if (message->data_size > queue->peak_message_size)
            {
               queue->peak_message_size = message->data_size;
            }
         }
         queue->message_count++;
         if (queue->message_count > queue->peak_message_count)
         {
            queue->peak_message_count = queue->message_count;
         }

         if (is_urgent)
         {
            Insert_Node_To_List(queue->urgent_tail->next_node, new_node);
            queue->urgent_tail = new_node;
         }
         else
         {
            Insert_Node_To_List(queue->end_marker, new_node);
         }
      }
      
      (void)SAL_Unlock_Mutex(&queue->queue_lock);
   }
   else
   {
      SAL_PRE_FAILED();
   }

   if (new_node != NULL)
   {
      (void)SAL_Signal_Semaphore(&queue->message_available);
   }

   return (bool)(new_node != NULL);
}

void SAL_I_Release_Message(SAL_Message_T* message)
{
   SAL_Message_Queue_T* q;

   SAL_PRE(message != NULL);

   q = ((SAL_Message_Queue_Node_T*)(void*)message)->queue;

   SAL_PRE(q != NULL);

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      SAL_I_Free_Message_Data_Buffer((SAL_Message_Queue_Node_T*)(void*)message);
      SAL_I_Add_Node_To_Free_List((SAL_Message_Queue_Node_T*)(void*)message);

      (void)SAL_Unlock_Mutex(&q->queue_lock);
   }
   else
   {
      SAL_PRE_FAILED();
   }
}

void SAL_I_Remove_Node_From_Queue(SAL_Message_Queue_Node_T* node)
{
   SAL_Message_Queue_T* q = node->queue;
   SAL_I_Remove_Node_From_List(node);
   if (node == q->urgent_tail)
   {
      q->urgent_tail = node->prev_node;
   }
   q->message_count--;
}
