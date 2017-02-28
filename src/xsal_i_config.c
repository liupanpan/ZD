#include "xsal_i_config.h"
#include "xsal_settings.h"

/** Identifier of XSAL Application
 */
SAL_App_Id_T  SAL_I_App_Id;

/** Maximum number of XSAL threads the application can have.
 *  This variable also defines maximum value for the XSAL thread identifier.
 */
size_t  SAL_I_Max_Number_Of_Threads;

/** First XSAL Thread ID which can be used during creation of the
 *  thread with "unknown" ID.
 */
SAL_Thread_Id_T SAL_First_Unknown_Thread_Id;

/** Defines maximum number of XSAL timers per process.
 */
size_t  SAL_I_Max_Number_Of_Timers;

/** Maximum number of buffer pools, application will have.
 */
size_t  SAL_I_Max_Number_Of_Buffer_Pools;

/** Defines signal used by XSAL for timer implementation.
 *  If it is not defined by the user then SIGRTMIN is used.
 */
SAL_Int_T SAL_I_Timer_Signal_Id = SAL_I_TIMER_SIGNAL_ID;

/** Defines clock by XSAL for timer implementation.
 *  If it is not defined by the user then CLOCK_REALTIME is used.
 */
SAL_Int_T SAL_I_Timer_Signal_Clock_Id = SAL_I_TIMER_SIGNAL_CLOCK_ID;
