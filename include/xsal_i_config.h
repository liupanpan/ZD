#ifndef XSAL_I_CONFIG_H
#define XSAL_I_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"

/** Identifier of SAL Application
 */
extern SAL_App_Id_T  SAL_I_App_Id;

/** Maximum number of SAL threads the application can have.
 *  This variable also defines maximum value for the SAL thread identifier.
 */
extern size_t  SAL_I_Max_Number_Of_Threads;

/** First SAL Thread ID which can be used during creation of the
 *  thread with "unknown" ID.
 */
extern SAL_Thread_Id_T SAL_First_Unknown_Thread_Id;

/** Defines maximum number of SAL timers per process
 */
extern size_t  SAL_I_Max_Number_Of_Timers;

/** Maximum number of buffer pools, application will have
 */
extern size_t  SAL_I_Max_Number_Of_Buffer_Pools;

/** Defines signal used by XSAL for timer implementation.
 *  If it is not defined by the user then SIGRTMIN is used.
 */
extern SAL_Int_T SAL_I_Timer_Signal_Id;

/** Defines clock by XSAL for timer implementation.
 *  If it is not defined by the user then CLOCK_REALTIME is used.
 */
extern SAL_Int_T SAL_I_Timer_Signal_Clock_Id;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_CONFIG_H */

