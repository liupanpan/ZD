#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_buffer_pools.h"

/** Pointer to the table with buffer pool headers
 */
SAL_I_Buffer_Pool_Header_T* SAL_I_Buffer_Pools_Tab = NULL;

/** Mutes to synchronize access to the SAL_I_Buffer_Pools_Tab
 */
SAL_Mutex_T SAL_I_Buffer_Pools_Mutex;

bool SAL_I_Init_Buffer_Pools(void)
{
   bool status = false;

   SAL_PRE(SAL_I_Buffer_Pools_Tab == NULL);

   if (SAL_I_Max_Number_Of_Buffer_Pools > 0u)
   {
      SAL_I_Buffer_Pools_Tab = (SAL_I_Buffer_Pool_Header_T*)malloc(SAL_I_Max_Number_Of_Buffer_Pools*sizeof(SAL_I_Buffer_Pool_Header_T));
      if (SAL_I_Buffer_Pools_Tab != NULL)
      {
         if (SAL_Create_Mutex(&SAL_I_Buffer_Pools_Mutex, NULL))
         {
            size_t i;
            for(i=0; i < SAL_I_Max_Number_Of_Buffer_Pools; i++)
            {
               (void)SAL_Create_Mutex(&SAL_I_Buffer_Pools_Tab[i].pool_mutex, NULL);
               SAL_I_Buffer_Pools_Tab[i].buffer = NULL;
            }
            status = true;
         }
         else
         {
            printf("SAL_I_Init_Buffer_Pools/SAL_Create_Mutex() failed");
         }

         if (!status)
         {
            free(SAL_I_Buffer_Pools_Tab);
            SAL_I_Buffer_Pools_Tab = NULL;
         }
      }
      else
      {
         printf("SAL_Init_Buffer_Pools/malloc() failed");
      }
   }
   else
   {
      status = true; 
   }

   SAL_POST(status);

   return status;
}

void SAL_I_Deinit_Buffer_Pools(void)
{
   if (SAL_I_Max_Number_Of_Buffer_Pools > 0u)
   {
      size_t i;
      
      SAL_PRE(SAL_I_Buffer_Pools_Tab != NULL);

      for(i=0; i < SAL_I_Max_Number_Of_Buffer_Pools; i++)
      {
         (void)SAL_Destroy_Mutex(&SAL_I_Buffer_Pools_Tab[i].pool_mutex);
         free(SAL_I_Buffer_Pools_Tab[i].buffer);
      }
      (void)SAL_Destroy_Mutex(&SAL_I_Buffer_Pools_Mutex);
      free(SAL_I_Buffer_Pools_Tab);
      SAL_I_Buffer_Pools_Tab = NULL;
   }
}

