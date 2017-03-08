#include "xsal.h"
#include "xsal_i_engine.h"
#include "xsal_i_thread.h"

int32_t SAL_Run(void)
{
   int32_t exit_status;

   if (SAL_I_Number_Of_Running_User_Threads == 0u)
   {
      printf("SAL_Run: no user threads were created");
      exit_status = -1;
   }
   else
   {
      if (SAL_I_Before_Run())
      {
         SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_fnc = (SAL_Thread_Fnc_T)SAL_Run;
         exit_status = SAL_I_Run();
         SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_fnc = NULL;
         SAL_I_After_Run();
      }
      else
      {
         exit_status = -1;
      }
   }
   return exit_status;
}

