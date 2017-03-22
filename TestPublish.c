#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "include/xsal.h"
#include "include/xsal_i_thread.h"

/* application ids */
#define APP_TEST_LOCAL    2
#define APP_TEST_PUBLISHER   2
#define APP_TEST_RECEIVER 3

/* control thread numbers */
#define REC_CONTROL 1
#define PUB_CONTROL 6
#define RECEIVER_THREAD 1
#define PUBLISHER_THREAD 2

#define NUMBER_OF_RECEIVERS 4

/* event's numbers */
typedef enum Event_Id_Tag
{
   EV_E9 = 9,
   EV_REC_PASS,
   EV_ERROR
} Event_Id_T;

typedef enum Thread_Id_Tag {
   PUBLISHER = 1,
   RECEIVER_2, 
   RECEIVER_3, 
   RECEIVER_4,
   RECEIVER_5,
} Thread_Id_T;

typedef enum E_Test_Type_Tag
{ 
   E_STAND_ALONE, 
   E_PUBLISHER, 
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

SAL_Semaphore_T Sem_Rec     [2 + 2*NUMBER_OF_RECEIVERS];
SAL_Semaphore_T Sem_Publish [2 + 2*NUMBER_OF_RECEIVERS];

E_Test_Type_T Test_Type = E_STAND_ALONE;
SAL_Int_T App_Id;
SAL_Mutex_T Queue_Dump_Mutex;

SAL_Int_T Timeout = 3000;


/*
 *  Test 1:
 *  
 *  Events with events id from 1 to 8 are published.
 *
 *  Only normal (not urgent) events without data
 */

const Ev_Test_Case_Data_T Ev_Publish_TC_1[] =
{
   { 1, 0, NULL }, 
   { 2, 0, NULL }, 

   { 3, 0, NULL }, 
   { 4, 0, NULL }, 
   { 5, 0, NULL }, 
   { 6, 0, NULL }, 
   { 7, 0, NULL }, 
   { 8, 0, NULL },
};

const SAL_Event_Id_T Ev_Subscribe_TC_1_1[] = { 1, 3, 5, 7 };
const Ev_Test_Case_Data_T Ev_Receive_TC_1_1[] =
{
   { 1, 0, NULL },

   { 3, 0, NULL },
   { 5, 0, NULL },
   { 7, 0, NULL },
};

const SAL_Event_Id_T Ev_Subscribe_TC_1_2[] = { 2, 4, 6, 8 };
const Ev_Test_Case_Data_T Ev_Receive_TC_1_2[] =
{
   { 2, 0, NULL },

   { 4, 0, NULL },
   { 6, 0, NULL },
   { 8, 0, NULL },
};

/*
 *  Test Case 3:
 *
 *  Only normal (not urgent) events with data
 */
const Ev_Test_Case_Data_T Ev_Publish_TC_3[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 5, 1,  "" },
   { 6, 5,  "1234" },
   { 7, 9,  "12345678" },
   { 8, 17, "1234567890123456" }
};

const SAL_Event_Id_T Ev_Subscribe_TC_3_1[] = { 7, 5, 3, 1 };
const Ev_Test_Case_Data_T Ev_Receive_TC_3_1[] =
{
   { 1, 1,  "" },
   { 3, 9,  "12345678" },
   { 5, 1,  "" },
   { 7, 9,  "12345678" }
};


const SAL_Event_Id_T Ev_Subscribe_TC_3_2[] = { 8, 6, 4, 2 };
const Ev_Test_Case_Data_T Ev_Receive_TC_3_2[] =
{
   { 2, 5,  "1234" },
   { 4, 17, "1234567890123456" },
   { 6, 5,  "1234" },
   { 8, 17, "1234567890123456" }
};

/*
 *  Test Case 5:
 *
 *  Only normal (not urgent) events without data.
 *  Subscribes on more events then size of message queue.
 */
const Ev_Test_Case_Data_T Ev_Publish_TC_5[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 5, 1,  "" },
   { 6, 5,  "1234" },
   { 7, 9,  "12345678" },
   { 8, 17, "1234567890123456" }
};

