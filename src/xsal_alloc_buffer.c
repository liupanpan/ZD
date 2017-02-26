#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

void* SAL_Alloc_Buffer(SAL_Buffer_Pool_Id_T pool_id)
{
   uint8_t* buff = NULL;
   SAL_I_Buffer_Pool_Header_T* pool_header;

   SAL_PRE(pool_id >= 0);
   SAL_PRE((size_t)pool_id < SAL_I_Max_Number_Of_Buffer_Pools);

   pool_header = &SAL_I_Buffer_Pools_Tab[pool_id];

   if (SAL_Lock_Mutex(&pool_header->pool_mutex))
   {
      int32_t buffer_id = pool_header->first_free_buffer_id;
      if (buffer_id != -1)
      {
         /** Get pointer to the buffer header.
          */
         buff = &pool_header->buffer[(uint32_t)buffer_id*pool_header->buffer_size];
         pool_header->first_free_buffer_id = ((int32_t*)(void*)&(buff[sizeof(uint32_t)]))[0];
      }
      (void)SAL_Unlock_Mutex(&pool_header->pool_mutex);

   }
   else
   {
      SAL_PRE_FAILED();
   }

   return (buff == NULL) ? NULL: (&buff[sizeof(uint32_t)]);
}
