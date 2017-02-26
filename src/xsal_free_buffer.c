#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

void SAL_Free_Buffer(void* buffer)
{
   uint8_t* raw_buffer  = &((uint8_t*)buffer)[-(SAL_Int_T)sizeof(uint32_t)];
   uint32_t pool_id     = Get_Pool_Id(((uint32_t*)(void*)raw_buffer)[0]);
   uint32_t buffer_id   = Get_Buffer_Index(((uint32_t*)(void*)raw_buffer)[0]);
   SAL_I_Buffer_Pool_Header_T* pool_header;

   SAL_PRE((size_t)pool_id < SAL_I_Max_Number_Of_Buffer_Pools);
   SAL_PRE((size_t)buffer_id < SAL_I_Buffer_Pools_Tab[pool_id].max_number_of_buffers);

   pool_header = &SAL_I_Buffer_Pools_Tab[pool_id];

   if (SAL_Lock_Mutex(&pool_header->pool_mutex))
   {
      /** Put the given buffer at the front of the free buffers list
       */
      ((int32_t*)buffer)[0] = pool_header->first_free_buffer_id;
      pool_header->first_free_buffer_id = (int32_t)buffer_id;

      (void)SAL_Unlock_Mutex(&pool_header->pool_mutex);
   }
}
