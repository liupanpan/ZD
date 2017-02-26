#include "xsal.h"
#include "xsal_i_assert.h"

#include <errno.h>
#include <string.h>

bool SAL_Signal_Semaphore(SAL_Semaphore_T* sem)
{
   SAL_Int_T status;

   SAL_PRE(sem != NULL);

   status = sem_post(sem);

   SAL_POST(status == 0);
   
   return (bool)(status == 0);
}
