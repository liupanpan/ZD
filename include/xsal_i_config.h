#ifndef XSAL_I_CONFIG_H
#define XSAL_I_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"



/** Maximum number of SAL threads the application can have.
 *  This variable also defines maximum value for the SAL thread identifier.
 */
extern size_t  SAL_I_Max_Number_Of_Threads;


/** Maximum number of buffer pools, application will have
 */
extern size_t  SAL_I_Max_Number_Of_Buffer_Pools;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_CONFIG_H */

