#include "zd_time.h"

void ZD_Get_Time(ZD_Time_Spec_T* time_spec)
{
	(void)clock_gettime(CLOCK_REALTIME, time_spec);
}

void ZD_Time_Add_Offset(ZD_Time_Spec_T* time_spec, int32_t time_offset)
{
   	time_t  sec  = (time_t)(time_offset / 1000U);
   	int32_t nsec = (int32_t)(time_offset % 1000u)*1000000;

   	if((time_spec->tv_nsec + nsec) >= 1000000000L)
   	{
    	nsec -= 1000000000;
      	sec++;     
   	}
   	time_spec->tv_sec  += sec;
   	time_spec->tv_nsec += nsec;
}
