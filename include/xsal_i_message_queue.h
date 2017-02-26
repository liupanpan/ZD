#ifndef XSAL_MESSAGE_QUEUE_H
#define XSAL_MESSAGE_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"

/** Queue iterator attributes
 */
#define SAL_I_Queue_Begin           0x01
#define SAL_I_Queue_End             0x02
#define SAL_I_Queue_Begin_And_End   0x03

/** Definition of the queue node (SAL_Message_T data and 
 *  internal node structures)
 */
typedef struct SAL_Message_Queue_Node_Tag
{
   /** SAL Message data
    *
    *  @note This field must be a first member of the structure!
    */
   SAL_Message_T message;

   /** Pointer to the queue which message belongs to
    */
   struct SAL_Message_Queue_Tag* queue;

   /** Next queue node
    */
   struct SAL_Message_Queue_Node_Tag* next_node;
 
   /** Prev queue node
    */
   struct SAL_Message_Queue_Node_Tag* prev_node;

} SAL_Message_Queue_Node_T;

/** Definition of the message queue structure
 */
typedef struct SAL_Message_Queue_Tag
{
   /** Queue size
    */
   size_t queue_size;

   /** Msg buffer size (as passed to SAL_Create_Queue)
    */
   size_t buffer_size;

   /** Current number of messages in the queue
    */
   size_t message_count;

   /** Peak number of enqueued messages (since queue creation)
    */
   size_t peak_message_count;

   /** Size of largest enqueued message (since queue creation)
    */
   size_t peak_message_size;

   /** Pointer to internal queue structure
    */
   SAL_Message_Queue_Node_T* node_tab;

   /** Pointer to the last urgent message
    */
   SAL_Message_Queue_Node_T* urgent_tail;

   /** Queue end marker
    */
   SAL_Message_Queue_Node_T* end_marker;
   
   /** First free node in the queue
    */
   SAL_Message_Queue_Node_T* free_head;
   
   /** Mutex for synchronization access to the queue internal data
    */
   SAL_Mutex_T queue_lock;

   /** Semafore blocks threads until new message arrives
    */
   SAL_Semaphore_T message_available;

   /** Handler to the memory allocator
    */
   void *(*alloc_buf)(size_t size);

   /** Handler to the memory de-allocator
    */
   void (*free_buf)(void *buf);
} SAL_Message_Queue_T;

/** Pointer to the "Queue full" callback
 */
extern SAL_Message_Dropped_Callback_T SAL_I_Message_Dropped_Callback;

SAL_Message_Queue_Node_T* SAL_I_Get_Free_Node(SAL_Message_Queue_T* queue, bool is_urgent, size_t data_size,SAL_Message_Dropped_Reason_T* err);

void SAL_I_Remove_Node_From_List(SAL_Message_Queue_Node_T* node);

void SAL_I_Free_Message_Data_Buffer(SAL_Message_Queue_Node_T* node);

void SAL_I_Add_Node_To_Free_List(SAL_Message_Queue_Node_T* node);

void SAL_I_Free_Node(SAL_Message_Queue_Node_T* node);

void SAL_I_Post_Message_Node(SAL_Message_Queue_Node_T* node, bool is_urgent);

bool SAL_I_Init_Queue_Structure(SAL_Message_Queue_T* queue);

void SAL_I_Deinit_Queue_Structure(SAL_Message_Queue_T* queue);

bool SAL_I_Create_Message_Queue(SAL_Message_Queue_T* queue, size_t queue_size,size_t buffer_size,void* (*alloc_buf)(size_t size),void (*free_buf)(void *buf));

void SAL_I_Destroy_Message_Queue(SAL_Message_Queue_T* queue);

bool SAL_I_Post_Message(SAL_Message_Queue_T* queue, const SAL_Message_T* message, bool is_urgent,SAL_Message_Dropped_Reason_T* err);

SAL_Message_T* SAL_I_Pend_Message(SAL_Message_Queue_T* queue);

SAL_Message_T* SAL_I_Try_Pend_Message(SAL_Message_Queue_T* queue);

SAL_Message_T* SAL_I_Pend_Message_Timeout(SAL_Message_Queue_T* queue, uint32_t timeout_ms);

void SAL_I_Release_Message(SAL_Message_T* message);

void SAL_I_Remove_Node_From_Queue(SAL_Message_Queue_Node_T* node);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_MESSAGE_QUEUE_H */

