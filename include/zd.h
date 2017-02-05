#ifndef ZD_H
#define ZD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
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

/* -------------------------------------------------------------------------
 *
 *                              SAL Semaphore
 *
 * -------------------------------------------------------------------------*/

/**
 *  Initializes semaphore attributes.
 *
 *  @param [out]  attr  semaphore attributes
 *
 *  @see ZD_Create_Semaphore()
 */
void ZD_Init_Semaphore_Attr(ZD_Semaphore_Attr_T* attr);

/**
 *  Creates a semaphore.
 *
 *  @param [out] sem   pointer to the semaphore to be created
 *  @param [in]  attr  semaphore attributes.
 *
 *  @note Semafore initial value is defined in structure pointed by 
 *        attr pointer. If the pointer is equal NULL then initial values 
 *        is set to 0.
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Destroy_Semaphore(), ZD_Wait_Semaphore(), 
 *       ZD_Signal_Semaphore()
 */
bool ZD_Create_Semaphore(ZD_Semaphore_T* sem, const ZD_Semaphore_Attr_T* attr);

/**
 *  Destroys a semaphore.
 *
 *  @param [in] sem  pointer to the semaphore to be destroyed
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Semaphore()
 */
bool ZD_Destroy_Semaphore(ZD_Semaphore_T* sem);

/**
 *  Attempts to wait for a semaphore.
 *
 *  Suspends the calling thread until the semaphore pointed to by 
 *  sem has non-zero count.
 *
 *  @param [in] sem  pointer to the semaphore to wait for
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Semaphore(), ZD_Try_Wait_Semaphore(),
 *       ZD_Wait_Semaphore_Timeout(), ZD_Signal_Semaphore()
 */
bool ZD_Wait_Semaphore(ZD_Semaphore_T* sem);

/**
 *  Attempts to wait for a semaphore with a timeout.
 *
 *  If semaphore count is 0, the calling thread blocks until 
 *  the semaphore is signaled or specified timeout expires.
 *
 *  @param [in] sem           pointer to the semaphore to wait for
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 * 
 *  @see ZD_Create_Semaphore(), ZD_Wait_Semaphore(), 
 *       ZD_Try_Wait_Semaphore(), ZD_Signal_Semaphore()
 */
bool ZD_Wait_Semaphore_Timeout(ZD_Semaphore_T* sem, int32_t timeout_msec);

/**
 *  Attempts to wait for a semaphore.
 *
 *  Function does not block the calling thread if semaphore
 *  value is equal to zero.
 *
 *  @param [in] sem  pointer to the semaphore to wait for
 *
 *  @return  true on success, false when semaphore value is zero
 *           or an error occurs
 * 
 *  @see ZD_Create_Semaphore(), ZD_Wait_Semaphore(),
 *       ZD_Wait_Semaphore_Timeout(), ZD_Signal_Semaphore()
 */
bool ZD_Try_Wait_Semaphore(ZD_Semaphore_T* sem);

/**
 *  Signals a semaphore.
 *
 *  @param [in] sem  pointer to the semaphore to be signaled
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Semaphore(), ZD_Wait_Semaphore(), 
 */
bool ZD_Signal_Semaphore(ZD_Semaphore_T* sem);

/* -------------------------------------------------------------------------
 *
 *                         SAL Conditional Variable
 *
 * -------------------------------------------------------------------------*/
/**
 *  Initializes conditional variable attributes.
 *
 *  @param [out] attr  conditional variable attributes to be initialized.
 *
 *  @note Currently ZD doesn't support any conditional variable attributes.
 *
 *  @see ZD_Create_Cond()
 */
void ZD_Init_Cond_Attr(ZD_Cond_Attr_T* attr);

/**
 *  Creates a conditional variable.
*
 *  @param [out] cond   pointer to the created conditional variable.
 *  @param [in]  attr   conditional variable attributes. If the attr pointer
 *                      is NULL, then the default attributes are used.
 *
 *  @return  true on success, false on failure
 *
 *  @note Currently ZD doesn't support any Conditional Variable attributes, 
 *        so the attr param has to be set to NULL.
 *
 *  @see ZD_Destroy_Cond(), ZD_Wait_Cond(), ZD_Signal_Cond()
 */
bool ZD_Create_Cond(ZD_Cond_T* cond, const ZD_Cond_Attr_T* attr);

/**
 *  Destroys a conditional variable.
 *
 *  @param [in] cond  pointer to the conditional variable to be destroyed.
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Cond().
 */
bool ZD_Destroy_Cond(ZD_Cond_T* cond);

/**
 *  Wait for conditional variable.
 *
 *  @param [in] cond   pointer to the conditional variable to wait for
 *  @param [in] mutex  pointer to the critical section mutex 
 *
 *  @return  true on success, false on failure
 *
 *  @note Some Operating Systems do not allow recursive mutexes with 
 *        condition variables.
 * 
 *  @see ZD_Create_Cond(), ZD_Wait_Cond_Timeout(), ZD_Signal_Cond()
 */
bool ZD_Wait_Cond(ZD_Cond_T* cond, ZD_Mutex_T* mutex);

