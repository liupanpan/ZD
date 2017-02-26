#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

static SAL_Buffer_Pool_Id_T SAL_I_Create_Buffer_Pool(SAL_Buffer_Pool_Id_T buffer_pool_id, size_t number_of_buffers, size_t buffer_size)
{
   uint8_t* buff;
   size_t alignment_buffer_size;
   SAL_I_Buffer_Pool_Header_T* buffer_pool = &SAL_I_Buffer_Pools_Tab[buffer_pool_id];
   /** Set the 'alignment_buffer_size' value to the nearest value 
    *  divided by sizeof(size_t) and at least equal 'buffer_size' value.
    *  Next buffer size is increased by 4 bytes to store parent
    *  buffer pool id and buffer_id.
    *  In debug version each buffer has also 4 bytes suffix for 
    *  debug pattern.
    */
   alignment_buffer_size = 
      (((buffer_size+sizeof(size_t)-1u)/sizeof(size_t))*sizeof(size_t)) /* aligment buffer size */
      + sizeof(uint32_t);                               /* space to store pool id and buffer id */
   buff = (uint8_t*)malloc(number_of_buffers*alignment_buffer_size);
   if (buff != NULL)
   {
      size_t i;
      buffer_pool->buffer = buff;
      buffer_pool->buffer_size = alignment_buffer_size;
      buffer_pool->max_number_of_buffers = number_of_buffers;
      buffer_pool->first_free_buffer_id = 0;
      /** Initialize list of free buffers 
       *  and buffer id and pool id in buffer header.
       */
      for(i=0; i < (number_of_buffers-1u); i++)
      {
         /** Initialize buffer header.
          */
         ((uint32_t*)(void*)buff)[0] = Get_Buffer_Id((uint32_t)buffer_pool_id, i);
         /** Initialize list of free buffers.
          *  In the first 4 bytes of buffer data 
          *  store index to the next free buffer.
          */
         ((int32_t*)(void*)buff)[1] = (int32_t)i + 1;
         buff = &buff[alignment_buffer_size];
      }
      /** Initialize the last buffer data
       */
      ((uint32_t*)(void*)buff)[0] = Get_Buffer_Id((uint32_t)buffer_pool_id, i);
      ((int32_t*)(void*)buff)[1] = -1;
   }
   else
   {
      printf("SAL_I_Create_Buffer_Pool/malloc() failed");
      buffer_pool_id = -1;
   }
   return buffer_pool_id;
}

bool SAL_Create_Buffer_Pool(size_t number_of_buffers, size_t buffer_size, SAL_Buffer_Pool_Id_T* pool_id)
{
   bool status = false;

   SAL_PRE(number_of_buffers > 0);
   SAL_PRE(number_of_buffers <= 65535u);
   SAL_PRE(buffer_size > 0);
   SAL_PRE(pool_id != NULL);
   SAL_PRE(SAL_I_Max_Number_Of_Buffer_Pools > 0u);

   if (SAL_I_Max_Number_Of_Buffer_Pools > 0u)
   {
      if (SAL_Lock_Mutex(&SAL_I_Buffer_Pools_Mutex))
      {
         size_t i;
         bool searching = true;
         /** buffer_size must be >= sizeof(int32_t) 
          *  to store list of free buffers.
          */
         buffer_size = (buffer_size < sizeof(int32_t))?sizeof(int32_t):buffer_size;
         /** Find first free buffer pool
          */
         for(i = 0;searching && (i < SAL_I_Max_Number_Of_Buffer_Pools);i++)
         {
            if (SAL_I_Buffer_Pools_Tab[i].buffer == NULL)
            {
               *pool_id = SAL_I_Create_Buffer_Pool(
                  (SAL_Buffer_Pool_Id_T)i,
                  number_of_buffers, 
                  buffer_size);
               if (*pool_id != -1)
               {
                  status = true;
               }
               searching = false;
            }
         }
         (void)SAL_Unlock_Mutex(&SAL_I_Buffer_Pools_Mutex);
      }
   }
   else
   {
      printf("SAL_Create_Buffer_Pool: max number of buffer pools == 0");
   }

   return status;
}
