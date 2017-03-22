#ifndef XSAL_I_TIME_H
#define XSAL_I_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_settings.h"
#include "xsal_data_types.h"
#include <time.h>

typedef struct timespec SAL_I_Time_Spec_T;

/* ------------------------------
 *  Exported Object Declarations
 * ------------------------------ */

extern SAL_Clock_T SAL_I_Clock_Start_Time;

/** Function initializes XSAL Time module.
 */
void SAL_I_Init_Time_Module(void);

/** Function releases resources allocated by 
 *  SAL_I_Init_Time_Module() function.
 */
void SAL_I_Deinit_Time_Module(void);

/** Function saves current time (seconds and milliseconds)
 */
void SAL_I_Get_Time(SAL_I_Time_Spec_T* time_spec);

/** Function returns number of seconds from time_spec variable
 */
uint32_t SAL_I_Time_Get_Seconds(const SAL_I_Time_Spec_T* time_spec);

/** Function returns number of milliseconds from time_spec variable
 */
SAL_Int_T SAL_I_Time_Get_Milli_Seconds(const SAL_I_Time_Spec_T* time_spec);

/** Function returns delta (in milliseconds) betwen two times.
 */
uint32_t SAL_I_Time_Diff(
   const SAL_I_Time_Spec_T* time1_spec,
   const SAL_I_Time_Spec_T* time2_spec);

/** Function increase time_spec by time_offset (in milliseconds).
 */
void SAL_I_Time_Add_Offset(
   SAL_I_Time_Spec_T* time_spec,
   uint32_t time_offset);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_TIME_H */


