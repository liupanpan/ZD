#ifndef XSAL_DATA_TYPES_H
#define XSAL_DATA_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdint.h> 

/** SAL_Int_T type is used internally as procesor native word.
 *  On N-bit platforms it should be defined as intN_t.
 */
typedef int32_t    SAL_Int_T;
typedef uint8_t    SAL_App_Id_T;
typedef SAL_Int_T  SAL_Thread_Id_T;
typedef SAL_Int_T  SAL_Sched_Policy_T;
typedef SAL_Int_T  SAL_Priority_T;
typedef SAL_Int_T  SAL_Event_Id_T;
typedef SAL_Int_T  SAL_Timer_Id_T;
typedef SAL_Int_T  SAL_Buffer_Pool_Id_T;
typedef SAL_Int_T  SAL_Port_Id_T;
typedef SAL_Int_T  SAL_Shared_Memory_Key_T;
typedef uint32_t   SAL_Clock_T;



#include "xsal_data_types_linux.h"




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_DATA_TYPES_H */

