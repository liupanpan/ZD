#ifndef XSAL_H
#define XSAL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_settings.h"
#include "xsal_data_types.h"
#include "stdio.h"
#include <stdbool.h>
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
 *  @see SAL_Create_Semaphore(), SAL_Wait_Semaphore(), 
 *       SAL_Try_Wait_Semaphore(), SAL_Signal_Semaphore()
 */
bool SAL_Wait_Semaphore_Timeout(SAL_Semaphore_T* sem, uint32_t timeout_msec);

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

/* -------------------------------------------------------------------------
 *
 *                              Utility functions
 *
 * -------------------------------------------------------------------------*/
/** 
 *  OS independent memory allocator 
 *
 *  @param [in] size number of bytes to allocate
 *
 *  @return pointer to allocated memory, NULL if unable to allocate
 *
 *  @see SAL_Free()
 */
void* SAL_Alloc(size_t size);

/** 
 *  OS independent memory free 
 *
 *  @param [in] ptr pointer to block previously allocated by SAL_Alloc()
 *
 *  @see SAL_Alloc()
 */
void SAL_Free(void* ptr);

/** 
 *  Function returns program execution time (since an implementation-defined
 *  point of time) in miliseconds.
 *
 *  @return  number of milliseconds
 *
 *  @see SAL_Elapsed_Time(), SAL_Delta_Time()
 */
SAL_Clock_T SAL_Clock(void);


/** 
 *  Function returns the number of milliseconds elapsed since/until timestamp.
 *  The number returned is negative if the timestamp lies in the future.
 *
 *  @param [in] timestamp timestamp to calculate elapsed time
 *
 *  @return  number of milliseconds since/until timestamp
 *
 *  @see SAL_Clock(), SAL_Delta_Time()
 */
SAL_Int_T SAL_Elapsed_Time(SAL_Clock_T timestamp);

/** 
 * Function returns the delta time (in milliseconds) between two timestamps.
 *
 *  @param [in] start_time first timestamp to calculate delta
 *  @param [in] end_time second timestamp to calculate delta
 *
 *  @return  delat (in millicecondsd) between start_time and end_time
 *
 *  @see SAL_Clock(), SAL_Delta_Time()
 */
SAL_Int_T SAL_Delta_Time(SAL_Clock_T start_time, SAL_Clock_T end_time);

/* -------------------------------------------------------------------------
 *
 *                              Queue management
 *
 * -------------------------------------------------------------------------*/
/**
 *  Creates a message queue for the calling thread. 
 *
 *  Every thread must create its message queue before it can receive 
 *  asynchronous messages.
 *
 *  XSAL pre-allocates queue_size memory buffers of size message_size and 
 *  places the received messages in the buffers.
 *
 *  If the size of the arriving message is greater than message_size then 
 *  XSAL uses alloc_buf to dynamically allocate a memory buffer (and free_buf
 *  to release it).
 *
 *  If message_size equals zero then memory buffers are not pre-allocated 
 *  and alloc_buf and free_buf are always used instead.
 *
 *  If alloc_buf and free_buf are NULL, the message is dropped when its size
 *  is greater than message_size.
 *
 *  @note Pre-allocated buffers are fast (memory allocation/de-allocation
 *        is not required) but more memory may be used because queue_size 
 *        buffers are always needed (even if the queue is partially empty).
 *
 *  @note When a thread is terminated, its message queue is destroyed
 *        automatically.
 *
 *  @param [in] queue_size    maximum number of messages the queue can have
 *  @param [in] message_size  maximum message size (pre-allocated 
 *                            buffer size)
 *  @param [in] alloc_buf     pointer to the function allocating memory 
 *                            for arriving messages
 *  @param [in] free_buf      pointer to the function freeing memory used 
 *                            by messages
 *  @return  true on success, false on failure
 *
 *  @see SAL_Send(), SAL_Publish(),
 *       SAL_Receive(), SAL_Receive_From(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent(),
 *       SAL_Subscribe(), SAL_Unsubscribe()
 */
bool SAL_Create_Queue(size_t queue_size,size_t message_size,void* (*alloc_buf)(size_t size),void (*free_buf)(void* buf));

/**
 *  Retrieves the status information and statistics related to 
 *  a message queue owned by thread thread_id.
 *
 *  If a given thread does not have queue, function returns false.
 *
 *  @param [in]  thread_id  ID of the thread whose the queue is examined
 *  @param [out] queue_stat pointer to structure in which queue statistics 
 *               are returned
 *
 *  @return  true on success, false on failure
 */ 
bool SAL_Stat_Queue(SAL_Thread_Id_T thread_id, SAL_Stat_Queue_T* queue_stat);

/* -------------------------------------------------------------------------
 *
 *                           Message passing
 *
 * -------------------------------------------------------------------------*/
