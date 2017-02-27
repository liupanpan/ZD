#ifndef XSAL_I_THREAD_H
#define XSAL_I_THREAD_H

#include "xsal_settings.h"
#include "xsal_i_message_queue.h"

struct SAL_I_Timer_Tag;

typedef void (* SAL_Thread_Fnc_T)(void*);

/** SAL thread attributes
 */
typedef struct SAL_I_Thread_Attr_Tag
{
   /** Main function of the thread
    */
   SAL_Thread_Fnc_T thread_fnc;

   /** Thread function parameter
    */
   void* thread_param;

   /** Thread name
    */
   char thread_name[SAL_THREAD_NAME_MAX_LENGTH];

   /** SAL thread id
    */
   SAL_Thread_Id_T thread_id;

   /** OS thread id
    */
   SAL_OS_TID_T os_tid;

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
   SAL_Priority_T initial_priority;

   /** Thread's queue
    */
   SAL_Message_Queue_T message_queue;

   /** Current thread's message
    */
   SAL_Message_T* current_message;

   /** Semaphore to synchronize threads startup
    */
   SAL_Semaphore_T thread_ready_sem;

   /** Number of selected events
    */
   size_t selected_count;

   /** 'is_ready' means whether function SAL_Signal_Ready was called or not
    */
   bool is_ready;

   /** Semaphore to synchronize threads destroy
    */
   SAL_Semaphore_T thread_destroyed_sem;

   /** Pointer to the first timer owned by thread
    */
   struct SAL_I_Timer_Tag* thread_timers;

} SAL_I_Thread_Attr_T;


/** Pointer to table with thread attributes
 */
extern SAL_I_Thread_Attr_T* SAL_I_Thread_Table;

/** Mutex to synchronize access to SAL_I_Thread_Table
 */
extern SAL_Mutex_T  SAL_I_Thread_Table_Mutex;

/** Function returns value of the key from the Thread Local Storage.
 */
void* SAL_I_TLS_Get_Specific(SAL_TLS_Key_T tls_key);

/** Returns SAL Thread Id.
 */
SAL_Thread_Id_T  SAL_I_Get_Thread_Id(void);

/** Returns pointer to the calling thread attributes
 */
SAL_I_Thread_Attr_T*  SAL_I_Get_Thread_Attr(void);


#endif /* XSAL_I_THREAD_H */

