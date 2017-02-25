#include "xsal.h"
#include "xsal_i_assert.h"

#include <errno.h>
#include <string.h>

bool SAL_Create_Mutex(
   SAL_Mutex_T* mutex,
   const SAL_Mutex_Attr_T* attr)
{
   SAL_Int_T status;

   SAL_PRE(mutex != NULL);

   if (attr == NULL)
   {
      /** Create mutex with default (system dependent) settings
       */
      status = pthread_mutex_init(mutex, NULL);
   }
   else
   {
      pthread_mutexattr_t ptmutex_attr;
      status = pthread_mutexattr_init(&ptmutex_attr);
      if (status == EOK)
      {
         if (attr->recursive)
         {
            status = pthread_mutexattr_settype(
               &ptmutex_attr, 
               PTHREAD_MUTEX_RECURSIVE_NP);
         }

         if (status == EOK)
         {
            status = pthread_mutex_init(mutex, &ptmutex_attr);
         }
         (void)pthread_mutexattr_destroy(&ptmutex_attr);
      }
   }

   SAL_POST(status == EOK);

   return (bool)(status == EOK);
}

