#include "xsal.h"
#include "xsal_i_assert.h"

#include <errno.h>
#include <string.h>

bool SAL_Try_Wait_Semaphore(SAL_Semaphore_T* sem)
{
   SAL_Int_T err;
   SAL_Int_T status;

   SAL_PRE(sem != NULL);

   status = sem_trywait(sem);
   err = errno;

   SAL_POST((status == 0) || (err == EAGAIN));
   
   return (bool)(status == 0);
}

