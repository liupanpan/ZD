#include "xsal.h"
#include "xsal_i_assert.h"
#include <errno.h>
#include <string.h>

bool SAL_Create_Semaphore(SAL_Semaphore_T* sem, const SAL_Semaphore_Attr_T* attr)
{
   SAL_Int_T status;
   uint32_t init_value = (attr == NULL) ? 0u : attr->initial_value;

   SAL_PRE(sem != NULL);

   status = sem_init(sem, 0, init_value);

   SAL_POST(status == 0);

   return (bool)(status == 0);
}

