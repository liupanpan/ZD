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

/** Function creates a message queue. 
 *
 *  Each queue has its own callback functions to allocate and free memory 
 *  for queue elements. The pre-allocated buffers, alloc_buf and free_buf 
 *  or function operating on buffer pools could be used or user can define
 *  their own functions.
 * 
 *  Queue pre-allocates queue_size memory buffers of size message_size and 
 *  places the received messages in the buffers.
 *  If a message arrives which size is greater than message_size then XSAL 
 *  uses alloc_buf to dynamically allocate a memory buffer (and free_buf 
 *  to release it).
 *
 *  If message_size equals zero then memory buffers are not pre-allocated 
 *  and alloc_buf and free_buf are always used instead.
 *
 *  @note Pre-allocated buffers are fast (memory allocation/de-allocation
 *        is not required) but more memory may be used because queue_size 
 *        buffers are always needed (even if the queue is not full).
 *
 *  @param [in] queue       queue to be created
 *  @param [in] queue_size  defines queue size
 *  @param [in] buffer_size defines size of message
 *  @param [in] alloc_buf   pointer to the function which allocates memory 
 *                          for queue element
 *  @param [in] free_buf    pointer to the function which frees unused queue 
 *                          memory
 *
 *  @return 0 on failure
 *
 *  @pre Function SAL_Init_Queue_Structure() must be called before.
 *
 *  @see SAL_Init_Queue_Structure(), SAL_Destroy_Message_Queue(), 
 *       SAL_I_Post_Message(), SAL_I_Pend_Message(), SAL_I_Release_Message()
 */
bool SAL_I_Create_Message_Queue(SAL_Message_Queue_T* queue, size_t queue_size,size_t buffer_size,void* (*alloc_buf)(size_t size),void (*free_buf)(void *buf));



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_MESSAGE_QUEUE_H */

