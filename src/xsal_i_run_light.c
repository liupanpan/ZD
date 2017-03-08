#include "xsal.h"
#include "xsal_i_engine.h"
#include "xsal_i_timer.h"


bool SAL_I_Before_Run(void)
{
   SAL_I_Start_Timer_Module();
   return true;
}

int32_t SAL_I_Run(void)
{
   return SAL_I_RT_Light();
}
