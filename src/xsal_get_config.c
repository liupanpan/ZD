#include "xsal.h"




void SAL_Get_Config(SAL_Config_T* sal_config)
{
   sal_config->app_id                     = 0;
   sal_config->max_number_of_threads      = 0;
   sal_config->max_number_of_timers       = XSAL_MAX_NUMBER_OF_TIMERS;
   sal_config->max_number_of_buffer_pools = XSAL_MAX_NUMBER_OF_BUFFER_POOLS;
   sal_config->first_unknown_thread_id    = -1;
}
