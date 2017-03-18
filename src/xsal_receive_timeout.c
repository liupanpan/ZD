#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
//#include "xsal_i_receive.h"


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


const SAL_Message_T* SAL_Receive_Timeout(uint32_t timeout_ms)
{
   SAL_I_Thread_Attr_T* queue_owner_thread = SAL_I_Get_Thread_Attr();

   SAL_PRE(queue_owner_thread != NULL);
   SAL_PRE(queue_owner_thread->thread_id > 0);
   SAL_PRE((size_t)queue_owner_thread->thread_id <= SAL_I_Max_Number_Of_Threads);

   return SAL_I_Receive_From_Timeout(queue_owner_thread, timeout_ms);
}

