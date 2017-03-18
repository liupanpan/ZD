#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_time.h"

#include <errno.h>
#include <string.h>

bool SAL_Wait_Semaphore_Timeout(
   SAL_Semaphore_T* sem, 
   uint32_t timeout_msec)
{
   SAL_Int_T err;
   SAL_Int_T status;
   SAL_I_Time_Spec_T timeout;

   SAL_PRE(sem != NULL);

   SAL_I_Get_Time(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   status = sem_timedwait(sem, &timeout);
   err = errno;

   SAL_POST((status == 0) || (err == ETIMEDOUT));

   return (bool)(status == 0);
}
