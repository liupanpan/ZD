#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>

#include "include/xsal.h"

#define APP_ID 2

#define MAX_NUMB_OF_THREADS 4

typedef enum Thread_Id_Tag
{
   SENDER_THREAD = 1,
   REC_THREAD_1,
   REC_THREAD_2,
   BUFFER_THREAD
} Thread_Id_T;

typedef enum Event_Id_Tag
{
   EV_E1 = 1,
   EV_E2,
   EV_E3,
   EV_E4,
   EV_E5,
   EV_E6,
   EV_E7,
   EV_E8,
   EV_E9, 
   EV_ERROR
} Event_Id_T;

typedef struct Sample_Msg_Tag_1
{
   SAL_Int_T field_1;
   SAL_Int_T field_2;
} Sample_Msg_T_1;

typedef struct Sample_Msg_Tag_2
{
   SAL_Int_T field_1;
   SAL_Int_T field_2;
   SAL_Int_T field_3;
} Sample_Msg_T_2;

typedef struct Param_List_Tag
{
   SAL_Event_Id_T* send_event_list;
   size_t send_event_list_size;
   SAL_Event_Id_T* rec_event_list;
   size_t rec_event_list_size;
   SAL_Int_T rec_event_list_number;
   SAL_Int_T test_nr;
   SAL_Int_T test_case_nr;
} Param_List_T;

SAL_Mutex_T Mutex;

size_t Message_Count, Peak_Message_Count;
size_t Peak_Message_Size;

SAL_Int_T Long_Timeout = 3000;

SAL_Event_Id_T Send_Event_List_Test_Case_1[] = { EV_E1, EV_E2, EV_E3, EV_E4, 
                                             EV_E1, EV_E2, EV_E3, EV_E4 };

SAL_Event_Id_T Send_Event_List_Test_Case_2[] = { EV_E1, EV_E2, EV_E3, EV_E4, 
                                             EV_E5, EV_E6, EV_E7, EV_E8 };

SAL_Event_Id_T Rec_Event_List_Test_Case_1[] = { EV_E1, EV_E4 };
SAL_Event_Id_T Rec_Event_List_Test_Case_2[] = { EV_E1, EV_E3, EV_E6, EV_E8 };

const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, 
                                          SAL_Thread_Id_T id, 
                                          SAL_Priority_T prior, 
                                          SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}


/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - receives messages
 *   - checks if data of received messages are valid
 *   - checks if fields of SAL_Stat_Queue_T are valid
 *   - sends message to REC_THREAD_2 (only for XSAL)
 */

void Receive_Only_Thread_2(void* param)
{
   SAL_Thread_Id_T wait_list[] = { SENDER_THREAD };
   const SAL_Message_T* msg;
   Sample_Msg_T_1* data_1;
   Sample_Msg_T_2* data_2;
   SAL_Stat_Queue_T stat;
   SAL_Int_T counter = 0;
   Param_List_T* param_list = (Param_List_T*)param;
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;

   printf("Receive_Only_Thread_2 %d: Start\n", SAL_Get_Thread_Id());

   /* create the queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      SAL_Exit(1);
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   /* wait until SENDER_THREAD sends messages */
   /* LONG TIMEOUT */
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
              Long_Timeout))
   {
         SAL_Exit(1);
   }

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   do
   {
      ftime(&time_1);
       
      /* receive selected messages */
      msg = SAL_Receive_Only_Timeout(param_list->rec_event_list, 
               param_list->rec_event_list_size, 
               Long_Timeout);

      ftime(&time_2);

      delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 
                                    + time_2.millitm - time_1.millitm);

      if (msg != NULL)
      {
         if (delta_time > 3)
         {
            SAL_Exit(1);
         }
      }
      else
      {
         SAL_Exit(1);
      }

      if (msg != NULL)
      {
         if (msg->data_size == sizeof(Sample_Msg_T_1))
         {
            data_1 = (Sample_Msg_T_1*)msg->data;
      
            printf("Receive_Only_Thread %d: field_1: %d, field_2: %d\n", 
                                   SAL_Get_Thread_Id(), data_1->field_1, data_1->field_2);
         }
         else /* msg->data_size == sizeof(Sample_Msg_T_2) */
         {
            data_2 = (Sample_Msg_T_2*)msg->data;

            printf("Receive_Only_Thread %d: field_1: %d, field_2: %d, field_3:%d\n", 
                  SAL_Get_Thread_Id(), data_2->field_1, data_2->field_2, data_2->field_3);
         }
      }
 
      counter++;
  
   }
   while (counter < param_list->rec_event_list_number);

   Message_Count -= param_list->rec_event_list_number;

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   /* check message_count */
   if (stat.message_count != Message_Count)
   {
      SAL_Exit(1);
   }

   /* check peak_message_count */
   if (stat.peak_message_count != Peak_Message_Count)
   {
      SAL_Exit(1);
   }

   /* check peak_message_size */
   if (stat.peak_message_size != Peak_Message_Size)
   {
      SAL_Exit(1);
   }

