#include "xsal.h"
#include "xsal_i_assert.h"

void SAL_Init_Semaphore_Attr(SAL_Semaphore_Attr_T* attr)
{
   SAL_PRE(attr != NULL);
   attr->initial_value = 0;
}

