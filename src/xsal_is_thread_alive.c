#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

bool SAL_Is_Thread_Alive(SAL_Thread_Id_T thread_id)
{
   SAL_PRE(thread_id > 0);
   SAL_PRE((size_t)thread_id <= SAL_I_Max_Number_Of_Threads);

   return (bool)(SAL_I_Thread_Table[thread_id].thread_id != SAL_UNKNOWN_THREAD_ID);
} 

