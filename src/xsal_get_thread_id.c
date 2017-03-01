#include "xsal.h"
#include "xsal_i_thread.h"



SAL_Thread_Id_T SAL_Get_Thread_Id(void)
{
   return SAL_I_Get_Thread_Id();
}