/**
 *  Wait for conditional variable until it will be signalized
 *  or the specified timeout expires.
 *
 *  @param [in] cond          pointer to the conditional variable to wait for
 *  @param [in] mutex         pointer to the critical section mutex 
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @note Some Operating Systems do not allow recursive mutexes 
 *        with condition variables.
 *
 *  @see ZD_Create_Cond(), ZD_Wait_Cond(), ZD_Signal_Cond()
 */
bool ZD_Wait_Cond_Timeout(ZD_Cond_T* cond, ZD_Mutex_T* mutex, uint32_t timeout_msec);

/**
 *  Signals a conditional variable.
 *
 *  @param [in] cond   pointer to the conditional variable to be signaled
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_Cond(), ZD_Wait_Cond()
 */
bool ZD_Signal_Cond(ZD_Cond_T* cond);

/* -------------------------------------------------------------------------
 *
 *                          SAL Read-Write Lock
 *
 * -------------------------------------------------------------------------*/
/**
 *  Initializes Read-Write lock attributes.
 *
 *  @param [out] attr  Read-Write lock attributes to be initialized.
 *
 *  @note Currently ZD doesn't support any Read-Write attributes.
 *
 *  @see ZD_Create_RWLock()
 */
void ZD_Init_RWLock_Attr(ZD_RWLock_Attr_T* attr);

/**
 *  Creates a Read-Write lock.
 *
 *  @param [out] rw_lock  pointer to the created Read-Write lock.
 *  @param [in]  attr     Read-Write lock attributes. If the attr pointer is
 *                        NULL, then the default attributes are used.
 *
 *  @return  true on success, false on failure
 *
 *  @note Currently ZD doesn't support any Read-Write attributes, 
 *        so the attr param has to be set to NULL.
 *
 *  @see ZD_Destroy_RWLock(), ZD_RLock_RWLock(), ZD_WLock_RWLock(),
 *       ZD_Unlock_RWLock().
 */
bool ZD_Create_RWLock(ZD_RWLock_T* rw_lock, const ZD_RWLock_Attr_T* attr);

/**
 *  Destroys a Read-Write lock.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be destroyed.
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_RWLock().
 */
bool ZD_Destroy_RWLock(ZD_RWLock_T* rw_lock);

/**
 *  Locks a Read-Write lock on Read operation.
 *
 *  @param [in] rw_lock  pointer to the Read_Write lock.
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_RWLock(), 
 *       ZD_RLock_RWLock_Timeout(), ZD_Try_RLock_RWLock(),
 *       ZD_Unlock_RWLock().
 */
bool ZD_RLock_RWLock(ZD_RWLock_T* rw_lock);

/**
 *  Attempts to lock a Read-Write lock on Read operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  the calling thread blocks until the Read-Write lock becomes available 
 *  or specified timeout expires.
 *
 *  @param [in] rw_lock       pointer to the Read-Write lock to be locked
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @see ZD_Create_RWLock(),
 *       ZD_RLock_RWLock(), ZD_Try_RLock_RWLock(),
 *       ZD_Unlock_RWLock().
 */
bool ZD_RLock_RWLock_Timeout(ZD_RWLock_T* rw_lock, uint32_t timeout_msec);

/**
 *  Attempts to lock a Read-Write lock on Read operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  then it returns immediately.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be locked
 *
 *  @return  true on success, false when the rw_lock is already locked 
 *           or an error occurs
 *
 *  @see ZD_Create_RWLock(),
 *       ZD_RLock_RWLock(), ZD_RLock_RWLock_Timeout(),
 *       ZD_Unlock_RWLock().
 */
bool ZD_Try_RLock_RWLock(ZD_RWLock_T* rw_lock);

/**
 *  Locks a Read-Write lock on Write operation.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be locked.
 *
 *  @return  true on success, false on failure.
 *
 *  @see ZD_Create_RWLock(),
 *       ZD_WLock_RWLock_Timeout(), ZD_Try_WLock_RWLock(), 
 *       ZD_Unlock_RWLock().
 */
bool ZD_WLock_RWLock(ZD_RWLock_T* rw_lock);

/**
 *  Attempts to lock a Read-Write on Write operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  the the calling thread blocks until the Read-Write lock becomes available 
 *  or specified timeout expires.
 *
 *  @param [in] rw_lock         pointer to the Read-Write lock to be locked
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @see SAL_Create_RWLock(),
 *       SAL_WLock_RWLock(), SAL_Try_WLock_RWLock(), 
 *       SAL_Unlock_RWLock().
 */
bool ZD_WLock_RWLock_Timeout(ZD_RWLock_T* rw_lock, uint32_t timeout_msec);

/**
 *  Attempts to lock a Read-Write on Write operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  then it returns immediately.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be locked.
 *
 *  @return  true on success, false when the mutex is already locked 
 *           or an error occurs
 *
 *  @see ZD_Create_Mutex(), ZD_Lock_Mutex(), 
 *       ZD_Lock_Mutex_Timeout, ZD_Unlock_Mutex()
 */
bool ZD_Try_WLock_RWLock(ZD_RWLock_T* rw_lock);

/**
 *  Unlocks a Read-Write lock.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be unlocked
 *
 *  @return  true on success, false on failure
 *
 *  @see ZD_Create_RWLock(), ZD_RLock_RWLock(), ZD_WLock_RWLock()
 */
bool ZD_Unlock_RWLock(ZD_RWLock_T* rw_lock);







#ifdef __cplusplus
}
#endif 

#endif 
