#ifndef ZD_DATA_TYPES_H
#define ZD_DATA_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdint.h> 
#include "zd_data_types_linux.h"

/* -------------------------------------------------------------------------
 *
 *                           Thread management
 *
 * -----------------------------------------------------------------------*/
/** Thread ID of the Router Thread
 */
#define ZD_ROUTER_THREAD_ID 0
#define ZD_UNKNOWN_THREAD_ID (-1)


/* -------------------------------------------------------------------------
 *
 *                           Queue management
 *
 * -----------------------------------------------------------------------*/

/** Structure with the message header.
 */
typedef struct ZD_Message_Tag
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
   int32_t  event_id;

   /** Object ID (reserved for XSAL C++ wrapper).
    */
   int16_t object_id;

   /** ID of application sending the message.
    */
   uint8_t sender_app_id;

   /** ID of thread sending the message.
    */
   int32_t sender_thread_id;

   /** ID of the receiver thread.
    */
   int32_t receiver_thread_id;

   /** Send time (milliseconds).
    */
   uint16_t timestamp_msec;

} ZD_Message_T;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
