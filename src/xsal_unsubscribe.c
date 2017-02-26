#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"
#include "xsal_i_thread.h"
#include "xsal_i_publish.h"

bool SAL_Unsubscribe(const SAL_Event_Id_T event_id_list[], size_t number_of_events)
{
   bool status = true;
   SAL_Thread_Id_T tid = SAL_I_Get_Thread_Id();

   SAL_PRE(tid > 0);
   SAL_PRE(tid <= (SAL_Thread_Id_T)SAL_I_Max_Number_Of_Threads);
   SAL_PRE((event_id_list != NULL) || (number_of_events == 0));

   if (SAL_Lock_Mutex(&SAL_I_Event_Property_Mutex))
   {
      size_t event_index;
      uint8_t* event_properties = SAL_I_Thread_Event_Property(tid);

      for(
         event_index = 0;
         event_index < number_of_events;
         event_index++)
      {
         SAL_Event_Id_T ev_id = event_id_list[event_index];

         event_properties[ev_id] &= ~SAL_I_Subscribe_Bit_Mask;
      }

      (void)SAL_Unlock_Mutex(&SAL_I_Event_Property_Mutex);
   }
   return status;
}