#if defined (XSAL)

   /* send message to REC_THREAD_2 */
   SAL_Send(APP_ID, REC_THREAD_2, EV_E1, NULL, 0);

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Long_Timeout))
   {
      SAL_Exit(1);
   }

#endif

   printf("Receive_Only_Thread_2 %d: Stop\n", SAL_Get_Thread_Id());
} 

/** \}
 */



/**  \name Functions of the third Receive_Only_Test
 *
 *   \{
 */

/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - try to receive message
 *   - checks if timeout is valid
 */

void Receive_Only_Thread_3(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD };
   const SAL_Message_T* msg;
   Param_List_T* param_list = (Param_List_T*)param;
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;

   printf("Receive_Only_Thread_3 %d: Start\n", SAL_Get_Thread_Id());

   /* create the queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      SAL_Exit(1);
   }

   ftime(&time_1);

   /* try to receive message */
   msg = SAL_Receive_Only_Timeout(param_list->rec_event_list, 
                  param_list->rec_event_list_size, 
                  100);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 
                           + time_2.millitm - time_1.millitm);

   if (msg != NULL)
   {
      SAL_Exit(1);
   }

   /* check if timeout is valid */
   if ((delta_time < 100 - 7) && (delta_time > 100 + 15))
   {
      SAL_Exit(1);
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
            sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
            Long_Timeout))
   {
      SAL_Exit(1);
   }

   printf("Receive_Only_Thread_3 %d: Stop\n", SAL_Get_Thread_Id());

}

/**  Function of SENDER_THREAD
 *
 *   - waits for REC_THREAD_1 thread
 */

void Sender_Thread_3(void* param)
{
   SAL_Thread_Id_T wait_list[] = { REC_THREAD_1 };

   printf("Sender_Thread_3: Start\n");

   /* wait for REC_THREAD_1 */
   if (!SAL_Wait_Ready_Timeout(wait_list, 
              sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
              Long_Timeout))
   {
         SAL_Exit(1);
   }

   printf("Sender_Thread_3: Stop\n");

}

/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - receives messages
 *   - checks if data of received messages are valid
 *   - checks if fields of SAL_Stat_Queue_T are valid
 *   - sends message to REC_THREAD_2 (only for XSAL)
 */

void Receive_Only_Thread_5(void* param)
{
#if defined (XSAL)
   SAL_Thread_Id_T destroyed_list[] = { REC_THREAD_2 };
#endif
   const SAL_Message_T* msg;
   Sample_Msg_T_1* data_1;
   Sample_Msg_T_2* data_2;
   SAL_Stat_Queue_T stat;
   SAL_Int_T counter = 0;
   Param_List_T* param_list = (Param_List_T*)param;
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;

   printf("Receive_Only_Thread_5 %d: Start\n", SAL_Get_Thread_Id());

   /* create queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      SAL_Exit(1);
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   do
   {
      ftime(&time_1);
      
      /* receive selected messages */
      msg = SAL_Receive_Only_Timeout(param_list->rec_event_list, 
                  param_list->rec_event_list_size, 
                  Long_Timeout);

      ftime(&time_2);

      delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 
                                    + time_2.millitm - time_1.millitm);

      if (msg != NULL)
      {
         if (delta_time > 3)
         {
         }
      }
      else
      {
         SAL_Exit(1);
      }
  
      if (msg != NULL)
      { 
         if (msg->data_size == sizeof(Sample_Msg_T_1))
         {
            data_1 = (Sample_Msg_T_1*)msg->data;
                
         }
         else /* msg->data_size == sizeof(Sample_Msg_T_2) */
         {
            data_2 = (Sample_Msg_T_2*)msg->data;
         }
      }         

      counter++;
   
   }  
   while (counter < param_list->rec_event_list_number);

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   /* check message_count */
   if (stat.message_count > (Peak_Message_Count - param_list->rec_event_list_number))
   {
      SAL_Exit(1);
   }

   /* check peak_message_count */
   if (stat.peak_message_count > Peak_Message_Count)
   {
      SAL_Exit(1);
   }

   /* check peak_message_size */
   if (stat.peak_message_size != Peak_Message_Size)
   {
      SAL_Exit(1);
   }

