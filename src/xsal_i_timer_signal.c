#include "xsal_settings.h"

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"

#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/resource.h>



bool SAL_I_Create_Timer(SAL_I_Timer_T* timer)
{
   timer = timer; /* remove unused variable warning */
   return true;
}
