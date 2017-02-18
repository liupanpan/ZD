#ifndef ZD_DATA_TYPES_LINUX_H
#define ZD_DATA_TYPES_LINUX_H

#include <pthread.h>
#include <semaphore.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <limits.h>
#include <signal.h>

/**
 *  ZD Mutex type.
 */
typedef pthread_mutex_t ZD_Mutex_T;

typedef struct ZD_Mutex_Attr_Tag
{
   bool recursive;
}ZD_Mutex_Attr_T;

/**
 *  SAL Semaphore type.
 */
typedef sem_t ZD_Semaphore_T;

typedef struct ZD_Semaphore_Attr_Tag
{
   int32_t initial_value;
}ZD_Semaphore_Attr_T;

/**
 *  XSAL Conditional Variable
 */
typedef pthread_cond_t ZD_Cond_T;

typedef void* ZD_Cond_Attr_T;

/**
 *  XSAL Read_Write lock.
 */
typedef pthread_rwlock_t ZD_RWLock_T;

typedef void* ZD_RWLock_Attr_T;

/* -------------------------------------------------------------------------
 *
 *                           Thread management
 *
 * -----------------------------------------------------------------------*/
/**
 *  Thread attributes
 */
typedef struct ZD_Thread_Attr_Tag
{
   const char*     	name;
   int32_t 			id;
   int32_t  		priority;
   size_t          	stack_size;
} ZD_Thread_Attr_T;

/** Default priority for Thread
 */
#if !defined(ZD_DEFAULT_THREAD_PRIORITY)
#define ZD_DEFAULT_THREAD_PRIORITY  10
#endif /* ZD_DEFAULT_THREAD_PRIORITY */

/** Default thread stack size
 */
#if !defined(ZD_DEFAULT_THREAD_STACK_SIZE)
/* Use OS default value */
#define ZD_DEFAULT_THREAD_STACK_SIZE  0
#endif /* ZD_DEFAULT_THREAD_STACK_SIZE */

/**
 *  Thread Local Storage key
 */
typedef pthread_key_t ZD_TLS_Key_T;

/**
 *  Thread handle
 */
typedef pthread_t ZD_OS_TID_T;

#define ZD_UNKNOWN_PRIORITY (-1)



#endif /* ZD_DATA_TYPES_LINUX_H */