#if defined (XSAL)

   /* send message to REC_THREAD_2 */
   SAL_Send(APP_ID, REC_THREAD_2, EV_E1, NULL, 0);

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      SAL_Exit(1);
   }

#endif

   printf("Receive_Only_Thread_5 %d: Stop\n", SAL_Get_Thread_Id());

} 

/** \}
 */

/**  \name Other Functions
 *   \{
 */

/**  Function of SENDER_THREAD for the fourth and the fifth Receive_Only_Test
 *
 *   - sends messages
 */

void Sender_Thread_First(void* param)
{
   SAL_Thread_Id_T wait_list[] = { REC_THREAD_1 };
   SAL_Thread_Id_T destroyed_list[] = { REC_THREAD_1 };
   Sample_Msg_T_1 data_1, data_3;
   Sample_Msg_T_2 data_2, data_4;
   Param_List_T* param_list = (Param_List_T*)param;

   printf("Sender_Thread_First: Start\n");

   /* wait until REC_THREAD_1 creates queue */
#if defined (XSAL)

   if (param_list->test_nr == 1)
   {
      SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));
   }

#endif

   if (param_list->test_nr == 2)
   {
      if (!SAL_Wait_Ready_Timeout(wait_list, 
              sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
              Long_Timeout))
      {
         SAL_Exit(1);
      }
   }

   data_1.field_1 = 123;
   data_1.field_2 = 234;

   data_2.field_1 = 234;
   data_2.field_2 = 345;
   data_2.field_3 = 456;

   data_3.field_1 = 345;
   data_3.field_2 = 456;

   data_4.field_1 = 456;
   data_4.field_2 = 567;
   data_4.field_3 = 678;

   /* send messages */
   SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));

   if (param_list->test_case_nr == 1)
   {
      SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));
   }
   else
   {
      SAL_Send(APP_ID, REC_THREAD_1, EV_E5, &data_1, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E6, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E7, &data_3, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E8, &data_4, sizeof(Sample_Msg_T_2));
   }

   SAL_Send(APP_ID, REC_THREAD_1, EV_E9, NULL, 0);

   Peak_Message_Count = Message_Count = 9;
   Peak_Message_Size = sizeof(data_2);

   /* signals that all messages are in the queue */
   SAL_Signal_Ready();

#if defined (XSAL)

   if (param_list->test_nr == 1)
   {
      SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
   }

#endif

   if (param_list->test_nr == 2)
   {
      if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Long_Timeout))
      {
         SAL_Exit(1);
      }
   }

   printf("Sender_Thread_First: Stop\n");

}


/**  Function of SENDER_THREAD for the fourth and the fifth Receive_Only_Test
 *
 *   - sends messages
 */

void Sender_Thread_Second(void* param)
{
   SAL_Thread_Id_T wait_list[] = { REC_THREAD_1 };
   Sample_Msg_T_1 data_1, data_3;
   Sample_Msg_T_2 data_2, data_4;
   Param_List_T* param_list = (Param_List_T*)param;

   printf("Sender_Thread_Second: Start\n");

   /* wait until REC_THREAD_1 creates queue */
#if defined (XSAL)

   if (param_list->test_nr == 4)
   {
      SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));
   }

