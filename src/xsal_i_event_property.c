#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"

/** Pointer to table with event properties.
 */
uint8_t (*SAL_I_Event_Property)[SAL_MAX_EVENTS];

/** Mutex to synchronize access to table with event properties.
 */
SAL_Mutex_T SAL_I_Event_Property_Mutex;

bool SAL_I_Init_Event_Property_Module(void)
{
   bool status = false;
   if (SAL_Create_Mutex(&SAL_I_Event_Property_Mutex, NULL))
   {
      SAL_I_Event_Property = calloc(SAL_MAX_EVENTS, SAL_I_Max_Number_Of_Threads);
      if (SAL_I_Event_Property != NULL)
      {
         status = true;
      }
      else
      {
         printf("SAL_I_Init_Event_Property_Module/calloc() failed");
      }

      if (!status)
      {
         (void)SAL_Destroy_Mutex(&SAL_I_Event_Property_Mutex);
      }
   }
   else
   {
      printf("SAL_I_Init_Event_Property_Module/SAL_Create_Mutex() failed");
   }
   SAL_POST(status);

   return status;
}