const SAL_Event_Id_T Ev_Subscribe_TC_5_1[] = { 1, 5, 6, 7, 8 };
const Ev_Test_Case_Data_T Ev_Receive_TC_5_1[] =
{
   { 1, 1,  "" },
   { 5, 1,  "" },
   { 6, 5,  "1234" },
   { 7, 9,  "12345678" }
};

const SAL_Event_Id_T Ev_Subscribe_TC_5_2[] = { 2, 3, 4, 7, 8 };
const Ev_Test_Case_Data_T Ev_Receive_TC_5_2[] =
{
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 7, 9,  "12345678" }
};

const char* SAL_Get_Thread_Name_Self(void)
{
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();

   /* Function returns NULL when SAL is not initialized or if 
    * it is called from NON-SAL thread    
    */
   return ((SAL_I_Thread_Table == NULL) || (thread_attr == NULL))?
      NULL : thread_attr->thread_name;
}

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



/**  \name Publish_Test_Suite
 *
 *   \{
 */


/**  Auxiliary function of receiver, writes out SAL_Stat_Queue_T contents.
 */

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


/**  Auxiliary function of receiver, writes out event_ids and data_size of all messages in the queue
 *   (<B><I>XSAL only</I></B>).
 */
void Dump_Queue(const char* pref)
{
#if defined (XSAL)
   SAL_Queue_Iterator_T iter;
   SAL_Thread_Id_T tid = SAL_Get_Thread_Id();

   SAL_First_Message(&iter);

   SAL_Lock_Mutex(&Queue_Dump_Mutex);

   printf("Messages in the queue %d, %s\n", SAL_Get_Thread_Id(), pref);
   while(!SAL_Is_Queue_End(&iter))
   {
      const SAL_Message_T* msg = SAL_Get_Message(&iter);
      SAL_Next_Message(&iter);
   }

   SAL_Unlock_Mutex(&Queue_Dump_Mutex);
#endif /* XSAL */
}


/**  Function of receiver, receives and checks the list of events.
 *
 *   - checks if number of messages in the queue is valid
 *   - receives messages and checks all data of messages
 *   - after receiving messages checks if the queue is empty
 */

SAL_Int_T Check_Events(const Ev_Test_Case_Data_T* event_list, size_t event_list_size)
{
   size_t i;
   SAL_Stat_Queue_T stat_queue;
   const SAL_Message_T* msg;
   
   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat_queue);

   /* check if the number of messages in the queue is valid */
   if (stat_queue.message_count != event_list_size)
   {
      Dump_Queue_Stat(SAL_Get_Thread_Id(), &stat_queue);
      Dump_Queue("Check_Events"); /* !!! */

      return 1;
   }

   /* receive message and check if all data are valid */
   for(i = 0; i < event_list_size; i++)
   {
         msg = SAL_Receive_Timeout(Timeout);

         if (msg == NULL)
         {
            SAL_Exit(1);
         }        
         printf("Ev %d, Size %d, From(%d)\n", msg->event_id, msg->data_size, msg->sender_thread_id);

         if (msg->event_id != event_list[i].event_id  ||
           msg->data_size != event_list[i].data_size ||
           memcmp(msg->data, event_list[i].data, msg->data_size) != 0 ||
           msg->receiver_thread_id != 0 ||
           msg->sender_thread_id != 1)
         {
            SAL_Stat_Queue_T stat;

            SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);
            Dump_Queue_Stat(SAL_Get_Thread_Id(), &stat);
            Dump_Queue("Error");

            return 1;
         }
   }
   
   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat_queue);

   /* check if queue is empty */
   if (stat_queue.message_count != 0)
   {
      return 1;
   }
   return 0;
}


/** Function of receiver, test case.
 *  
 *  - uses semaphores for communication synchronization
 *  - receives and checks events
 */

