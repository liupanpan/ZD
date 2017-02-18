#include "zd_config.h"


/** Maximum number of ZD threads the application can have.
 *  This variable also defines maximum value for the XSAL thread identifier.
 */
size_t  ZD_I_Max_Number_Of_Threads;

/** First ZD Thread ID which can be used during creation of the
 *  thread with "unknown" ID.
 */
int32_t ZD_First_Unknown_Thread_Id;
