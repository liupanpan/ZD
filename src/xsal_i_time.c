#include "xsal_i_time.h"
#include "xsal_i_config.h"

SAL_Clock_T SAL_I_Clock_Start_Time;

void SAL_I_Deinit_Time_Module(void)
{
}

void SAL_I_Get_Time(SAL_I_Time_Spec_T* time_spec)
{
   (void)clock_gettime(CLOCK_REALTIME, time_spec);
}

uint32_t SAL_I_Time_Get_Seconds(const SAL_I_Time_Spec_T* time_spec)
{
   return (uint32_t)time_spec->tv_sec;
}

SAL_Int_T SAL_I_Time_Get_Milli_Seconds(const SAL_I_Time_Spec_T* time_spec)
{
   return (SAL_Int_T)((time_spec->tv_nsec)/1000000);
}

uint32_t SAL_I_Time_Diff(
   const SAL_I_Time_Spec_T* time1_spec,
   const SAL_I_Time_Spec_T* time2_spec)
{
   uint32_t sec = (uint32_t)(time2_spec->tv_sec - time1_spec->tv_sec)*1000U;
   int32_t msec = (int32_t)(time2_spec->tv_nsec - time1_spec->tv_nsec)/1000000;

   return (msec >= 0) ? (sec + (uint32_t)msec) : (sec-(uint32_t)(-msec));
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