/**
 *  Puts a message into the queue of the specified thread.
 *
 *  @note Function SAL_Send() also can be used to achieve limited event 
 *        publication. When thread ID equals zero, SAL_Send() publishes 
 *        the event only in the specified application.
 *
 *  @param [in] app_id     ID of the application to send the message to 
 *  @param [in] thread_id  ID of the thread to send the message to 
 *  @param [in] event_id   ID of the message
 *  @param [in] data       pointer to message data
 *  @param [in] data_size  size of message data
 *
 *  @return                true on success, false on failure
 *
 *  @see SAL_Declare_Urgent(), SAL_Undeclare_Urgent(), SAL_Publish()
 */ 
bool SAL_Send(SAL_App_Id_T app_id, SAL_Thread_Id_T thread_id, SAL_Event_Id_T event_id, const void* data, size_t data_size);

/**
 *  Retrieves a message from the thread's queue with a timeout.
 *
 *  Function blocks the calling thread until a message becomes available 
 *  or the timeout expires.
 *
 *  @param timeout_ms  timeout value
 *
 *  @return  pointer to the received message or NULL in case of timeout
 *
 *  @see SAL_Receive_From()
 */
const SAL_Message_T* SAL_Receive_Timeout(uint32_t timeout_ms);

/* -------------------------------------------------------------------------
 *
 *                            Event subscribtion
 *
 * -------------------------------------------------------------------------*/
/**
 *  Subscribes the calling thread to published events.
 *
 *  Published events are delivered only to threads subscribed to them.
 *
 *  It is possible to call SAL_Subscribe() / SAL_Unsubscribe() 
 *  (@see SAL_Unsubscribe()) more than once 
 *  (and with different sets of events). The newly subscribed/unsubscribed 
 *  events will be added to/removed from the set of already subscribed ones.
 *
 *  @note Functions SAL_Declare_Urgent() and SAL_Undeclare_Urgent() 
 *        work with subscribed events too.
 *
 *  @param [in] event_id_list     list of events to subscribe to
 *  @param [in] number_of_events  number of events to subscribe to
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Unsubscribe(), SAL_Publish(), SAL_Local_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent()
 */
bool SAL_Subscribe(const SAL_Event_Id_T event_id_list[], size_t number_of_events);

/**
 *  Unsubscribes the calling thread from events.
 *
 *  @param [in] event_id_list     list of events to unsubscribe from
 *  @param [in] number_of_events  number of events to unsubscribe from
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Subscribe(), SAL_Publish(), SAL_Local_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent()
 */
bool SAL_Unsubscribe(const SAL_Event_Id_T event_id_list[], size_t number_of_events);

/* -------------------------------------------------------------------------
 *
 *                           Message publishing
 *
 * -------------------------------------------------------------------------*/
/**
 *  Deliveres an event to all subscribers (in all applications 
 *  in the system) at once.
 *
 *  @param [in] event_id   event identifier
 *  @param [in] data       data to be delivered with the event
 *  @param [in] data_size  size of data
 *
 *  @see SAL_Subscribe(), SAL_Unsubscribe(), SAL_Local_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent()
 */ 
void SAL_Publish(SAL_Event_Id_T event_id, const void* data, size_t data_size);

/* -------------------------------------------------------------------------
 *
 *                           Timer management
 *
 * -------------------------------------------------------------------------*/
/**
 *  Creates a timer sending an event upon expiration.
 *  After creation, the timer is not running yet. 
 *  It is started with a call to SAL_Start_Timer().
 *
 *  @note XSAL timer events are sent only to the thread that has created 
 *        the timer. Threads do not have to subscribe to timer events to 
 *        receive them. Timer events do not carry any data, only event ID. 
 *        Timers can be single-shot or periodic. 
 *
 *  @note Only the thread that created the timer may start 
 *        (by calling SAL_Start_Timer()), stop (by calling SAL_Stop_Timer()) 
 *        or destroy it (by calling SAL_Destroy_Timer()).
 *        In other words, timers are local to threads creating them.
 *
 *  @param [in]  event_id  ID of event which is sent upon timer expiration
 *  @param [out] timer_id  ID of the timer
 *
 *  @return true on succes, false on failure
 *
 *  @see SAL_Start_Timer(), SAL_Stop_Timer(), SAL_Destroy_Timer(), 
 *       SAL_Destroy_Thread()
 */
bool SAL_Create_Timer(SAL_Event_Id_T event_id, SAL_Timer_Id_T* timer_id);

/**
 *  Destroys the timer identified by timer_id.
 *
 *  @note Events generated by a timer are not removed from 
 *        the thread's message queue when the timer is destroyed. 
 *
 *  @param [in] timer_id  ID of the timer to be destroyed
 *
 *  @see SAL_Create_Timer(), SAL_Stop_Timer(), SAL_Destroy_Thread()
 */
