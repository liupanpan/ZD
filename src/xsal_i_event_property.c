#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"

/** Mutex to synchronize access to table with event properties.
 */
SAL_Mutex_T SAL_I_Event_Property_Mutex;
