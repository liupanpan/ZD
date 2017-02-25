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




#if !defined(EOK)
#define EOK 0
#endif /* !EOK */

#endif /* XSAL_DATA_TYPES_LINUX_H */

