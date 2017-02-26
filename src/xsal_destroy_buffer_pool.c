#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

void SAL_Destroy_Buffer_Pool(SAL_Buffer_Pool_Id_T pool_id)
{
   SAL_I_Buffer_Pool_Header_T* pool_header;

   SAL_PRE(pool_id >= 0);
   SAL_PRE((size_t)pool_id < SAL_I_Max_Number_Of_Buffer_Pools);

   pool_header = &SAL_I_Buffer_Pools_Tab[pool_id];
   if (SAL_Lock_Mutex(&pool_header->pool_mutex))
   {
      free(pool_header->buffer);

      if (SAL_Lock_Mutex(&SAL_I_Buffer_Pools_Mutex))
      {
         pool_header->first_free_buffer_id = -1;
         pool_header->buffer = NULL;
         (void)SAL_Unlock_Mutex(&SAL_I_Buffer_Pools_Mutex);
      }  
      (void)SAL_Unlock_Mutex(&pool_header->pool_mutex);
   }
   else
   {
      SAL_PRE_FAILED();
   }
}



