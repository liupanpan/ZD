#include "xsal.h"
#include "xsal_i_assert.h"



void SAL_Init_Thread_Attr(SAL_Thread_Attr_T* attr)
{
   SAL_PRE(attr != NULL);

   attr->id         = SAL_UNKNOWN_THREAD_ID;
   attr->priority   = SAL_DEFAULT_THREAD_PRIORITY;
   attr->name       = NULL;
   attr->stack_size = SAL_DEFAULT_THREAD_STACK_SIZE;
}
