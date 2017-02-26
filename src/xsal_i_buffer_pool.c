#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_buffer_pools.h"

/** Pointer to the table with buffer pool headers
 */
SAL_I_Buffer_Pool_Header_T* SAL_I_Buffer_Pools_Tab = NULL;

/** Mutes to synchronize access to the SAL_I_Buffer_Pools_Tab
 */
SAL_Mutex_T SAL_I_Buffer_Pools_Mutex;
