#include "xsal.h"
#include "xsal_i_assert.h"

void SAL_Init_Mutex_Attr(SAL_Mutex_Attr_T* attr)
{
   SAL_PRE(attr != NULL);

   /* By default mutex is not recursive */
   attr->recursive = false;
}
