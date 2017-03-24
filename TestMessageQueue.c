#include <stdio.h>
#include <stdlib.h>

#include "include/xsal.h"

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 2

#define SENDER_THREAD 1
#define RECEIVER_THREAD 2
#define ITERATOR_THREAD 1
#define PURGE_THREAD    1

typedef enum Event_Id_Tag {
   EV_E1 = 1,
   EV_E2,
   EV_E3,
   EV_E4,
   EV_E5,
   EV_E6
} Event_Id_T;

typedef struct Sample_Msg_Tag1
{
   SAL_Int_T field1;
   SAL_Int_T field2;
} Sample_Msg_T1;

typedef struct Sample_Msg_Tag2
{
   SAL_Int_T field1;
   SAL_Int_T field2;
   SAL_Int_T field3;
} Sample_Msg_T2;

typedef struct Sizes_Tag
{
  size_t alloc_size;
  size_t max_buffer_size;
} Sizes_T;

Sizes_T Size;

size_t Queue_Size;
size_t Max_Message_Count, Message_Count;

SAL_Int_T Timeout = 3000;

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

/**  Function of RECEIVER_THREAD.
 *
 *   - creates the queue, receives messages and checks SAL_Stat_Queue_T fields
 */

void Receive_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { SENDER_THREAD };
   SAL_Stat_Queue_T stat;
   const SAL_Message_T* msg;
   Sizes_T size = *(Sizes_T*)param;

   printf("Receive_Fun: Start\n");

   Queue_Size = 5;

   if (!SAL_Create_Queue(Queue_Size, size.alloc_size, malloc, free))
   {
      SAL_Exit(1);
   }

   SAL_Signal_Ready();

   if (!SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat))
   {
      SAL_Exit(1);
   }

   if (stat.queue_size != Queue_Size)
   {
      SAL_Exit(1);
   }

   if (stat.buffer_size != size.alloc_size)
   {
      SAL_Exit(1);
   }

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      SAL_Exit(1);
   }

#endif

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif

   Message_Count--;

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif

   Message_Count--;

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

   if (stat.peak_message_count != Max_Message_Count)
   {
      SAL_Exit(1);
   }

   if (stat.message_count != Message_Count)
   {
      SAL_Exit(1);
   }

   if (stat.peak_message_size != size.max_buffer_size)
   {
      SAL_Exit(1);
   }

   printf("Receive_Fun: Stop\n");
}



/**  Function of SENDER_THREAD.  
 *
 *   - sends messages to the RECEIVER_THREAD
 */

void Send_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { RECEIVER_THREAD };
   SAL_Thread_Id_T destroyed_list [] = { RECEIVER_THREAD };
   Sample_Msg_T1* data1;
   Sample_Msg_T2* data2;
   size_t i;
   size_t size_of_1, size_of_2;
   SAL_Int_T data_bool = (SAL_Int_T)param;

   printf("Send_Fun: Start\n");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      SAL_Exit(1);
   }
 
#endif

   Max_Message_Count = Message_Count = 5;

   /* sending data */
   if (data_bool)
   {
      data1 = (Sample_Msg_T1*)malloc(sizeof(Sample_Msg_T1));
      data2 = (Sample_Msg_T2*)malloc(sizeof(Sample_Msg_T2));

      data1->field1 = 123;
      data1->field2 = 456;

      data2->field1 = 123;
      data2->field2 = 456;
      data2->field3 = 789;

      size_of_1 = sizeof(Sample_Msg_T1);
      size_of_2 = sizeof(Sample_Msg_T2);
   }
   /* sending no data */
   else
   {
      data1 = NULL;   
      data2 = NULL;      
      size_of_1 = size_of_2 = 0;
   }

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data1, size_of_1);

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data2, size_of_2);

   for (i = 0; i < Message_Count - 2; i++)
   {
      SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data1, size_of_1);
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      SAL_Exit(1);
   }

#endif

   printf("Send_Fun: Stop\n");
}


/**  Function tests SAL_Create_Queue function and SAL_Stat_Message_Queue struct.
 *
 *   Creates threads and starts SAL_Run.
 */

SAL_Int_T Queue_Test(Sizes_T size)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, RECEIVER_THREAD };
   SAL_Int_T ret;
   SAL_Int_T param;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   /* sending data */
   if (size.max_buffer_size > 0)
   {
      param = 1;
   }
   /* sending no data */
   else
   {
      param = 0;
   }

   SAL_Create_Thread(
      Send_Fun, 
      (void*)param, 
      Init_Thread_Attr("SENDER", SENDER_THREAD, priority, &thread_attr));

    SAL_Create_Thread(
       Receive_Fun, 
       (void*)&size, 
       Init_Thread_Attr("RECEIVER", RECEIVER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      ret = 1;
   }

#endif

   return ret;
}

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


SAL_Int_T main(SAL_Int_T argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up())
   {
      return 1;
   }
  
#if defined (XSAL)

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_For_Thread_Test(&Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_For_Thread_Test(&Size);

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_For_Thread_Test(&Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_For_Thread_Test(&Size);

#endif

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_Test(Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_Test(Size);

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_Test(Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_Test(Size);

#if defined (XSAL)
   ret = ret || Iterator_Test();
   ret = ret || SAL_Purge_Test_Suite();
#endif

   Tear_Down();

   printf("Test %s\n", ((ret)?("FAILED"):("PASSED")));

   return ret;	
}