SAL_Int_T Receive_Test(char* s, 
                       const Ev_Test_Case_Data_T* event_list, 
                       size_t event_list_size,
                       const SAL_Event_Id_T* subscribe_list, 
                       size_t subscribe_list_size
#if defined (XSAL)
                       ,
                       const SAL_Event_Id_T* urgent_list, 
                       size_t urgent_list_size
#endif
                       )
{
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();
   SAL_Int_T ret;

   printf("Receive_Test %s[%s]: Start\n", s, SAL_Get_Thread_Name_Self());

   SAL_Subscribe(subscribe_list, subscribe_list_size);

#if defined (XSAL)
   if (urgent_list_size > 0)
      SAL_Declare_Urgent(urgent_list, urgent_list_size);
#endif

   /* LOCAL RECEIVER */
   if (SAL_Get_App_Id() != APP_TEST_RECEIVER)
   {
      /* receiver is ready */
      SAL_Signal_Semaphore(&Sem_Rec[thread_id]);


      /* wait until publisher publishes messages */
#if defined (XSAL) 
      SAL_Wait_Semaphore(&Sem_Publish[thread_id]);
#else
      if (!SAL_Wait_Semaphore_Timeout(&Sem_Publish[thread_id], Timeout))
      {
         SAL_Exit(1);
      }
#endif

   }
   /* REMOTE RECEIVER */
   else
   {
      /* receiver is ready */
      SAL_Signal_Semaphore(&Sem_Rec[thread_id + NUMBER_OF_RECEIVERS]);

      /* wait until publisher publishes messages */
#if defined (XSAL)
      SAL_Wait_Semaphore(&Sem_Publish[thread_id + NUMBER_OF_RECEIVERS]);
#else
      if (!SAL_Wait_Semaphore_Timeout(&Sem_Publish[thread_id + NUMBER_OF_RECEIVERS], Timeout))
      {
         SAL_Exit(1);
      }
#endif
   }

   ret = Check_Events(event_list, event_list_size);

#if defined (XSAL)
   if (urgent_list_size > 0)
   {
      SAL_Undeclare_Urgent(urgent_list, urgent_list_size);
   }
#endif

   SAL_Unsubscribe(subscribe_list, subscribe_list_size);

   /* REMOTE RECEIVER: receiver sends the info about passing the test
      to the control thread */
   if (Test_Type == E_RECEIVER)
   {
      SAL_Send(SAL_Get_App_Id(), REC_CONTROL, EV_REC_PASS, (void*)&ret, sizeof(SAL_Int_T));
   }

   printf("Receive_Test%s[%s]: Stop, ret = %d\n", s, SAL_Get_Thread_Name_Self(), ret);

   return ret;
}


/**  Function of publisher.
 *
 *   - publishes list of messages given as an entry parameter
 */

void Publish_Events(const Ev_Test_Case_Data_T* event_list, size_t event_list_size)
{
   size_t i;

   for(i = 0; i < event_list_size; i++)
   {
      SAL_Publish(event_list[i].event_id, event_list[i].data, event_list[i].data_size);
   }

}


/**  Function of publisher, test case.
 *
 *   - uses semaphores for communication synchronization
 *   - publishes the list of messages
 */

SAL_Int_T Publish_Test(SAL_Int_T j, 
                       const Ev_Test_Case_Data_T* event_list, 
                       size_t event_list_size)
{
   SAL_Int_T i;


   /* wait until receivers will be ready */

   /* LOCAL RECEIVERS */
   for(i = 2; i < 2 + NUMBER_OF_RECEIVERS; i++)
   {
         if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[i], Timeout))
         {
            SAL_Exit(1);
         }
   }
  
   /* REMOTE RECEIVERS */
   if (Test_Type != E_STAND_ALONE)
   {
      for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)
      {
         if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[i], Timeout))
         {
            SAL_Exit(1);
         }
      }
   }

   /* publishes test messages */
   Publish_Events(event_list, event_list_size);

   /* signal that all messages were published */

   /* LOCAL RECEIVERS */
   for(i = 2; i < 2 + NUMBER_OF_RECEIVERS; i++)
   {
      SAL_Signal_Semaphore(&Sem_Publish[i]);
   }

   /* REMOTE RECEIVERS */
   if (Test_Type != E_STAND_ALONE)
   {
      for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)
      {
         SAL_Signal_Semaphore(&Sem_Publish[i]);         
      }
   }

   printf("Publish_Test %d Ok\n", j);

   return 0;
}



