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

/** Thread Local Storage Key with SAL Thread ID.
 */
extern SAL_TLS_Key_T SAL_I_Thread_Id_Self;

/** Number of currently running SAL threads started by user.
 */
extern size_t SAL_I_Number_Of_Running_User_Threads;

/** Pointer to table with thread attributes
 */
extern SAL_I_Thread_Attr_T* SAL_I_Thread_Table;

/** Mutex to synchronize access to SAL_I_Thread_Table
 */
extern SAL_Mutex_T  SAL_I_Thread_Table_Mutex;

/** Function assigns value to key in the Thread Local Storage.
 */
bool SAL_I_TLS_Set_Specific(SAL_TLS_Key_T tls_key, const void* value);

/** Function returns value of the key from the Thread Local Storage.
 */
void* SAL_I_TLS_Get_Specific(SAL_TLS_Key_T tls_key);

/** Function sets priority for given thread.
 */
bool SAL_I_Set_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T priority);

/** Function frees thread's resources
 */
void SAL_I_Free_Thread_Resources(SAL_Thread_Id_T thread_id);

/** Returns SAL Thread Id.
 */
SAL_Thread_Id_T  SAL_I_Get_Thread_Id(void);

/** Returns pointer to the calling thread attributes
 */
SAL_I_Thread_Attr_T*  SAL_I_Get_Thread_Attr(void);

void SAL_I_Configure_RT_Sched_Policy(void);

pid_t SAL_I_Get_Linux_Tid(void);

/** Function initializes thread module. It is called by SAL_Init()
 */
bool SAL_I_Init_Thread_Module(void);




#endif /* XSAL_I_THREAD_H */

