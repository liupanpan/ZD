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





#if !defined(EOK)
#define EOK 0
#endif /* !EOK */

#endif /* XSAL_DATA_TYPES_LINUX_H */

