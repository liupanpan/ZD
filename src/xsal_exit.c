#include "xsal.h"
#include "xsal_i_engine.h"
#include "xsal_i_thread.h"



void SAL_Exit(int32_t status)
{
	SAL_I_Stop_Router_Thread(status);
   	SAL_I_Free_Thread_Resources(SAL_I_Get_Thread_Id());
   	SAL_I_Exit_Thread();
}
