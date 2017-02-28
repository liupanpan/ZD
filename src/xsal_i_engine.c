#include "xsal.h"
#include "xsal_i_engine.h"

#include "xsal_i_timer.h"
#include <stdlib.h>

void SAL_I_Stop_Router_Thread(int32_t status)
{
   SAL_I_Stop_RT_Light(status);
}
