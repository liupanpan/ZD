#ifndef XSAL_SETTINGS_H
#define XSAL_SETTINGS_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/***********************************
 *
 *       Timer configuration
 *
 ***********************************/

/** There are 3 SAL Timer implementations for diffrent OS's:
 *
 *  PULSES    - this should be selected for QNX
 *  SIGNALS   - this should be selected for UNIX/LINUX or QNX
 *  WIN32_API - this should be selected for Windows
 */
#define XSAL_TIMER_USE_PULSES    1
#define XSAL_TIMER_USE_SIGNALS   2
#define XSAL_TIMER_USE_WIN32_API 3

/** Choose SAL Timer implementation.
 *
 *  User must change this macro, depending on their operating system.
 */
#if !defined(XSAL_TIMER_IMPL)
#define XSAL_TIMER_IMPL XSAL_TIMER_USE_SIGNALS
#endif /* XSAL_TIMER_IMPL */

/** Defines maximum number of timers which application can have.
 *
 *  This value may be changed during runtime, calling SAL_Init() function.
 */
#if !defined(XSAL_MAX_NUMBER_OF_TIMERS)
#define XSAL_MAX_NUMBER_OF_TIMERS 64
#endif /* XSAL_MAX_NUMBER_OF_TIMERS */

#if XSAL_TIMER_IMPL == XSAL_TIMER_USE_SIGNALS
#if !defined(SAL_I_TIMER_SIGNAL_ID)
/** If SAL Timer uses signals then user can define which signal is uesd.
 *  The user cannot use this signal for own purposes.
 *
 *  If this is not defined the the default value is SIGRTMIN
 *
 *  This value may be changed during runtime, calling SAL_Init() function.(TBD)
 */
#define SAL_I_TIMER_SIGNAL_ID SIGVTALRM
#endif /* !SAL_I_TIMER_SIGNAL_ID */
#endif /* XSAL_TIMER_IMPL == XSAL_TIMER_USE_SIGNALS */

#if !defined(SAL_I_TIMER_SIGNAL_CLOCK_ID)
/** If SAL Timer uses signals or pulses user can choose OS Clock.
 *
 *  If this is not defined the the default value is CLOCK_REALTIME.
 *
 *  This value may be changed during runtime, calling SAL_Init() function.(TBD)
 */
#define SAL_I_TIMER_SIGNAL_CLOCK_ID CLOCK_REALTIME
#endif /* SAL_I_TIMER_SIGNAL_CLOCK_ID */

/***********************************
 *
 *    Buffer Pools configuration
 *
 ***********************************/
/** Define maximum number of Buffer Pools which application can have.
 *
 *  This value may be changed during runtime, calling SAL_Init() function.
 */
#if !defined(XSAL_MAX_NUMBER_OF_BUFFER_POOLS)
#define XSAL_MAX_NUMBER_OF_BUFFER_POOLS 32
#endif /* XSAL_MAX_NUMBER_OF_BUFFER_POOLS */

/** Maximum length of the thread name (including terminating string byte).
 */
#define SAL_THREAD_NAME_MAX_LENGTH (8+1)

/** Define Sheduling thread polisy (for POSIX systems)
 */
#if !defined (XSAL_SCHED_POLICY)
#define XSAL_SCHED_POLICY SCHED_RR
#endif /* XSAL_SCHED_POLICY */


/** Define default thread priority value.
 */
#if !defined(SAL_DEFAULT_THREAD_PRIORITY)
#define SAL_DEFAULT_THREAD_PRIORITY   10
#endif /* SAL_DEFAULT_THREAD_PRIORITY */

/** Define multiplier for channels number.
 */
#if !defined(SAL_NS_CHANNEL_MULTIPLIER)
#define SAL_NS_CHANNEL_MULTIPLIER 3
#endif /* SAL_NS_CHANNEL_MULTIPLIER */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_SETTINGS_H */

