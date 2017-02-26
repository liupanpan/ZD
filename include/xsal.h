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
 *  Locks a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be locked
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Mutex(), SAL_Try_Lock_Mutex(), 
 *       SAL_Lock_Mutex_Timeout(), SAL_Unlock_Mutex()
 */
bool SAL_Lock_Mutex(SAL_Mutex_T* mutex);

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
 *  @see SAL_Create_Semaphore()
 */
void SAL_Init_Semaphore_Attr(SAL_Semaphore_Attr_T* attr);

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
 *  @see SAL_Destroy_Semaphore(), SAL_Wait_Semaphore(), 
 *       SAL_Signal_Semaphore()
 */
bool SAL_Create_Semaphore(SAL_Semaphore_T* sem, const SAL_Semaphore_Attr_T* attr);

/**
 *  Destroys a semaphore.
 *
 *  @param [in] sem  pointer to the semaphore to be destroyed
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Semaphore()
 */
bool SAL_Destroy_Semaphore(SAL_Semaphore_T* sem);

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
 *  @see SAL_Create_Semaphore(), SAL_Try_Wait_Semaphore(),
 *       SAL_Wait_Semaphore_Timeout(), SAL_Signal_Semaphore()
 */
bool SAL_Wait_Semaphore(SAL_Semaphore_T* sem);

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
 *  @see SAL_Create_Semaphore(), SAL_Wait_Semaphore(),
 *       SAL_Wait_Semaphore_Timeout(), SAL_Signal_Semaphore()
 */
bool SAL_Try_Wait_Semaphore(SAL_Semaphore_T* sem);

/**
 *  Signals a semaphore.
 *
 *  @param [in] sem  pointer to the semaphore to be signaled
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Semaphore(), SAL_Wait_Semaphore(), 
 */
bool SAL_Signal_Semaphore(SAL_Semaphore_T* sem);

/* -------------------------------------------------------------------------
 *
 *                              Buffer Pools
 *
 * -------------------------------------------------------------------------*/
/**
 *  Creates a buffer pool.
 *
 *  @param [in] number_of_buffers  number of buffers in the pool
 *  @param [in] buffer_size        size of each buffer
 *  @param [out] pool_id           pointer to the returned buffer 
 *                                 pool identifier
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Alloc_Buffer(), SAL_Free_Buffer()
 */
bool SAL_Create_Buffer_Pool(size_t number_of_buffers, size_t buffer_size, SAL_Buffer_Pool_Id_T* pool_id);

/**
 *  Destroys the specified buffer pool.
 *
 *  @param [in] pool_id  buffer pool to be destroyed
 * 
 *  @see SAL_Create_Buffer_Pool()
 */
void SAL_Destroy_Buffer_Pool(SAL_Buffer_Pool_Id_T pool_id);

/**
 *  Allocates a buffer from the specified buffer pool.
 *
 *  @param [in] pool_id  identifier of the buffer pool to allocate
 *                       the buffer from
 *
 *  @return  pointer to the allocated buffer or NULL on error
 */
void* SAL_Alloc_Buffer(SAL_Buffer_Pool_Id_T pool_id);

/**
 *  Returns the given buffer to its pool.
 *
 *  @param [in] buffer  pointer to the allocated buffer
 *
 */
void SAL_Free_Buffer(void* buffer);







#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_H */

