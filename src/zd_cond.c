#include <string.h>
#include <errno.h>
#include "zd.h"
#include "zd_assert.h"
#include "zd_time.h"

void ZD_Init_Cond_Attr(ZD_Cond_Attr_T* attr)
{
   	ZD_CON(attr != NULL);

   	/* ZD_Cond_Attr_T structure doesn't contain any data */
}

bool ZD_Create_Cond(ZD_Cond_T* cond, const ZD_Cond_Attr_T* attr)
{
   	int32_t status;

   	ZD_CON(cond != NULL);

   	status = pthread_cond_init(cond, NULL);

   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_Destroy_Cond(ZD_Cond_T* cond)
{
   	int32_t status;

   	ZD_CON(cond != NULL);

   	status = pthread_cond_destroy(cond);

   	ZD_CON(status != EINVAL);
   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_Wait_Cond(ZD_Cond_T* cond, ZD_Mutex_T* mutex)
{
   	int32_t status;

   	ZD_CON(cond != NULL);
   	ZD_CON(mutex != NULL);
  
   	status = pthread_cond_wait(cond, mutex);

   	ZD_CON(status != EINVAL);
   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_Wait_Cond_Timeout(ZD_Cond_T* cond, ZD_Mutex_T* mutex, uint32_t timeout_msec)
{
   	int32_t status;
   	ZD_Time_Spec_T timeout;

   	ZD_CON(mutex != NULL);

   	ZD_Get_Time(&timeout);
   	ZD_Time_Add_Offset(&timeout, timeout_msec);
  
   	status = pthread_cond_timedwait(cond, mutex, &timeout);

   	ZD_CON(status != EINVAL);
   	ZD_CON((status == 0) || (status == ETIMEDOUT));

   	return (bool)(status == 0);
}

bool ZD_Signal_Cond(ZD_Cond_T* cond)
{
   	int32_t status;
 
   	ZD_CON(cond != NULL);
 
   	status = pthread_cond_signal(cond);

   	ZD_CON(status != EINVAL);
   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}


















