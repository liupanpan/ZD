#include <stdio.h>
#include "xsal.h"
#include "debugtrace.h"

#define APP_ID 2

#define MAX_NUMB_OF_THREADS 10

#define FIRST_UNKNOWN_THREAD_ID 6

#define EV_E1 1

typedef enum Thread_Id_Tag
{
   MAIN_THREAD = 1,
   KNOWN_THREAD
} Thread_Id_T;

char* Thread_Name[6] = { "Un6", "Un7", "Un8", "Un9", "Un10", "Un11" };

SAL_Thread_Id_T Thread_Id_1, Known_Thread_Id, Unknown_Thread_Id;
SAL_Thread_Id_T Thread_Id[6];
SAL_Int_T Next_Start = 0;

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


/**  Function of known thread.
 */

void Known_Thread_Fun(void * param)
{
   printf("Known_Thread_Fun: Start\n");

   printf("Known_Thread_Fun: Stop\n");
}


/**  Function of unknown thread.
 *
 *   - checks if Thread_Id is correct
 *   - sends the message with its thread id to the main thread 
 *   - receives the message from the main thread with the thread id of the receiving (unknown)
 *     thread
 *   - checks if the received thread id is correct
 */

void Unknown_Thread_Fun(void* param)
{
   const SAL_Message_T* msg;
   SAL_Thread_Id_T data;
   SAL_Thread_Id_T j = (SAL_Thread_Id_T)param;
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD };
   SAL_Int_T i, k;

   printf("Unknown_Thread_Fun: %d, Start\n", SAL_Get_Thread_Id());

   /* unknown thread is started for the second time */
   if (Next_Start)
   {
      i = Unknown_Thread_Id - 6;
      Thread_Id[i] = Unknown_Thread_Id;      
   }
   /* for the first time */
   else
      i = j - 6;

   /* check if thread id is correct */
   if (Thread_Id[i] != SAL_Get_Thread_Id())
   {
                                                         i, SAL_Get_Thread_Id());
      for (k = 0; k < 5; k++)
      {
         printf("Thread_Id[%d]: %d\n", k, Thread_Id[k]);
      }
      SAL_Exit(1);
   }

   printf("Thread number %d\n", SAL_Get_Thread_Id());

   /* send the message to the main thread */
   SAL_Create_Queue(1, 0, malloc, free);

   j = SAL_Get_Thread_Id();

   SAL_Send(APP_ID, MAIN_THREAD, EV_E1, &j, sizeof(SAL_Thread_Id_T));

    /* wait for the message from the main thread */
#if defined (XSAL)
   msg = SAL_Receive();
#else
   msg = SAL_Receive_Timeout(Timeout);
#endif

   data = *(SAL_Thread_Id_T*)msg->data;

   if (data != SAL_Get_Thread_Id())
   {
      SAL_Exit(1);
   }

   /* all threads apart from thread_id = 8 and 9 are waiting untill the main thread finishes */
   if ((j != 8) && (j != 9))
   {
#if defined (XSAL)
      SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
#else
      if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
          Timeout))
      {
         SAL_Exit(1);
      }
#endif
   }

   Thread_Id[i] = 0;

   printf("Unknown_Thread_Fun: %d, Stop\n", SAL_Get_Thread_Id());
}



/** Function of MAIN_THREAD.
 *
 *  - creates 5 unknown threads
 *  - when all unknown thread ids are in use, tries to create one more (which shouldn't been 
 *    created)
 *  - then waits for the message from each of the 5 unknown treads including their thread id
 *  - sends to each of 5 threads the message with unknown thread id
 *  - after some of the threads finish, create the next unknown thread
 */


