#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_time.h"

#include <errno.h>
#include <string.h>


bool SAL_Lock_Mutex_Timeout(SAL_Mutex_T* mutex, uint32_t timeout_msec)
{
   SAL_Int_T status;
   SAL_I_Time_Spec_T timeout;

   SAL_PRE(mutex != NULL);

   SAL_I_Get_Time(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   status = pthread_mutex_timedlock(mutex, &timeout);

   SAL_PRE(status != EINVAL);
   SAL_POST((status == EOK) || (status == ETIMEDOUT));

   return (bool)(status == EOK);
}
