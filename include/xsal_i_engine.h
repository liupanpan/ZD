#ifndef XSAL_I_ENGINE_H
#define XSAL_I_ENGINE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"





/** Function signalizes SAL Message Loop to 
 *  terminates and returns given status.
 */
void SAL_I_Stop_Router_Thread(int32_t status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_ENGINE_H */

