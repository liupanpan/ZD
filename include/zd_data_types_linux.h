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



#endif /* ZD_DATA_TYPES_LINUX_H */

