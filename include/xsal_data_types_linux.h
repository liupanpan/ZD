#ifndef XSAL_DATA_TYPES_LINUX_H
#define XSAL_DATA_TYPES_LINUX_H

#include <pthread.h>
#include <semaphore.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <limits.h>
#include <signal.h>

/**
 *  SAL Mutex type.
 */
typedef pthread_mutex_t SAL_Mutex_T;

typedef struct SAL_Mutex_Attr_Tag
{
   bool recursive;
} SAL_Mutex_Attr_T;

#define SAL_DEFAULT_MUTEX_ATTR { false }

/**
 *  SAL Semaphore type.
 */
typedef sem_t SAL_Semaphore_T;

typedef struct SAL_Semaphore_Attr_Tag
{
   uint32_t initial_value;
} SAL_Semaphore_Attr_T;

#define SAL_DEFAULT_SEMAPHORE_ATTR { 0 }

/**
 *  Connection to SAL synchronous channel type.
 */
typedef struct SAL_Connection_Tag
{
   pid_t app_pid;
   int socket_d;
   uid_t user_id;
} SAL_Connection_T;

#define SAL_Is_Connected(connection)           ((bool)((connection)->app_pid > 0))
#define SAL_Invalidate_Connection(connection)  ((connection)->app_pid = 0)
/* -------------------------------------------------------------------------
 *
 *                           Thread management
 *
 * -----------------------------------------------------------------------*/
/**
 *  Thread attributes
 */
typedef struct SAL_Thread_Attr_Tag
{
   const char*     name;
   SAL_Thread_Id_T id;
   SAL_Priority_T  priority;
   size_t          stack_size;
} SAL_Thread_Attr_T;

/** Default priority for Thread
 */
#if !defined(SAL_DEFAULT_THREAD_PRIORITY)
#define SAL_DEFAULT_THREAD_PRIORITY  10
#endif /* SAL_DEFAULT_THREAD_PRIORITY */

/** Default thread stack size
 */
#if !defined(SAL_DEFAULT_THREAD_STACK_SIZE)
/* Use OS default value */
#define SAL_DEFAULT_THREAD_STACK_SIZE  0
#endif /* SAL_DEFAULT_THREAD_STACK_SIZE */

#define SAL_DEFAULT_THREAD_ATTR { "", SAL_UNKNOWN_THREAD_ID, SAL_DEFAULT_THREAD_PRIORITY, SAL_DEFAULT_THREAD_STACK_SIZE }
/**
 *  Thread Local Storage key
 */
typedef pthread_key_t SAL_TLS_Key_T;
/**
 *  Thread handle
 */
typedef pthread_t SAL_OS_TID_T;

#define SAL_UNKNOWN_PRIORITY (-1)






#if !defined(EOK)
#define EOK 0
#endif /* !EOK */

#endif /* XSAL_DATA_TYPES_LINUX_H */

