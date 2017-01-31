#ifndef ZD_DATA_TYPES_LINUX_H
#define ZD_DATA_TYPES_LINUX_H

#include <pthread.h>
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




#endif /* ZD_DATA_TYPES_LINUX_H */

