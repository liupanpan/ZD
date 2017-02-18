#ifndef ZD_THREAD_H
#define ZD_THREAD_H

#include "zd_data_types.h"

/** Maximum length of the thread name (including terminating string byte).
 */
#define ZD_THREAD_NAME_MAX_LENGTH (8+1)

struct ZD_I_Timer_Tag;

typedef void (* ZD_Thread_Fnc_T)(void*);

/** ZD thread attributes
 */
typedef struct SAL_I_Thread_Attr_Tag
{
   /** Main function of the thread
    */
   ZD_Thread_Fnc_T thread_fnc;

   /** Thread function parameter
    */
   void* thread_param;

   /** Thread name
    */
   char thread_name[ZD_THREAD_NAME_MAX_LENGTH];

   /** SAL thread id
    */
   int32_t thread_id;

   /** OS thread id
    */
   ZD_OS_TID_T os_tid;

   /** Linux Thread Id for SAL Thread (with type 'pid_t').
    *  This field is used to get/set thread's priority running with 
    *  OTHER sched policy.
    *  To set/get thread's priority running with RR or FIFO sched policy 
    *  the POSIX Thread ID ('os_tid' field) is used.
    */
   pid_t linux_tid;

   /** Thread's initial priority. 
    *  This filed is set only if new thread with OTHER sched policy is created 
    *  to set its initial priority.
    */
   int32_t initial_priority;

   /** Thread's queue
    */
   //SAL_Message_Queue_T message_queue;

   /** Current thread's message
    */
   ZD_Message_T* current_message;

   /** Semaphore to synchronize threads startup
    */
   ZD_Semaphore_T thread_ready_sem;

   /** Number of selected events
    */
   size_t selected_count;

   /** 'is_ready' means whether function SAL_Signal_Ready was called or not
    */
   bool is_ready;

   /** Semaphore to synchronize threads destroy
    */
   ZD_Semaphore_T thread_destroyed_sem;

   /** Pointer to the first timer owned by thread
    */
   struct ZD_I_Timer_Tag* thread_timers;

} ZD_I_Thread_Attr_T;


/** Function assigns value to key in the Thread Local Storage.
 */
bool ZD_I_TLS_Set_Specific(ZD_TLS_Key_T tls_key, const void* value);


pid_t SAL_I_Get_Linux_Tid(void);


#endif
