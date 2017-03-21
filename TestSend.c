#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "include/xsal.h"

#define APP_TEST_LOCAL    2
#define APP_TEST_SENDER   2
#define APP_TEST_RECEIVER 3

#define EV_E6 6
#define EV_ERROR 7

/* Thread ids */
typedef enum Thread_Id_Tag
{
   RECEIVER_1 = 1,
   SENDER_1, 

   RECEIVER_2,
   SENDER_2, 

   RECEIVER_3,
   SENDER_3, 

   RECEIVER_4,
   SENDER_4, 

   LAST_THREAD_ID = SENDER_4
} Thread_Id_T;


typedef enum Thread_Ctrl_Id_Tag
{
   S_CTRL_1 = 1,
   R_CTRL_1, 

   S_CTRL_2,
   R_CTRL_2, 

   S_CTRL_3,
   R_CTRL_3, 

   S_CTRL_4,
   R_CTRL_4
} Thread_Ctrl_Id_T;


/* Control messages */
typedef enum E_Ready_Tag
{
   EV_REMOTE_SENDER_READY = 1,
   EV_REMOTE_RECEIVER_READY 
} E_Ready_T;


typedef enum E_Test_Type_Tag
{
   E_STAND_ALONE, 
   E_SENDER, 
   E_RECEIVER 
} E_Test_Type_T;

typedef struct Ev_Test_Case_Data_Tag
{
   SAL_Event_Id_T event_id;
   size_t data_size;
   void* data;
} Ev_Test_Case_Data_T;

typedef struct Sample_Msg_Tag
{
   SAL_Int_T field1;
   SAL_Int_T field2;
} Sample_Msg_T;

struct timeb Time;

SAL_Semaphore_T Sem_Rec  [LAST_THREAD_ID + 1];
SAL_Semaphore_T Sem_Send [LAST_THREAD_ID + 1];

E_Test_Type_T Test_Type = E_STAND_ALONE;

SAL_Mutex_T Queue_Dump_Mutex;

SAL_Int_T Timeout = 3000;

/*
 *  Test Case 1:
 *
 *  Only normal (not urgent) events without data
 */
const Ev_Test_Case_Data_T Ev_Send_TC_1[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL }
};


const Ev_Test_Case_Data_T Ev_Receive_TC_1[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL }
};

/*
 *  Test Case 3:
 *
 *  Only normal (not urgent) events with data
 */
const Ev_Test_Case_Data_T Ev_Send_TC_3[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" }
};

const Ev_Test_Case_Data_T Ev_Receive_TC_3[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" }
};

/*
 *  Test Case 5:
 *
 *  Only normal (not urgent) events without data.
 *  Send more events then size of message queue
 */
const Ev_Test_Case_Data_T Ev_Send_TC_5[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL },
   { 5, 0, NULL }
};


const Ev_Test_Case_Data_T Ev_Receive_TC_5[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL }
};

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

void Dump_Queue_Stat(SAL_Thread_Id_T queue_id, SAL_Stat_Queue_T* stat)
{
   printf(
      "Queue %d Stat:\n"
      "\tqueue_size: %d\n"
      "\tbuffer_size: %d\n"
      "\tmessage_count: %d\n"
      "\tpeak_message_count: %d\n"
      "\tpeak_message_size: %d", 
      queue_id, 
      stat->queue_size,
      stat->buffer_size, 
      stat->message_count, 
      stat->peak_message_count,
      stat->peak_message_size);
}

