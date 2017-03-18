#ifndef XSAL_THREAD_PRIORITY_H
#define XSAL_THREAD_PRIORITY_H

#include "include/xsal_data_types.h"


/**
 *  Definition of the XSAL thread priority values for various platforms.
 */
typedef enum SAL_Thread_Priority_Tag
{
   SAL_VERY_HIGH_PRIORITY = 50,
   SAL_HIGH_PRIORITY      = 25,
   SAL_NORMAL_PRIORITY    = 15,
   SAL_LOW_PRIORITY       = 10,
   SAL_VERY_LOW_PRIORITY  = 5,
  
   /** SAL_INHERITED_PRIORITY name is used internally by SAL.
    *  Do not change this name.
    */
   SAL_INHERITED_PRIORITY = -1
} SAL_Thread_Priority_T;

#endif /* XSAL_THREAD_PRIORITY_H */
