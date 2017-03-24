#include "xsal_i_receive.h"


/* -----------------------
 *  Function Definitions
 * ----------------------- */


SAL_Message_Queue_Node_T* SAL_I_Search_Message_Queue(
   const SAL_Message_Queue_T* q, 
   const SAL_Event_Id_T event_list[],
   size_t number_of_events)
{
   SAL_Message_Queue_Node_T* q_end = q->end_marker;
   SAL_Message_Queue_Node_T* n = q_end->next_node;

   while(n != q_end)
   {
      size_t i;
      SAL_Event_Id_T ev_id = n->message.event_id;

      for(i=0; i < number_of_events; i++)
      {
         if (event_list[i] == ev_id)
         {
            break;
         }
      }
      if (i == number_of_events)
      {
         n = n->next_node;
      }
      else
      {
         break;
      }
   }
   return n;
}