SAL_Int_T Check_Events(const Ev_Test_Case_Data_T* event_list, size_t event_list_size)
{
   size_t i;
   SAL_Stat_Queue_T stat_queue;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat_queue);

   /* check if the number of messages in the queue is valid */
   if (stat_queue.message_count != event_list_size)
   {
      printf("Check_Events: Invalid message count in the queue %d %d", 
                                          stat_queue.message_count, event_list_size);
      Dump_Queue_Stat(SAL_Get_Thread_Id(), &stat_queue);

      return 1;
   }

   /* receive message and check if all data are valid */
   for(i = 0; i < event_list_size; i++)
   {
      const SAL_Message_T* msg;

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         printf("SAL_Receive_Timeout: timeout has been reached");
         SAL_Exit(1);
      }
      printf("Ev %d, Size %d, From(%d)", msg->event_id, msg->data_size, msg->sender_thread_id);

      if (msg->event_id != event_list[i].event_id  ||
         msg->data_size != event_list[i].data_size ||
         memcmp(msg->data, event_list[i].data, msg->data_size) != 0 ||
         msg->sender_thread_id != thread_id + 1 ||
         msg->receiver_thread_id != thread_id)
      {
         SAL_Stat_Queue_T stat;

         printf("Check_Events: expected %d, received %d", event_list[i].event_id, msg->event_id);

         SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);
         Dump_Queue_Stat(SAL_Get_Thread_Id(), &stat);

         return 1;
      }
   }

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat_queue);

   /* check if queue is empty */
   if (stat_queue.message_count != 0)
   {
      printf("Check_Events: queue is not empty");
      return 1;
   }
   return 0;
}

/** Function of receiver, test case.
 *  
 *  - uses semaphores for communication synchronization
 *  - receives and checks events
 */

SAL_Int_T Receive_Test(SAL_Int_T i, 
                       const Ev_Test_Case_Data_T* event_list, 
                       size_t event_list_size)
{
   SAL_Int_T ret;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

   /* receiver is ready */
   SAL_Signal_Semaphore(&Sem_Rec[thread_id]);

   /* wait until sender sends messages */
   if (!SAL_Wait_Semaphore_Timeout(&Sem_Send[thread_id+1], Timeout))
   {
      printf("Timeout has been reached");
      SAL_Exit(1);
   }

   ret = Check_Events(event_list, event_list_size);

   printf("Receive_Test %d: %d", i, ret);

   return ret;
}

/**  Function of sender.
 *
 *   - sends list of messages given as an entry parameter
 */

SAL_Int_T Send_Events(SAL_App_Id_T app_id, 
                      SAL_Thread_Id_T thread_id, 
                      const Ev_Test_Case_Data_T* event_list, 
                      size_t event_list_size)
{
   size_t i;

   for(i = 0; i < event_list_size; i++)
   {
      if (!SAL_Send(app_id, thread_id, event_list[i].event_id, event_list[i].data, event_list[i].data_size))
      {
         printf("Send_Events FAILED\n");
         SAL_Exit(1);
      }
   }
   return 0;
}

/**  Function of sender, test case.
 *
 *   - uses semaphores for communication synchronization
 *   - sends the list of messages
 */

SAL_Int_T Send_Test(SAL_Int_T i, 
                    SAL_App_Id_T app_id, 
                    SAL_Thread_Id_T receiver_thread_id, 
                    const Ev_Test_Case_Data_T* event_list, 
                    size_t event_list_size)
{
   SAL_Int_T ret;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

   /* wait until receiver thread will be ready */
   if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[receiver_thread_id], Timeout))
   {
      printf("Timeout has been reached");
      SAL_Exit(1);
   }

   /* send test messages */
   ret = Send_Events(app_id, receiver_thread_id, event_list, event_list_size);

   /* signal that all test messages were send */
   SAL_Signal_Semaphore(&Sem_Send[thread_id]);

   printf("Send_Test %d: %d", i, ret);

   return ret;
}

/**  Function of sender, test case 5.
 *
 *   - uses semaphores for communication synchronization
 *   - sends the list of messages
 */

