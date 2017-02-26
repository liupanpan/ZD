#ifndef XSAL_I_EVENT_PROPERTY_H
#define XSAL_I_EVENT_PROPERTY_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"

extern uint8_t (*SAL_I_Event_Property)[SAL_MAX_EVENTS];

/** The following bitmask maps propertirs to single bits.
 */
#define SAL_I_Subscribe_Bit_Mask  0x01
#define SAL_I_Urgent_Bit_Mask     0x02
#define SAL_I_Select_Bit_Mask     0x04

/** Function returns true if given thread is subscribed for given event.
 *
 *  @param [in] tid id of examined thread
 *  @param [in] ev_id id of examined event
 *
 *  @return true when thread is subscribed for event.
 */
#define SAL_I_Is_Subscribed(tid, ev_id) ((SAL_I_Event_Property[(tid)-1][(ev_id)] & SAL_I_Subscribe_Bit_Mask) != 0)



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_EVENT_PROPERTY_H */