/*  ***************************************************************
 *
 *   Test communications between threads inside the same process
 *
 *   
 *
 *  ***************************************************************/


/**  Function of RECEIVER_\a n for local receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Local_Receiver_Test_1(void* param)
{
   size_t message_size = (size_t)param;
   SAL_Int_T ret = 0;

   printf("Local_Receiver_Test_1: Start\n");

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      SAL_Exit(1);
   }
   ret = ret || Receive_Test("1_1", Ev_Receive_TC_1_1, sizeof(Ev_Receive_TC_1_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_1, sizeof(Ev_Subscribe_TC_1_1)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("3_1", Ev_Receive_TC_3_1, sizeof(Ev_Receive_TC_3_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_1, sizeof(Ev_Subscribe_TC_3_1)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("5_1", Ev_Receive_TC_5_1, sizeof(Ev_Receive_TC_5_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_1, sizeof(Ev_Subscribe_TC_5_1)/sizeof(SAL_Event_Id_T));

   if (ret)
   {      
      SAL_Exit(1);
   }

   printf("Local_Receiver_Test_1: End\n");
}


/**   Function of RECEIVER_\a n for local receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Local_Receiver_Test_2(void* param)
{
   size_t message_size = (size_t)param;
   SAL_Int_T ret = 0;

   printf("Local_Receiver_Test_2: Start\n");

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      SAL_Exit(1);
   }

   ret = ret || Receive_Test("1_2", Ev_Receive_TC_1_2, sizeof(Ev_Receive_TC_1_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_2, sizeof(Ev_Subscribe_TC_1_2)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("3_2", Ev_Receive_TC_3_2, sizeof(Ev_Receive_TC_3_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_2, sizeof(Ev_Subscribe_TC_3_2)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("5_2", Ev_Receive_TC_5_2, sizeof(Ev_Receive_TC_5_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_2, sizeof(Ev_Subscribe_TC_5_2)/sizeof(SAL_Event_Id_T));
  
   if (ret)
   {
      SAL_Exit(1);
   }

   printf("Local_Receiver_Test_2: Stop\n");
}


/**  Function of PUBLISHER for publisher thread of local communication.
 *
 *   - publishes messages from the list of all test cases
 */

