#ifndef XSAL_TIME_H
#define XSAL_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "zd_data_types_linux.h"
#include <time.h>

typedef struct timespec ZD_Time_Spec_T;

/** Function saves current time (seconds and milliseconds)
 */
void ZD_Get_Time(ZD_Time_Spec_T* time_spec);

/** Function increase time_spec by time_offset (in milliseconds).
 */
void ZD_Time_Add_Offset(ZD_Time_Spec_T* time_spec, int32_t time_offset);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_TIME_H */
