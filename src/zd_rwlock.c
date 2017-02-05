#include <errno.h>
#include <string.h>
#include "zd.h"
#include "zd_assert.h"

void ZD_Init_RWLock_Attr(ZD_RWLock_Attr_T* attr)
{
   	ZD_CON(attr != NULL);
}

bool ZD_Create_RWLock(ZD_RWLock_T* rw_lock, const ZD_RWLock_Attr_T* attr)
{
   	int32_t status;

   	ZD_CON(rw_lock != NULL);

   	/* Current implementation doesn't use any RWLock attributes. */
   	status = pthread_rwlock_init(rw_lock, NULL);

   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_Destroy_RWLock(ZD_RWLock_T* rw_lock)
{
   	int32_t status;

   	ZD_CON(rw_lock != NULL);

   	status = pthread_rwlock_destroy(rw_lock);
   
    ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_RLock_RWLock(ZD_RWLock_T* rw_lock)
{
   	int32_t status;

   	ZD_CON(rw_lock != NULL);

   	status = pthread_rwlock_rdlock(rw_lock);
   
   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_RLock_RWLock_Timeout(ZD_RWLock_T* rw_lock, uint32_t timeout_msec)
{
   	int32_t status;
   	struct timespec timeout;
   	time_t  sec  = (time_t)(timeout_msec/1000u);
   	int32_t nsec = (int32_t)(timeout_msec%1000u)*1000000;

   	ZD_CON(rw_lock != NULL);

   	(void)clock_gettime(CLOCK_REALTIME, &timeout);
   	if ((timeout.tv_nsec + nsec) >= 1000000000L)
   	{
      	nsec -= 1000000000;
      	sec++;     
   	}
   	timeout.tv_sec  += sec;
   	timeout.tv_nsec += nsec;

   	status = pthread_rwlock_timedrdlock(rw_lock, &timeout);

   	ZD_CON(status != EINVAL);
   	ZD_CON((status == 0) || (status == ETIMEDOUT));

   	return (bool)(status == 0);
}

bool ZD_Try_RLock_RWLock(ZD_RWLock_T* rw_lock)
{
   	int32_t status;

   	ZD_CON(rw_lock != NULL);

   	status = pthread_rwlock_tryrdlock(rw_lock);

   	ZD_CON(status != EINVAL);
   	ZD_CON((status == 0) || (status == EBUSY));

   	return (bool)(status == 0);
}

bool ZD_WLock_RWLock(ZD_RWLock_T* rw_lock)
{
   	int32_t status;

   	ZD_CON(rw_lock != NULL);

   	status = pthread_rwlock_wrlock(rw_lock);

   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}

bool ZD_WLock_RWLock_Timeout(ZD_RWLock_T* rw_lock, uint32_t timeout_msec)
{
   	int32_t status;
   	struct timespec timeout;
   	time_t  sec = (time_t)(timeout_msec/1000u);
   	int32_t nsec = (int32_t)(timeout_msec%1000u)*1000000;

   	ZD_CON(rw_lock != NULL);

   	(void)clock_gettime(CLOCK_REALTIME, &timeout);
   	if ((timeout.tv_nsec + nsec) >= 1000000000L)
   	{
     	nsec -= 1000000000;
      	sec++;
   	}
   	timeout.tv_sec  += sec;
  	timeout.tv_nsec += nsec;

	status = pthread_rwlock_timedwrlock(rw_lock, &timeout);   

   	ZD_CON(status != EINVAL);
   	ZD_CON((status == 0) || (status == ETIMEDOUT));

   	return (bool)(status == 0);
}

bool ZD_Try_WLock_RWLock(ZD_RWLock_T* rw_lock)
{
   	int32_t status;

   	ZD_CON(rw_lock != NULL);

   	status = pthread_rwlock_trywrlock(rw_lock);

   	return (bool)(status == 0);
}

bool ZD_Unlock_RWLock(ZD_RWLock_T* rw_lock)
{
   	int32_t status;

   	ZD_CON(rw_lock != NULL);

   	status = pthread_rwlock_unlock(rw_lock);
   
   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}









