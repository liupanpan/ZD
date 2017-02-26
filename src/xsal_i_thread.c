#include <sys/syscall.h>
#include <sys/resource.h>

#include "xsal.h"
#include "xsal_i_thread.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/** Thread Local Storage Key with SAL Thread ID.
 */
SAL_TLS_Key_T SAL_I_Thread_Id_Self;

/** Pointer to table with thread attributes
 */
SAL_I_Thread_Attr_T* SAL_I_Thread_Table;


void* SAL_I_TLS_Get_Specific(SAL_TLS_Key_T tls_key)
{
   return pthread_getspecific(tls_key);
}

/** Returns pointer to calling thread data.
 */
SAL_I_Thread_Attr_T* SAL_I_Get_Thread_Attr(void)
{
   return (SAL_I_Thread_Attr_T*)SAL_I_TLS_Get_Specific(SAL_I_Thread_Id_Self);
}