void Local_Publisher_Test(void* param)
{
   SAL_Int_T ret = 0;

   printf("Local_Publish_Test: Start\n");
   

   ret = ret || Publish_Test(1, Ev_Publish_TC_1, sizeof(Ev_Publish_TC_1)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Publish_Test(3, Ev_Publish_TC_3, sizeof(Ev_Publish_TC_3)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Publish_Test(5, Ev_Publish_TC_5, sizeof(Ev_Publish_TC_5)/sizeof(Ev_Test_Case_Data_T));

   if (ret)
   {
      SAL_Exit(1);      
   }

   printf("Local_Publish_Test: Stop\n");
}

/**  Test of SAL_Publish function in local or remote communication. 
 *   
 *   SAL_Subscribe, SAL_Unsubscribe functions and urgent messages are also tested.
 *   Creates threads and starts SAL_Run.
 */

SAL_Int_T Publish_Test_Suite()
{
   SAL_Thread_Id_T destroyed_list[] = { 1, 2 };
   SAL_Int_T retcode;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   switch(Test_Type)
   {

   default:
      /* local communication */

      /* Publisher thread */
      SAL_Create_Thread(
         Local_Publisher_Test, 
         NULL, 
         Init_Thread_Attr("LPublish", PUBLISHER, priority, &thread_attr));

      /* Receiver1 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_1, 
         NULL, 
         Init_Thread_Attr("LRec2", RECEIVER_2, priority, &thread_attr));

      /* Receiver2 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_2, 
         (void*)4, 
         Init_Thread_Attr("LRec3", RECEIVER_3, priority, &thread_attr));

      /* Receiver3 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_1, 
         (void*)5, 
         Init_Thread_Attr("LRec4", RECEIVER_4, priority, &thread_attr));

      /* Receiver4 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_2, 
         (void*)512, 
         Init_Thread_Attr("LRec5", RECEIVER_5, priority, &thread_attr));

      break;
   }

   retcode = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      retcode = 1;
   }

   if (retcode != 0)
   {
      return 1;
   }

   return 0;
}

/**  Function of PUBLISHER_THREAD.
 *
 *   - publishes message with timestamp
 */

void Publisher_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { RECEIVER_THREAD };
   Sample_Msg_T data1;

   printf("Publisher_Fun %d: Start\n", SAL_Get_Thread_Id());

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      SAL_Exit(1);
   }

   data1.field1 = 1;
   data1.field2 = 2;

   ftime(&Time);

   SAL_Publish(EV_E9, &data1, sizeof(data1));

   printf("Publisher_Fun %d: Stop\n", SAL_Get_Thread_Id());
}


/**  Function of RECEIVER_THREAD.
 *
 *   - receives message with timestamp and checks if the time is correct
 */

void Receiver_Fun(void* param)
{
   SAL_Thread_Id_T subscribe_list [] = { EV_E9 };
   SAL_Thread_Id_T unsubscribe_list [] = { EV_E9 };
   const SAL_Message_T* msg;

   printf("Receiver_Fun: Start %d\n", SAL_Get_Thread_Id());

   SAL_Create_Queue(1, 0, malloc, free);

   SAL_Subscribe(subscribe_list, sizeof(subscribe_list)/sizeof(SAL_Thread_Id_T));

   SAL_Signal_Ready();

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

   if (((msg->timestamp_sec - Time.time)*1000 + msg->timestamp_msec - Time.millitm) > 10)   
   {
      SAL_Exit(1);
   }

   SAL_Unsubscribe(unsubscribe_list, sizeof(unsubscribe_list)/sizeof(SAL_Thread_Id_T));

   printf("Receiver_Fun: Stop %d\n", SAL_Get_Thread_Id());
}


/**  Test of timestamp.
 *
 *  Function creates two threads: PUBLISHER_THREAD and RECEIVER_THREAD and starts SAL_Run.
 */

SAL_Int_T Time_Publish_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Publisher_Fun, 
      NULL, 

      Init_Thread_Attr("PUB", PUBLISHER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Receiver_Fun, 
      NULL, 
      Init_Thread_Attr("REC", RECEIVER_THREAD, priority, &thread_attr));
   
   return SAL_Run();
}

/** \} 
 */


SAL_Int_T Set_Up(SAL_Int_T argc, char** argv) 
{
   SAL_Int_T i;
   SAL_Int_T option;
   SAL_Config_T config;
   SAL_Mutex_Attr_T mutex_attr;
   SAL_Semaphore_Attr_T sem_attr;

   while((option = getopt(argc, argv, "srA:v:g:l:teN:")) != -1)
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
      config.max_number_of_threads = 8;

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

   for(i = 2; i <= 2 + 2 * NUMBER_OF_RECEIVERS - 1; i++)
   {
      if (!SAL_Create_Semaphore(&Sem_Rec[i], &sem_attr) || 
         !SAL_Create_Semaphore(&Sem_Publish[i], &sem_attr))
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


   for(i = 2; i <= 2 + 2 * NUMBER_OF_RECEIVERS - 1; i++)
   {
      if (!SAL_Destroy_Semaphore(&Sem_Rec[i]) || 
         !SAL_Destroy_Semaphore(&Sem_Publish[i]))
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

   ret = ret || Publish_Test_Suite();
   
   if (Test_Type == E_STAND_ALONE)
   {
      ret = ret || Time_Publish_Test();
   }

   if (Tear_Down())
   {
      return 1;
   }
   
   printf("Test %s\n", ((ret)?("FAILED"):("PASSED")));

   return ret;
}