void  SAL_Destroy_Timer(SAL_Timer_Id_T timer_id);

/**
 *  Starts the timer identified by timer_id.
 *  Timer interval is given in milliseconds but timer resolution 
 *  depends on the underlying operating system. 
 *
 *  Parameter is_periodic defines the timer as periodic or single-shot.
 *
 *  @param [in] timer_id       ID of the timer to be started
 *  @param [in] interval_msec  timer interval
 *  @param [in] is_periodic    timer mode: single-shot if false, 
 *                             periodic otherwise
 *
 *  @see SAL_Create_Timer(), SAL_Start_Timer_Ex(), SAL_Stop_Timer(),
 *       SAL_Destroy_Timer()
 */
void SAL_Start_Timer(SAL_Timer_Id_T timer_id, uint32_t interval_msec, bool is_periodic);

/**
 *  Stops the timer identified by timer_id.
 *
 *  @param [in] timer_id  ID of the timer to be stopped
 *
 *  @note Events generated by a timer are not removed from 
 *        the thread's message queue when the timer is stopped. 
 *
 *  @see SAL_Create_Timer(), SAL_Start_Timer(), SAL_Destroy_Timer()
 */
void  SAL_Stop_Timer(SAL_Timer_Id_T timer_id); 

/* -------------------------------------------------------------------------
 *
 *                           Thread management
 *
 * -------------------------------------------------------------------------*/
void SAL_Init_Thread_Attr(SAL_Thread_Attr_T* attr);

SAL_Thread_Id_T  SAL_Create_Thread(void (*thread_function)(void*), void* param, const SAL_Thread_Attr_T* attr);

bool SAL_Is_Thread_Alive(SAL_Thread_Id_T thread_id);

SAL_Thread_Id_T  SAL_Get_Thread_Id(void);

const char* SAL_Get_Thread_Name(SAL_Thread_Id_T thread_id);

bool  SAL_Set_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T priority);

bool  SAL_Get_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T* priority);

void  SAL_Sleep(uint32_t milliseconds);

void  SAL_Delay(uint32_t microseconds);
/* -------------------------------------------------------------------------
 *
 *              Thread synchronization at startup/termination
 *
 * -------------------------------------------------------------------------*/
/**
 *  Notifies other threads in a process that a given thread 
 *  has completed its initialization.  
 *
 *  @note The notification generated by SAL_Signal_Ready() remains 
 *        in effect until the calling thread is terminated.
 *
 *  @see SAL_Wait_Ready(), SAL_Wait_Destroyed()
 */
void SAL_Signal_Ready(void);

/**
 *  Suspends its caller until all threads in thread_id_list
 *  have called SAL_Signal_Ready().
 *
 *  @param [in] thread_id_list   list of IDs of threads to wait for
 *  @param [in] number_of_items  number of thread IDs on the list
 *
 *  @see SAL_Signal_Ready(), SAL_Wait_Destroyed()
 */
void SAL_Wait_Ready(const SAL_Thread_Id_T thread_id_list[], size_t number_of_items);

/**
 *  Suspends its caller until all threads in thread_id_list
 *  have called SAL_Signal_Ready() or the specified timeout expires.
 *
 *  @param [in] thread_id_list   list of IDs of threads to wait for
 *  @param [in] number_of_items  number of thread IDs on the list
 *  @param [in] timeout_msec     timeout (milliseconds)
 *
 *  @see SAL_Signal_Ready(), SAL_Wait_Destroyed()
 */
bool SAL_Wait_Ready_Timeout(
   const SAL_Thread_Id_T thread_id_list[], 
   size_t number_of_items, 
   uint32_t timeout_msec);

/**
 *  Suspends its caller until all threads in thread_id_list 
 *  have been terminated or the specified timeout expires.
 *
 *  @param [in] thread_id_list   list of IDs of threads to wait for
 *  @param [in] number_of_items  number of thread IDs on the list
 *  @param [in] timeout_msec     timeout (milliseconds)
 *
 *  @see SAL_Signal_Ready(), SAL_Wait_Ready()
 */
bool SAL_Wait_Destroyed_Timeout(
   const SAL_Thread_Id_T thread_id_list[],
   size_t number_of_items,
   uint32_t timeout_msec);

/* -------------------------------------------------------------------------
 *
 *                          XSAL engine
 *
 * -----------------------------------------------------------------------*/
void SAL_Get_Config(SAL_Config_T* sal_config);

bool SAL_Init(const SAL_Config_T* sal_config);

int32_t SAL_Run(void);

void SAL_Deinit(void);

SAL_App_Id_T  SAL_Get_App_Id(void);

size_t SAL_Get_Max_Number_Of_Threads(void);

void SAL_Exit(int32_t status);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_H */

