#include <errno.h>
#include <string.h>

#include "zd.h"
#include "zd_assert.h"
#include "zd_time.h"

void ZD_Init_Semaphore_Attr(ZD_Semaphore_Attr_T* attr)
{
   	ZD_CON(attr != NULL);
   	attr->initial_value = 0;
}

bool ZD_Create_Semaphore(ZD_Semaphore_T* sem, const ZD_Semaphore_Attr_T* attr)
{
	int32_t status;
   	int32_t init_value = (attr == NULL) ? 0 : attr->initial_value;

   	ZD_CON(sem != NULL);

	status = sem_init(sem, 0, init_value);

   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_Destroy_Semaphore(ZD_Semaphore_T* sem)
{
	int32_t status;

   	ZD_CON(sem != NULL);

   	status = sem_destroy(sem);

   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_Wait_Semaphore(ZD_Semaphore_T* sem)
{
	int32_t status;

   	ZD_CON(sem != NULL);
  
   	status = sem_wait(sem);

   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_Wait_Semaphore_Timeout(ZD_Semaphore_T* sem, int32_t timeout_msec)
{
   	int32_t err;
   	int32_t status;
   	ZD_Time_Spec_T timeout;

   	ZD_CON(sem != NULL);

	ZD_Get_Time(&timeout);
   	ZD_Time_Add_Offset(&timeout, timeout_msec);

   	status = sem_timedwait(sem, &timeout);
   	err = errno;

   	ZD_CON((status == 0) || (err == ETIMEDOUT));

   	return (bool)(status == 0);
}

bool ZD_Try_Wait_Semaphore(ZD_Semaphore_T* sem)
{
	int32_t err;
   	int32_t status;

   	ZD_CON(sem != NULL);

   	status = sem_trywait(sem);
   	err = errno;

   	ZD_CON((status == 0) || (err == EAGAIN));
   
   	return (bool)(status == 0);
}

bool ZD_Signal_Semaphore(ZD_Semaphore_T* sem)
{
   	int32_t status;

   	ZD_CON(sem != NULL);

   	status = sem_post(sem);

   	ZD_CON(status == 0);
   
   	return (bool)(status == 0);
}