SAL_Int_T Send_Test_5(SAL_App_Id_T app_id, SAL_Thread_Id_T receiver_thread_id)
{
   SAL_Int_T i;
   SAL_Int_T ret = 0;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();
   SAL_Int_T event_list_size = sizeof(Ev_Send_TC_5)/sizeof(Ev_Test_Case_Data_T);

   /* wait until receiver thread will be ready */
   if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[receiver_thread_id], Timeout))
   {
      printf("Timeout has been reached");
      SAL_Exit(1);
   }

   /* send test messages */
   /* sends 5 messages (more then receiver queue size) */
   for(i = 0; i < event_list_size-1; i++)
   {
      if (!SAL_Send(app_id, receiver_thread_id, Ev_Send_TC_5[i].event_id, Ev_Send_TC_5[i].data, Ev_Send_TC_5[i].data_size))
      {
         printf("Send_Test_5 FAILED\n");
         ret = 1;
      }
   }
   if (SAL_Send(app_id, receiver_thread_id, Ev_Send_TC_5[i].event_id, Ev_Send_TC_5[i].data, Ev_Send_TC_5[i].data_size))
   {
      printf("Send_Test_5 FAILED\n");
      ret = 1;
   }

   /* signal that all test messages were send */
   SAL_Signal_Semaphore(&Sem_Send[thread_id]);

   printf("Send_Test_5 %d", ret);

   return ret;
}

/**  Function of RECEIVER_\a n for local receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */
void Local_Receiver_Test(void* p)
{
   SAL_Int_T ret = 0;
   size_t message_size = (size_t)p;

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      printf("Receiver_Fnc: SAL_Create_Queue error\n");
      SAL_Exit(1);
   }

   ret = ret || Receive_Test(1, Ev_Receive_TC_1, 
                sizeof(Ev_Receive_TC_1)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Receive_Test(3, Ev_Receive_TC_3, 
                sizeof(Ev_Receive_TC_3)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Receive_Test(5, Ev_Receive_TC_5, 
                sizeof(Ev_Receive_TC_5)/sizeof(Ev_Test_Case_Data_T));

   if (ret)
   {
      SAL_Exit(1);
   }
}

/**  Function of SENDER_\a n for local sender thread.
 *
 *   - sends messages from the list of all test cases
 */
void Local_Sender_Test(void* param)
{
   SAL_Thread_Id_T receiver_id = (SAL_Thread_Id_T)param;
   SAL_Int_T ret = 0;
   SAL_App_Id_T app_id = SAL_Get_App_Id();

   ret = ret || Send_Test(1, app_id, receiver_id, Ev_Send_TC_1, sizeof(Ev_Send_TC_1)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Send_Test(3, app_id, receiver_id, Ev_Send_TC_3, sizeof(Ev_Send_TC_3)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Send_Test_5(app_id, receiver_id);

   if (ret)
   {
      SAL_Exit(1);
   }
}

/**  Test of SAL_Send function in local or remote communication. 
 *   
 *   Urgent messages are also tested.
 *   Creates eight threads and starts SAL_Run.
 */

SAL_Int_T Send_Test_Suite()
{
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_1, SENDER_1 };
   SAL_Int_T retcode;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   switch(Test_Type)
   {
   
   default:
      /* local communication */

      /* Receiver1 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         NULL, 
         Init_Thread_Attr("LRecD1", RECEIVER_1, priority, &thread_attr));

      /* Sender1 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_1, 
         Init_Thread_Attr("LSndD1", SENDER_1, priority, &thread_attr));

      /* Receiver2 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         (void*)4, 
         Init_Thread_Attr("LRecD2", RECEIVER_2, priority, &thread_attr));

      /* Sender2 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_2, 
         Init_Thread_Attr("LSndD2", SENDER_2, priority, &thread_attr));
      
      /* Receiver3 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         (void*)5, 
         Init_Thread_Attr("LRecD3", RECEIVER_3, priority, &thread_attr));

      /* Sender3 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_3, 
         Init_Thread_Attr("LSndD3", SENDER_3, priority, &thread_attr));

      /* Receiver4 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         (void*)512, 
         Init_Thread_Attr("LRecD4", RECEIVER_4, priority, &thread_attr));

      /* Sender4 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_4, 
         Init_Thread_Attr("LSndD4", SENDER_4, priority, &thread_attr));

   }

   retcode = SAL_Run();
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      printf("Timeout has been reached");
      retcode = 1;
   }

   if (retcode != 0)
   {
      printf("Send_Test: SAL_Run() return %d\n", retcode);
      return 1;
   }

   return 0;
}

/**  Function of SENDER_1.
 *
 *   - sends message with timestamp
 */
void Sender_Fun(void* param)
{
   Sample_Msg_T data;
   SAL_Thread_Id_T wait_list[] = { RECEIVER_1 };

   printf("Sender_Fun: Start");
   
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      printf("Timeout has been reached");
      SAL_Exit(1);
   }

   data.field1 = 123;
   data.field2 = 456;

   ftime(&Time);
 
   if (!SAL_Send(APP_TEST_LOCAL, RECEIVER_1, EV_E6, &data, sizeof(Sample_Msg_T)))
   {
      printf("SAL_Send: The message has not been sent");
      SAL_Exit(1);
   }

   printf("Sender_Fun: Stop");
}

/**  Function of RECEIVER_1.
 *
 *   - receives message with timestamp and checks if the time is correct
 */
void Receiver_Fun(void* param)
{
   const SAL_Message_T* msg;

   printf("Receiver_Fun %d: Start",SAL_Get_Thread_Id());

   SAL_Create_Queue(1, sizeof(Sample_Msg_T), malloc, free);

   SAL_Signal_Ready();

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      printf("SAL_Receive_Timeout: timeout has been reached");
      SAL_Exit(1);
   }

   if (((msg->timestamp_sec - Time.time)*1000 + msg->timestamp_msec - Time.millitm) > 10)
   {
      printf("SAL_Send - SAL_Receive: Invalid time in header components");
      SAL_Exit(1);
   }

   printf("Receiver_Fun %d: Stop",SAL_Get_Thread_Id());

}

/**  Test of timestamp.
 *
 *   Function creates two threads: SENDER_1 and RECEIVER_1 and starts SAL_Run.
 */
SAL_Int_T Time_Send_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Sender_Fun, 
      NULL, 
      Init_Thread_Attr("SENDER", SENDER_1, priority, &thread_attr));

   SAL_Create_Thread(
      Receiver_Fun, 
      NULL, 
      Init_Thread_Attr("REC_T", RECEIVER_1, priority, &thread_attr));

   return SAL_Run();
}

