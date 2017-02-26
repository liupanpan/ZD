#ifndef XSAL_I_PUBLISH_H
#define XSAL_I_PUBLISH_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"

void SAL_I_Init_Publish_Module(void);
void SAL_I_Deinit_Publish_Module(void);
void SAL_I_Local_Publish(const SAL_Message_T* message);
void SAL_I_Unsubscribe_Thread_Events(SAL_Thread_Id_T thread_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_PUBLISH_H */

