#ifndef XSAL_I_TIME_H
#define XSAL_I_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_settings.h"
#include "xsal_data_types.h"

/** Function initializes XSAL Time module.
 */
void SAL_I_Init_Time_Module(void);

/** Function releases resources allocated by 
 *  SAL_I_Init_Time_Module() function.
 */
void SAL_I_Deinit_Time_Module(void);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_TIME_H */