SAL_Int_T Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_Int_T i;
   SAL_Int_T option;
   SAL_Config_T config;
   SAL_Mutex_Attr_T mutex_attr;
   SAL_Semaphore_Attr_T sem_attr;

   while((option = getopt(argc, argv, "srv:g:l:teN:")) != -1)
   {        
      switch(option) 
      {             
         default:
            break;
      }     
   }

   SAL_Get_Config(&config);

   switch(Test_Type)
   {

   default:

      config.app_id = APP_TEST_LOCAL;
      config.max_number_of_threads = LAST_THREAD_ID + 2;

      if (!SAL_Init(&config))
      {
         return 1;
      }

   }

   SAL_Init_Mutex_Attr(&mutex_attr);

   if (!SAL_Create_Mutex(&Queue_Dump_Mutex, &mutex_attr))
   {
      return 1;
   }

   SAL_Init_Semaphore_Attr(&sem_attr);

   for(i = 1; i <= LAST_THREAD_ID; i++)
   {
      if (!SAL_Create_Semaphore(&Sem_Rec[i], &sem_attr) || 
         !SAL_Create_Semaphore(&Sem_Send[i], &sem_attr))
      {
         return 1;
      }
   }
   return 0;
}


SAL_Int_T Tear_Down()
{
   SAL_Int_T i;

   if (!SAL_Destroy_Mutex(&Queue_Dump_Mutex))
   {
      return 1;
   }

   for(i = 1; i <= LAST_THREAD_ID; i++)
   {
      if (!SAL_Destroy_Semaphore(&Sem_Rec[i]) || 
         !SAL_Destroy_Semaphore(&Sem_Send[i]))
      {
         return 1;
      }
   }

   return 0;
}


SAL_Int_T main (SAL_Int_T argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up(argc, argv))
   {
      return 1;
   }

   ret = ret || Send_Test_Suite();

   /* test of timestamp, only locally */
   if (Test_Type == E_STAND_ALONE)
   {
      ret = ret || Time_Send_Test();
   }

   if (Tear_Down())
   {
      return 1;
   }
   
   printf("Test %s", ((ret)?("FAILED"):("PASSED")));
   
   return ret;
}
