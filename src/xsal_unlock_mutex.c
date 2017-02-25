#include "xsal.h"
#include "xsal_i_assert.h"

#include <errno.h>
#include <string.h>

bool SAL_Unlock_Mutex(SAL_Mutex_T* mutex)
{
   SAL_Int_T status;

   SAL_PRE(mutex != NULL);

   status = pthread_mutex_unlock(mutex);

   SAL_PRE(status != EINVAL);
   SAL_POST(status == EOK);
   
   return (bool)(status == EOK);
}
