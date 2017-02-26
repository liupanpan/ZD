#ifndef XSAL_DATA_TYPES_H
#define XSAL_DATA_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdint.h> 

/** SAL_Int_T type is used internally as procesor native word.
 *  On N-bit platforms it should be defined as intN_t.
 */
typedef int32_t    SAL_Int_T;
typedef uint8_t    SAL_App_Id_T;
typedef SAL_Int_T  SAL_Thread_Id_T;
typedef SAL_Int_T  SAL_Sched_Policy_T;
typedef SAL_Int_T  SAL_Priority_T;
typedef SAL_Int_T  SAL_Event_Id_T;
typedef SAL_Int_T  SAL_Timer_Id_T;
typedef SAL_Int_T  SAL_Buffer_Pool_Id_T;
typedef SAL_Int_T  SAL_Port_Id_T;
typedef SAL_Int_T  SAL_Shared_Memory_Key_T;
typedef uint32_t   SAL_Clock_T;

#include "xsal_data_types_linux.h"


/* -------------------------------------------------------------------------
 *
 *                           Queue management
 *
 * -----------------------------------------------------------------------*/
/** Structure with the message header.
 */
typedef struct SAL_Message_Tag
{
   /** Send time (seconds).
    */
   time_t timestamp_sec;

   /** Size of data associated with the message.
    */
   size_t data_size;

   /** Pointer to message data.
    */
   void* data;

   /** Event ID.
    */
   SAL_Event_Id_T  event_id;

   /** Object ID (reserved for XSAL C++ wrapper).
    */
   int16_t object_id;

   /** ID of application sending the message.
    */
   SAL_App_Id_T sender_app_id;

   /** ID of thread sending the message.
    */
   SAL_Thread_Id_T  sender_thread_id;

   /** ID of the receiver thread.
    */
   SAL_Thread_Id_T  receiver_thread_id;

   /** Send time (milliseconds).
    */
   uint16_t timestamp_msec;
} SAL_Message_T;

/** Structure with the queue statistics data.
 */
typedef struct SAL_Stat_Queue_Tag
{
   /** Queue size (as passed to SAL_Create_Queue)
    */
   size_t queue_size;

   /** Message buffer size (as passed to SAL_Create_Queue)
    */
   size_t buffer_size;

   /** Current number of messages in the queue
    */
   size_t message_count;

   /** Peak number of enqueued messages (since queue creation)
    */
   size_t peak_message_count;

   /** Size of largest enqueued messages (since queue creation)
    */
   size_t peak_message_size;
} SAL_Stat_Queue_T;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_DATA_TYPES_H */

