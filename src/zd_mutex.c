#include <string.h>
#include <errno.h>

#include "zd.h"
#include "zd_assert.h"

bool ZD_Create_Mutex(ZD_Mutex_T* mutex, const ZD_Mutex_Attr_T* attr)
{
	int32_t status;

	ZD_CON(mutex != NULL);	

	if(attr == NULL)
	{
		status = pthread_mutex_init(mutex, NULL);
	}
	else
	{	
		pthread_mutexattr_t ptmutex_attr;
		status = pthread_mutexattr_init(&ptmutex_attr);
		if(status == 0)
		{
			if(attr->recursive)
			{
				status = pthread_mutexattr_settype(&ptmutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
			}
		
			if(status == 0)
			{
				status = pthread_mutex_init(mutex, &ptmutex_attr);
			}
			(void)pthread_mutexattr_destroy(&ptmutex_attr);
		}
	}
	
	ZD_CON(status == 0);

	return (bool)(status == 0);
}

bool ZD_Lock_Mutex(ZD_Mutex_T* mutex)
{
	int32_t status;

   	ZD_CON(mutex != NULL);
   
   	status = pthread_mutex_lock(mutex);

   	ZD_CON(status != -1);
   	ZD_CON(status == 0);
   
   	return (bool)(status == 0);
}

bool ZD_Unlock_Mutex(ZD_Mutex_T* mutex)
{
   	int32_t status;

   	ZD_CON(mutex != NULL);

   	status = pthread_mutex_unlock(mutex);

   	ZD_CON(status != -1);
	ZD_CON(status == 0);
   
   	return (bool)(status == 0);
}

bool ZD_Try_Lock_Mutex(ZD_Mutex_T* mutex)
{
   	int32_t status;

   	ZD_CON(mutex != NULL);
   
   	status = pthread_mutex_trylock(mutex);

   	ZD_CON(status != -1);
   	ZD_CON((status == 0) || (status == EBUSY));

   	return (bool)(status == 0);
}

bool ZD_Destroy_Mutex(ZD_Mutex_T* mutex)
{
   	int32_t status;

	ZD_CON(mutex != NULL);

   	status = pthread_mutex_destroy(mutex);

   	ZD_CON(status != -1);
   	ZD_CON(status == 0);

   	return (bool)(status == 0);
}
















