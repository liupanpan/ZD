#include "xsal_i_config.h"
#include "xsal_settings.h"

/** Identifier of XSAL Application
 */
SAL_App_Id_T  SAL_I_App_Id;

/** Maximum number of XSAL threads the application can have.
 *  This variable also defines maximum value for the XSAL thread identifier.
 */
size_t  SAL_I_Max_Number_Of_Threads;

/** Maximum number of buffer pools, application will have.
 */
size_t  SAL_I_Max_Number_Of_Buffer_Pools;
