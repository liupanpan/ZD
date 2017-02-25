#ifndef XSAL_H
#define XSAL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "xsal_data_types.h"
#include "stdio.h"

/* -------------------------------------------------------------------------
 *
 *                              SAL Mutex
 *
 * -------------------------------------------------------------------------*/
/**
 *  Initializes mutex attributes.
 *
 *  @param [out]  attr mutex attributes to be initialized
 *
 *  @see SAL_Create_Mutex()
 */
void SAL_Init_Mutex_Attr(SAL_Mutex_Attr_T* attr);

/**
 *  Creates a mutex.
 *
 *  @param [out] mutex  pointer to the created mutex
 *  @param [in]  attr   mutex attributes
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Destroy_Mutex(), SAL_Lock_Mutex(), SAL_Unlock_Mutex()
 */
bool SAL_Create_Mutex(SAL_Mutex_T* mutex, const SAL_Mutex_Attr_T* attr);

/**
 *  Destroys a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be destroyed
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Mutex()
 */
bool SAL_Destroy_Mutex(SAL_Mutex_T* mutex);

/**
 *  Attempts to lock a mutex.
 *
 *  If the mutex is already locked, the calling thread blocks until 
 *  the mutex becomes available or specified timeout expires.
 *
 *  @param [in] mutex         pointer to the mutex to be locked
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @see SAL_Create_Mutex(), SAL_Lock_Mutex(), 
 *       SAL_Try_Lock_Mutex(),SAL_Unlock_Mutex()
 */
bool SAL_Lock_Mutex_Timeout(SAL_Mutex_T* mutex, uint32_t timeout_msec);


/**
 *  Attempts to lock a mutex.
 *
 *  If the mutex is already locked then it returns immediately.
 *
 *  @param [in] mutex  pointer to the mutex to be locked
 *
 *  @return  true on success, false when the mutex is already locked 
 *           or an error occurs
 *
 *  @see SAL_Create_Mutex(), SAL_Lock_Mutex(), 
 *       SAL_Lock_Mutex_Timeout, SAL_Unlock_Mutex()
 */
bool SAL_Try_Lock_Mutex(SAL_Mutex_T* mutex);

/**
 *  Unlocks a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be unlocked
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Mutex(), SAL_Lock_Mutex(), SAL_Try_Lock_Mutex()
 */
bool SAL_Unlock_Mutex(SAL_Mutex_T* mutex);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_H */

