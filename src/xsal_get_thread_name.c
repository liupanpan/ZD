#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"


const char* SAL_Get_Thread_Name(SAL_Thread_Id_T thread_id)
{
   SAL_PRE(thread_id >= SAL_ROUTER_THREAD_ID);
   SAL_PRE((size_t)thread_id <= SAL_I_Max_Number_Of_Threads);

   return SAL_I_Thread_Table[thread_id].thread_name;
}
