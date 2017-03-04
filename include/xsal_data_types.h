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
 *                           Application management
 *
 * -----------------------------------------------------------------------*/
typedef struct SAL_Config_Tag
{
   /** Identifier of XSAL Application.
    * 
    *  Application IDs are unique for the whole system -- each XSAL 
    *  application has its own ID defined in a globally known file
    *  xsal_application_id.h.
    */
   SAL_App_Id_T app_id;

   /** The maximum number of threads the application can have.
    *
    *  @note max_number_of_threads may be greater than the 
    *        actual number of threads in the applications but it shouldn't be 
    *        too large because XSAL preallocates some resources for all 
    *        declared threads.
    */
   size_t max_number_of_threads;
   
   /** The maximum number of timers the application can have.
    *  If the application doesn't use any timers, then set it to 0.
    */
   size_t max_number_of_timers;

   /** The maximum number of buffer pools the application can have.
    *  If the application doesn't use any buffer pools, then set it to 0.
    */
   size_t max_number_of_buffer_pools;

   /** First dynamic thread ID. XSAL threads have numerical IDs. The IDs
    *  can be allocated by XSAL or defined by the user. All user-defined
    *  thread IDs must be greater than zero and less than
    *  first_unknown_thread_id. When XSAL allocates a thread ID, it starts
    *  at first_unknown_thread_id.
    */
   SAL_Thread_Id_T first_unknown_thread_id;
} SAL_Config_T;


/* -------------------------------------------------------------------------
 *
 *                           Thread management
 *
 * -----------------------------------------------------------------------*/
/** Thread ID of the Router Thread
 */
#define SAL_ROUTER_THREAD_ID 0
#define SAL_UNKNOWN_THREAD_ID (-1)
/** Command ID's for Router Thread
 */
enum SAL_RT_Event_T
{
   EV_RT_DT_Command   = -1,
   EV_RT_Last_Command = -2
};

/* -------------------------------------------------------------------------
 *
 *                           Queue management
 *
 * -----------------------------------------------------------------------*/
/* Forward declaration */

 struct SAL_Message_Queue_Node_Tag;

/** List of reasons why the message has been dropped.
 */
typedef enum SAL_Message_Dropped_Reason_Tag
{
   /** Destination queue doesn't exist.
    */
   SAL_No_Queue,

   /** Destination queue is full.
    */
   SAL_Queue_Full,

   /** Memory for the message couldn't be allocated.
    */
   SAL_No_Memory,

   /** Memory allocation function is not defined 
    *  and pre-allocated buffer is too small.
    */
   SAL_Too_Small_Buffer,

   /** There was a communication error during message transmission.
    */
   SAL_Communication_Error
} SAL_Message_Dropped_Reason_T;

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


typedef void (* SAL_Message_Dropped_Callback_T)(SAL_Thread_Id_T thread_id, SAL_Message_Dropped_Reason_T reason, const SAL_Message_T* message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_DATA_TYPES_H */

