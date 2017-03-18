#include "xsal_i_time.h"
#include "xsal_i_config.h"

void SAL_I_Deinit_Time_Module(void)
{
}

void SAL_I_Get_Time(SAL_I_Time_Spec_T* time_spec)
{
   (void)clock_gettime(CLOCK_REALTIME, time_spec);
}

void SAL_I_Time_Add_Offset(
   SAL_I_Time_Spec_T* time_spec,
   uint32_t time_offset)
{
   time_t  sec  = (time_t)(time_offset / 1000U);
   int32_t nsec = (int32_t)(time_offset % 1000u)*1000000;

   if ((time_spec->tv_nsec + nsec) >= 1000000000L)
   {
      nsec -= 1000000000;
      sec++;     
   }
   time_spec->tv_sec  += sec;
   time_spec->tv_nsec += nsec;
}
