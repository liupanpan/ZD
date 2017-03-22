#include "xsal.h"
#include "xsal_i_time.h"


SAL_Clock_T SAL_Clock(void)
{
   SAL_I_Time_Spec_T time_spec;

   SAL_I_Get_Time(&time_spec);
   return (SAL_Clock_T)
      (((SAL_I_Time_Get_Seconds(&time_spec) - SAL_I_Clock_Start_Time)*1000) + 
      (uint32_t)SAL_I_Time_Get_Milli_Seconds(&time_spec));
}
