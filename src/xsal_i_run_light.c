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

void SAL_I_After_Run(void)
{
   SAL_I_Stop_Timer_Module();

   (void)SAL_Wait_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_ready_sem);
   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].is_ready = false;

   (void)SAL_Signal_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_destroyed_sem);
}
