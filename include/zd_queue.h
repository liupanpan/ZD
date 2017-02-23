#ifndef ZD_QUEUE_H
#define ZD_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "zd_data_types_linux.h"

/** Definition of the queue node (ZD_Message_T data and 
 *  internal node structures)
 */
typedef struct ZD_Message_Queue_Node_Tag
{
   /** ZD Message data
    *
    *  @note This field must be a first member of the structure!
    */
   ZD_Message_T message;

   /** Pointer to the queue which message belongs to
    */
   struct ZD_Message_Queue_Tag* queue;

   /** Next queue node
    */
   struct ZD_Message_Queue_Node_Tag* next_node;
 
   /** Prev queue node
    */
   struct ZD_Message_Queue_Node_Tag* prev_node;

} ZD_Message_Queue_Node_T;

/** Definition of the message queue structure
 */
typedef struct ZD_Message_Queue_Tag
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
   ZD_Message_Queue_Node_T* node_tab;

   /** Pointer to the last urgent message
    */
   ZD_Message_Queue_Node_T* urgent_tail;

   /** Queue end marker
    */
   ZD_Message_Queue_Node_T* end_marker;
   
   /** First free node in the queue
    */
   ZD_Message_Queue_Node_T* free_head;
   
   /** Mutex for synchronization access to the queue internal data
    */
   ZD_Mutex_T queue_lock;

   /** Semafore blocks threads until new message arrives
    */
   ZD_Semaphore_T message_available;

   /** Handler to the memory allocator
    */
   void *(*alloc_buf)(size_t size);

   /** Handler to the memory de-allocator
    */
   void (*free_buf)(void *buf);
} ZD_Message_Queue_T;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
