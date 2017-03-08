#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"
#include "xsal_i_thread.h"
#include "xsal_i_timer.h"
#include "xsal_i_time.h"
#include "xsal_i_publish.h"
#include "xsal_i_event_property.h"
#include "xsal_i_event_property.h"
#include "xsal_i_connections.h"

#include <stdlib.h>


void SAL_Deinit(void)
{
   SAL_PRE(SAL_I_App_Id != 0);

   SAL_I_Deinit_Connection_Module();
   SAL_I_Deinit_Event_Property_Module();
   SAL_I_Deinit_Timer_Module();
   SAL_I_Deinit_Buffer_Pools();
   SAL_I_Deinit_Thread_Module();
   SAL_I_Deinit_Publish_Module();
   SAL_I_Deinit_Time_Module();
   SAL_NS_Deinit_Proxy();

   SAL_I_App_Id = 0;
}