void Main_Thread_Fun(void* param)
{
   const SAL_Message_T* msg;
   SAL_Int_T i, k;
   SAL_Thread_Id_T j;
   static SAL_Thread_Id_T destroyed_list[] = { 8, 9 };
   SAL_Thread_Id_T data;
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;

   printf("Main_Thread_Fun: Start\n");

   SAL_Create_Queue(5, 0, malloc, free); 

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   /* create 5 unknown threads */
   for (j = 6, i = 0; j < 11 && i < 5; j++, i++)
   {  
      if ((Thread_Id[i] = 
         SAL_Create_Thread(
            Unknown_Thread_Fun, 
            (void*)j,
            Init_Thread_Attr(Thread_Name[i], SAL_UNKNOWN_THREAD_ID, priority, &thread_attr))) 
         == SAL_UNKNOWN_THREAD_ID)
      {
         for (k = 0; k < 5; k++)
         {
            Tr_Inf(DT_SAL, "Thread_Id[%d]: %d\n", k, Thread_Id[k]);         
         }
         SAL_Exit(1);
      }

      if (Thread_Id[i] != j)
      {
                                                                             j, i, Thread_Id[i]);
         for (k = 0; k < 5; k++)
         {
            printf("Thread_Id[%d]: %d\n", k, Thread_Id[k]);         
         }
         SAL_Exit(1);
      }

   }

   /* try to create one thread above the max admitted number */
   if ((Thread_Id[i] = 
      SAL_Create_Thread(
         Unknown_Thread_Fun, 
         (void*)j, 
         Init_Thread_Attr(Thread_Name[i], SAL_UNKNOWN_THREAD_ID, priority, &thread_attr))) 
      != SAL_UNKNOWN_THREAD_ID)
   {
      SAL_Exit(1);
   }

   /* wait for the messages from unknown threads */
   for (i = 0; i < 5; i++)
   {
#if defined (XSAL)
      msg = SAL_Receive();
#else
      msg = SAL_Receive_Timeout(Timeout);
#endif

      data = *(SAL_Thread_Id_T*)msg->data;

      if (data != msg->sender_thread_id)
      {
         SAL_Exit(1);
      }
   }

   /* send messages to the unknown threads */
   for (j = 6; j < 11; j++)
   {
      SAL_Send(APP_ID, j, EV_E1, &j, sizeof(SAL_Thread_Id_T));
   }

#if defined (XSAL)
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
#else
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
       sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
       Timeout))
   {
      SAL_Exit(1);
   }
#endif

   Next_Start = 1;

   /* create known thread */
   if ((Known_Thread_Id = 
      SAL_Create_Thread(
         Known_Thread_Fun, 
         (void*)KNOWN_THREAD, 
         Init_Thread_Attr("Known", KNOWN_THREAD, priority, &thread_attr))) 
      == SAL_UNKNOWN_THREAD_ID)
   {
      SAL_Exit(1);
   }

   if (Known_Thread_Id != KNOWN_THREAD)
   {
      SAL_Exit(1);
   }

   /* create next unknown thread */

   if ((Unknown_Thread_Id =
      SAL_Create_Thread(
         Unknown_Thread_Fun, 
         NULL, 
         Init_Thread_Attr("Unknown", SAL_UNKNOWN_THREAD_ID, priority, &thread_attr))) 
      == SAL_UNKNOWN_THREAD_ID)
   {
      SAL_Exit(1);
   }

#if defined (XSAL)
   msg = SAL_Receive();
#else
   msg = SAL_Receive_Timeout(Timeout);
#endif

   data = *(SAL_Thread_Id_T*)msg->data;

   if (data != msg->sender_thread_id)
   {
      SAL_Exit(1);
   }

   SAL_Send(APP_ID, Unknown_Thread_Id, EV_E1, &Unknown_Thread_Id, sizeof(SAL_Thread_Id_T));

   printf("Main_Thread_Fun: Stop\n");
}


/**  Test of unknown threads.
 *
 *   Creates MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Unknown_Thread_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Thread_Id_1 = 
      SAL_Create_Thread(Main_Thread_Fun, 
                        NULL, 
                        Init_Thread_Attr("Main", MAIN_THREAD, priority, &thread_attr));

   return SAL_Run();
}


SAL_Int_T Set_Up()
{
   SAL_Int_T i;
   SAL_Config_T config;

   SAL_Get_Config(&config);
   
   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;
   config.first_unknown_thread_id = FIRST_UNKNOWN_THREAD_ID;
   
   if (!SAL_Init(&config))
   {
      return 1;
   }

   for (i = 0; i < 6; i++)
   {
      Thread_Id[i] = 0;
   }

   return 0;
}


void Tear_Down()
{
}

SAL_Int_T main (SAL_Int_T argc, char** argv)
{
   SAL_Int_T ret = 0;
      
   if (Set_Up())
   {
      return 1;
   }

   ret = ret || Unknown_Thread_Test();

   Tear_Down();

   printf("Test %s\n", ((ret)?("FAILED"):("PASSED")));

   return ret;
}