#endif

   if (param_list->test_nr == 5)
   {
      if (!SAL_Wait_Ready_Timeout(wait_list, 
           sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
           Long_Timeout))
      {
         SAL_Exit(1);
      }
   }

   data_1.field_1 = 123;
   data_1.field_2 = 234;

   data_2.field_1 = 234;
   data_2.field_2 = 345;
   data_2.field_3 = 456;

   data_3.field_1 = 345;
   data_3.field_2 = 456;

   data_4.field_1 = 456;
   data_4.field_2 = 567;
   data_4.field_3 = 678;

   Peak_Message_Count = 9;
   Peak_Message_Size = sizeof(data_2);

   /* send messages */
   SAL_Sleep(500);

   SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));

   SAL_Sleep(500);

   SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));

   if (param_list->test_case_nr == 1)
   {

      SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));
   }
   else
   {
      SAL_Send(APP_ID, REC_THREAD_1, EV_E5, &data_1, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E6, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E7, &data_3, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E8, &data_4, sizeof(Sample_Msg_T_2));
   }

   SAL_Send(APP_ID, REC_THREAD_1, EV_E9, NULL, 0);

   printf("Sender_Thread_Second: Stop\n");

}


/**  Function of test case.
 *
 *   Function creates four threads: SENDER_THREAD, RECEIVE_FROM_THREAD1, RECEIVE_FROM_THREAD2, QUEUE_THREAD
 *   and starts SAL_Run.
 */

SAL_Int_T Receive_Only_Test(void* sender_fun, void* receiver_fun, void* param)
{
   SAL_Thread_Id_T destroyed_list_2[] = { SENDER_THREAD, REC_THREAD_1 };
   SAL_Thread_Attr_T thread_attr;
   SAL_Mutex_Attr_T mutex_attr;
   SAL_Int_T ret = 0;
   SAL_Priority_T priority;
   
   SAL_Init_Mutex_Attr(&mutex_attr);

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      SAL_Exit(1);
   }

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      receiver_fun,
      param,
      Init_Thread_Attr("REC1", REC_THREAD_1, priority, &thread_attr));

#if defined (XSAL)

   if (param_list->test_nr != 3)
   {

      SAL_Create_Thread(
         Receive_From_Thread,
         param,
         Init_Thread_Attr("REC2", REC_THREAD_2, priority, &thread_attr));
   }

#endif

   SAL_Create_Thread(
      sender_fun, 
      param,
      Init_Thread_Attr("SENDER", SENDER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   if (param_list->test_nr != 3)
   {
      SAL_Wait_Destroyed(destroyed_list_1, sizeof(destroyed_list_1)/sizeof(SAL_Thread_Id_T));
   }
   else
   {
      SAL_Wait_Destroyed(destroyed_list_2, sizeof(destroyed_list_2)/sizeof(SAL_Thread_Id_T));
   }

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list_2, 
           sizeof(destroyed_list_2)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      ret = 1;
   }

#endif

   SAL_Destroy_Mutex(&Mutex);

   return ret;
}

/** \}
 */

SAL_Int_T Set_Up()
{
   SAL_Config_T config;

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&config))
   {
      return 1;
   }

   return 0;
}


void Tear_Down()
{
}


SAL_Int_T main (SAL_Int_T argc, char** argv)
{
   SAL_Int_T ret = 0;
   Param_List_T param_list;

   if (Set_Up())
   {
      return 1;
   }

   /* FIRST SENT, THEN RECEIVED */

#if defined (XSAL)

   /* without timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 1;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_1, (void*)&param_list);   

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 1;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_1, (void*)&param_list);   

#endif

   /* with long timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 2;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_2, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 2;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_2, &param_list);

   /* with short timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 3;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_3, Receive_Only_Thread_3, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 3;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_3, Receive_Only_Thread_3, &param_list);

   /* FIRST WAITING FOR RECEIVING, THEN SENT */

#if defined (XSAL)

   /* without timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 4;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_4, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 4;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_4, &param_list);

#endif

   /* with long timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 5;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_5, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 5;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_5, &param_list);

   Tear_Down();

   printf("Test %s\n", ((ret)?("FAILED"):("PASSED")));

   return ret;

}
