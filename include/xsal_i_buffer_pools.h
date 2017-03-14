#ifndef XSAL_I_BUFFER_POOL_H
#define XSAL_I_BUFFER_POOL_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_i_config.h"

/** Buffer pool header
 */
typedef struct SAL_I_Buffer_Pool_Header_Tag
{
   /** Pointer to the buffer in given pool
    */
   uint8_t* buffer;

   /** Number of buffers in given pool
    */
   size_t max_number_of_buffers;

   /** size of single buffer (in bytes)
    */
   size_t buffer_size;

   /** Identifier of the first free buffer in given pool
    */
   int32_t first_free_buffer_id;

   /** Pool synchronization mutex
    */
   SAL_Mutex_T pool_mutex;

} SAL_I_Buffer_Pool_Header_T;

/** Pointer to the table with buffer pool headers
 */
extern SAL_I_Buffer_Pool_Header_T* SAL_I_Buffer_Pools_Tab;

/** Synchronize access to the Buffer_Pool_Tab
 */
extern SAL_Mutex_T  SAL_I_Buffer_Pools_Mutex;

/* ----------------------------------------------------------------------
 *  Private Inline Function Prototypes and #define Function-Like Macros
 * ---------------------------------------------------------------------- */
#define Get_Pool_Id(buffer_id)            ((buffer_id) & 0xffffu)
#define Get_Buffer_Index(buffer_id)       ((buffer_id) >> 16)
#define Get_Buffer_Id(pool_id, buffer_id) ((buffer_id << 16) | pool_id)

bool SAL_I_Init_Buffer_Pools(void);

void SAL_I_Deinit_Buffer_Pools(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_BUFFER_POOL_H */

