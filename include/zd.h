#ifndef ZD_H
#define ZD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "zd_data_types_linux.h"

/* -------------------------------------------------------------------------
 *
 *                              ZD Mutex
 *
 * -------------------------------------------------------------------------*/
/**
 *  Creates a mutex.
 *
 *  @param [out] mutex  pointer to the created mutex
 *  @param [in]  attr   mutex attributes
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Destroy_Mutex(), ZD_Lock_Mutex(), ZD_Unlock_Mutex()
 */
bool ZD_Create_Mutex(ZD_Mutex_T* mutex, const ZD_Mutex_Attr_T* attr);

/**
 *  Destroys a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be destroyed
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Mutex()
 */
bool ZD_Destroy_Mutex(ZD_Mutex_T* mutex);

/**
 *  Locks a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be locked
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Mutex(), ZD_Try_Lock_Mutex(), 
 *       ZD_Lock_Mutex_Timeout(), ZD_Unlock_Mutex()
 */
bool ZD_Lock_Mutex(ZD_Mutex_T* mutex);

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
 *  @see ZD_Create_Mutex(), ZD_Lock_Mutex(), 
 *       ZD_Lock_Mutex_Timeout, ZD_Unlock_Mutex()
 */
bool ZD_Try_Lock_Mutex(ZD_Mutex_T* mutex);

/**
 *  Unlocks a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be unlocked
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Mutex(), ZD_Lock_Mutex(), ZD_Try_Lock_Mutex()
 */
bool ZD_Unlock_Mutex(ZD_Mutex_T* mutex);







#ifdef __cplusplus
}
#endif 

#endif 
