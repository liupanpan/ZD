#ifndef XSAL_I_CONFIG_H
#define XSAL_I_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "zd_data_types.h"

/** Define Sheduling thread polisy (for POSIX systems)
 */
#if !defined (ZD_SCHED_POLICY)
#define ZD_SCHED_POLICY SCHED_RR
#endif /* ZD_SCHED_POLICY */



/** Maximum number of ZD threads the application can have.
 *  This variable also defines maximum value for the ZD thread identifier.
 */
extern size_t  ZD_I_Max_Number_Of_Threads;

/** First ZD Thread ID which can be used during creation of the
 *  thread with "unknown" ID.
 */
extern int32_t ZD_First_Unknown_Thread_Id;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
