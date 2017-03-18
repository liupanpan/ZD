#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/timeb.h>

#include "include/xsal.h"
#include "include/xsal_i_thread.h"
#include "thread_priority.h"

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 14

#define EV_STOP 1

#define EV_E1 1

typedef enum Thread_Id_Tag
{
   CREATE_THREAD = 1,
   PARAM1_THREAD,
   PARAM2_THREAD,
   THREAD,
   DESTROY_THREAD,
   FUN_THREAD,
   VHIGH_P_THREAD,
   HIGH_P_THREAD,
   NORMAL_P_THREAD,
   LOW_P_THREAD,
   VLOW_P_THREAD,
   MAIN_THREAD,
   WAIT_THREAD,
   SIGNAL_THREAD
} Thread_Id_T;

typedef struct Data_Tag
{
   const char* thread_name;
   SAL_Thread_Id_T thread_id;
   SAL_Priority_T priority;
} Data_T;

Data_T Data_1, Data_2;

short Thread_Running_1 = 0;
short Thread_Running_2 = 0;
short Thread_Running_3 = 0;

SAL_Int_T Long_Timeout = 3000;

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

void Create_Thread_Light_Fun(void* param)
{
   static SAL_Thread_Id_T destroyed_list[] = { PARAM1_THREAD, PARAM2_THREAD };
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;
   bool ret_timeout;

   printf("Create_Thread_Light_Fun: Start %d\n", SAL_Get_Thread_Id());

   Thread_Running_1 = 1;

   while ((!Thread_Running_2) && (!Thread_Running_3))
   {
      SAL_Sleep(100);
   }

   ftime(&time_1);

   ret_timeout = SAL_Wait_Destroyed_Timeout(destroyed_list, 
                    sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
                    Long_Timeout);

   ftime(&time_2);

   if (SAL_Is_Thread_Alive(PARAM1_THREAD) || (SAL_Is_Thread_Alive(PARAM2_THREAD)))
   {
      SAL_Exit(1);
   }

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   if (ret_timeout)
   {
      if (delta_time > 1000 + 20)
      {
         SAL_Exit(1);
      }
   }
   else /* !ret_timeout */
   {
      SAL_Exit(1);
   }

   printf("Create_Thread_Light_Fun: End %d\n", SAL_Get_Thread_Id());

}

void Create_Param_Light_Fun_1(void* param)
{
   Data_T data_3;
   SAL_Int_T i = 0; 

   printf("Create_Param_Light_Fun_1: Start %d\n", SAL_Get_Thread_Id());

   Thread_Running_2 = 1;

   data_3 = *(Data_T*)param;

   /* check if data of the last parameters are valid */
   if ((data_3.thread_name != Data_1.thread_name) || (data_3.thread_id != Data_1.thread_id))
   {
      SAL_Exit(1);
   }

   while ((!Thread_Running_1) && (i++ < 10))
   {
      SAL_Sleep(100);
   }
   
   /* check if function Create_Thread_Fun has started */
   if (!Thread_Running_1)
   {
      SAL_Exit(1);
   }

   printf("Create_Param_Light_Fun_1: End %d\n", SAL_Get_Thread_Id());
}

void Create_Param_Light_Fun_2(void* param)
{
   SAL_Thread_Id_T thread_id;
   SAL_Priority_T priority;
   SAL_Int_T i = 0;

   printf("Create_Param_Light_Fun_2: Start %d\n", SAL_Get_Thread_Id());

   Thread_Running_3 = 1;

   thread_id = SAL_Get_Thread_Id();

   SAL_Get_Thread_Priority(thread_id, &priority);

   /* check if the first three parameters are valid */
   if ( (strcmp(Data_2.thread_name, SAL_Get_Thread_Name_Self()) ) ||
       (Data_2.thread_id != thread_id ) ||
       (Data_2.priority != priority) )
   {
      SAL_Exit(1);
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   printf("Create_Param_Light_Fun_2: End %d\n", SAL_Get_Thread_Id());
}

SAL_Int_T Create_Thread_Light_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { CREATE_THREAD, PARAM1_THREAD, PARAM2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   if (SAL_Create_Thread(
           Create_Thread_Light_Fun, 
           NULL, 
           Init_Thread_Attr("CREATE_T", CREATE_THREAD, priority, &thread_attr)) 
        == SAL_UNKNOWN_THREAD_ID)
   {
      return 1;
   }

   Data_1.thread_name = "Name";
   Data_1.thread_id = 5;

   SAL_Create_Thread(
      Create_Param_Light_Fun_1, 
      (void*)&Data_1, 
      Init_Thread_Attr("PARAM1_T", PARAM1_THREAD, priority, &thread_attr));

   Data_2.thread_name  = "PARAM2_T";
   Data_2.thread_id    = PARAM2_THREAD;
   Data_2.priority     = priority;

   SAL_Create_Thread(
      Create_Param_Light_Fun_2, 
      NULL, 
      Init_Thread_Attr(Data_2.thread_name, Data_2.thread_id, Data_2.priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      ret = 1;
   }

   return ret;

}

void Run_Thread_Fun(void * param)
{
   printf("Run_Thread_Fun: Start %d\n", SAL_Get_Thread_Id());

   Thread_Running_2 = 1;

   SAL_Sleep(1000);

   printf("Run_Thread_Fun: Stop %d\n", SAL_Get_Thread_Id());
}

void Wait_Destroyed_Timeout_Fun(void* param)
{
   static SAL_Thread_Id_T destroyed_list[] = { THREAD };
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;
   bool ret_timeout;

   printf("Wait_Destroyed_Timeout_Fun: Start %d\n", SAL_Get_Thread_Id());

   while (!Thread_Running_2)
   {
      SAL_Sleep(100);
   }

   ftime(&time_1);

   ret_timeout = SAL_Wait_Destroyed_Timeout(destroyed_list, 
                     sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
                     100);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   if (!ret_timeout)
   {
      if ((delta_time < 100 - 7) || (delta_time > 100 + 15))
      {
         SAL_Exit(1);
      }
   } 
   else /* ret_timeout */
   {
      SAL_Exit(1);
   }

   printf("Wait_Destroyed_Timeout_Fun: Stop %d\n", SAL_Get_Thread_Id());
}

SAL_Int_T Wait_Destroyed_Timeout_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { THREAD, DESTROY_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   
   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   if (SAL_Create_Thread(      
          Run_Thread_Fun, 
          NULL, 
          Init_Thread_Attr("THREAD", THREAD, priority, &thread_attr)) 
        == SAL_UNKNOWN_THREAD_ID)
   {
      return 1;
   }

   SAL_Create_Thread(
      Wait_Destroyed_Timeout_Fun, 
      NULL, 
      Init_Thread_Attr("DESTR_T", DESTROY_THREAD, priority, &thread_attr));

    ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      ret = 1;
   }

   return ret;
}

void Thread_Fun(void* param)
{
   printf("Thread_Fun: Start %d\n", SAL_Get_Thread_Id());

   if (!SAL_Create_Queue(5, 0, malloc, free))
   {
      SAL_Exit(1);
   }

   SAL_Signal_Ready();

   Thread_Running_2 = 1;

   if (!SAL_Receive_Timeout(Long_Timeout))
   {
      SAL_Exit(1);
   }

   printf("Thread_Fun: End %d\n", SAL_Get_Thread_Id());
}

void Thread_Fun_Test_Fun(void* param)
{
   SAL_Priority_T priority1 = SAL_NORMAL_PRIORITY;
   SAL_Priority_T priority2;
   SAL_Int_T i = 0;
   static SAL_Thread_Id_T wait_list[] = { THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;

   printf("Thread_Fun_Test_Fun: Start %d\n", SAL_Get_Thread_Id());

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Thread_Fun, 
      NULL, 
      Init_Thread_Attr("THREAD", THREAD, priority, &thread_attr));

   while (!Thread_Running_2)
   {
      SAL_Sleep(100);
   }

   /* test of SAL_Is_Thread_Alive */
   if (!SAL_Is_Thread_Alive(THREAD))
   {
      SAL_Exit(1);
   }

   /* test of SAL_Get_Thread_Name */
   if (strcmp("THREAD", SAL_Get_Thread_Name(THREAD)))
   {
      SAL_Exit(1);
   }

   /* test of SAL_Get_Thread_Name_Self */
   if (strcmp(Data_1.thread_name, SAL_Get_Thread_Name_Self()))
   {
      SAL_Exit(1);
   }

   /* test of SAL_Get_Thread_Id */
   if (Data_1.thread_id != SAL_Get_Thread_Id())
   {
      SAL_Exit(1);
   }

   if (!SAL_Get_Thread_Priority(THREAD, &priority2))
   {
      SAL_Exit(1);
   }

   /* /if (SAL_NORMAL_PRIORITY != Priority2) !!! */
   if (priority != priority2)
   {
      SAL_Exit(1);
   }

   priority1 = SAL_HIGH_PRIORITY;

   SAL_Set_Thread_Priority(THREAD, priority1);

   if (!SAL_Get_Thread_Priority(THREAD, &priority2))
   {
      SAL_Exit(1);
   }

   if (priority2 != priority1)
   {
      SAL_Exit(1);
   }

   /* test of SAL_Sleep */

   ftime(&time_1);

   SAL_Sleep(1000);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);


   if ((delta_time < 990) || (delta_time > 1010))
   {
   }

   if ((delta_time < 985) || (delta_time > 1015))
   {
      SAL_Exit(1);
   }

   /* test of SAL_Delay */

   ftime(&time_1);

   SAL_Delay(1000*1000);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   if ((delta_time < 990) || (delta_time > 1010))
   {
      SAL_Exit(1);
   }

   if (!SAL_Wait_Ready_Timeout(wait_list, 
          sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      SAL_Exit(1);
   }

   SAL_Send(APP_ID, THREAD, EV_STOP, NULL, 0);

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   /* test of SAL_Is_Thread_Alive */
   if (SAL_Is_Thread_Alive(THREAD))
   {
      SAL_Exit(1);
   }

   printf("Thread_Fun_Test_Fun: End %d\n", SAL_Get_Thread_Id());
}

SAL_Int_T Thread_Fun_Test()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { FUN_THREAD, THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Data_1.thread_name  = "FUN_T";
   Data_1.thread_id    = FUN_THREAD;

   SAL_Create_Thread(
      Thread_Fun_Test_Fun, 
      NULL, 
      Init_Thread_Attr("FUN_T", FUN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      ret = 1;
   }

   return ret;
}

void Prior_Level_Fun(void* param)
{
   SAL_Priority_T priority1 = (SAL_Priority_T)param;
   SAL_Priority_T priority2;

   printf("Prior_Level_Fun: Start %d\n", SAL_Get_Thread_Id());

   if (!SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority2))
   {
      SAL_Exit(1);
   }

   if (priority1 != priority2)
   {
      SAL_Exit(1);
   }

   printf("Prior_Level_Fun: End %d\n", SAL_Get_Thread_Id());
}

void Priority_Fun(void* param)
{
   SAL_Thread_Attr_T thread_attr;

   printf("Priority_Fun: Start \n");

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_VERY_HIGH_PRIORITY, 
      Init_Thread_Attr("VHIGH_P", VHIGH_P_THREAD, SAL_VERY_HIGH_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_HIGH_PRIORITY, 
      Init_Thread_Attr("HIGH_P", HIGH_P_THREAD, SAL_HIGH_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_NORMAL_PRIORITY, 
      Init_Thread_Attr("NORM_P", NORMAL_P_THREAD, SAL_NORMAL_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_LOW_PRIORITY, 
      Init_Thread_Attr("LOW_P", LOW_P_THREAD, SAL_LOW_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_VERY_LOW_PRIORITY, 
      Init_Thread_Attr("VLOW_P", VLOW_P_THREAD, SAL_VERY_LOW_PRIORITY, &thread_attr));

   printf("Priority_Fun: End \n");
}

SAL_Int_T Priority_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Priority_Fun, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      ret = 1;
   }

   return ret;
}

void Wait_Timeout_Fun_1(void* param)
{
   static SAL_Thread_Id_T wait_list[] = { SIGNAL_THREAD };
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD };
   struct timeb time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool ret_timeout;

   ftime(&time_1);

   /* should wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     Long_Timeout);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   res_time = delta_time - 200;

   if (ret_timeout)
   {
      if (abs(res_time) > 40)
      {
         SAL_Exit(1);
      }
   }
   else /* !ret_timeout */
   {
      SAL_Exit(1);
   }

   SAL_Create_Queue(5, 0, malloc, free);

   Thread_Running_1 = 1;

   ftime(&time_1);

   /* should not wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     Long_Timeout);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         SAL_Exit(1);
      }
   }
   else /* !ret_timeout */
   {
      SAL_Exit(1);
   }

   /* should receive message */
   if (SAL_Receive_Timeout(Long_Timeout) == NULL)
   {
      SAL_Exit(1);
   }

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      SAL_Exit(1);
   }
}

void Signal_Timeout_Fun_1(void* param)
{
   SAL_Stat_Queue_T queue_stat;
   SAL_Int_T i = 0;

   /* try to send message to the queue which shouldn't exist */
   if (SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      SAL_Exit(1);
   }

   /* after SAL_Signal_Ready the other thread should create the queue */
   SAL_Signal_Ready();

   while (!Thread_Running_1)
   {
      SAL_Sleep(100);
   }

   /* here message should be sent because queue should exist*/
   if (!SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      SAL_Exit(1);
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   SAL_Stat_Queue(WAIT_THREAD, &queue_stat);

   /* check if the other thread received message */
   if (queue_stat.message_count == 1)
   {
      SAL_Exit(1);
   }
}

void Main_Timeout_Fun_1(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Wait_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("WAIT_T", WAIT_THREAD, priority, &thread_attr));
   
   SAL_Sleep(200);

   SAL_Create_Thread(
      Signal_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("SIGNAL_T", SIGNAL_THREAD, priority, &thread_attr));
}

SAL_Int_T Wait_Ready_Timeout_Test_1()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD, WAIT_THREAD, SIGNAL_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Main_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));
   
   ret =  SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      ret = 1;
   }

   return ret;
}

void Wait_Timeout_Fun_2(void* param)
{
   static SAL_Thread_Id_T wait_list[] = { SIGNAL_THREAD };
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD };
   struct timeb time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool ret_timeout;

   ftime(&time_1);

   /* should wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     100);
   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   res_time = delta_time - 100;

   if (!ret_timeout)
   {
      if (abs(res_time) > 20)
      {
         SAL_Exit(1);
      }
   }
   else /* ret_timeout */
   {
      SAL_Exit(1);
   }

   SAL_Create_Queue(5, 0, malloc, free);

   Thread_Running_1 = 1;

   while (!Thread_Running_2)
   {
      SAL_Sleep(100);
   }

   ftime(&time_1);

   /* should not wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     Long_Timeout);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         SAL_Exit(1);
      }
   }
   else
   {
      SAL_Exit(1);
   }

   /* should receive message */
   if (SAL_Receive_Timeout(Long_Timeout) == NULL)
   {
      SAL_Exit(1);
   }

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      SAL_Exit(1);
   }
}

void Signal_Timeout_Fun_2(void* param)
{
   SAL_Stat_Queue_T queue_stat;
   SAL_Int_T i = 0;

   SAL_Signal_Ready();

   Thread_Running_2 = 1;

   while (!Thread_Running_1)
   {
      SAL_Sleep(100);
   }

   if (!SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      SAL_Exit(1);
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   SAL_Stat_Queue(WAIT_THREAD, &queue_stat);

   if (queue_stat.message_count == 1)
   {
      SAL_Exit(1);
   }
}

void Main_Timeout_Fun_2(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Wait_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("WAIT_T", WAIT_THREAD, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Signal_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("SIGNAL_T", SIGNAL_THREAD, priority, &thread_attr));
}

SAL_Int_T Wait_Ready_Timeout_Test_2()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD, WAIT_THREAD, SIGNAL_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Main_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));
   
   ret =  SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      ret = 1;
   }

   return ret;
}

SAL_Int_T Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_Int_T Option;
   SAL_Config_T Config;
   SAL_Priority_T priority = 2; /* no priority test */

   while ( ( Option = getopt( argc, argv, "pg:v:l:teN:" ) ) != -1 ) 
   {        
      switch(Option) 
      {             
         case 'p':
            priority = 1; /* with priority test */
            break;

         default:
            break;
      }     
   }

   SAL_Get_Config(&Config);

   Config.app_id = APP_ID;
   Config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&Config))
   {
      return 0;
   }

   return priority;
}


void Tear_Down()
{
}


SAL_Int_T main (SAL_Int_T argc, char** argv)
{
   SAL_Int_T ret = 0;
   SAL_Priority_T priority = 0;
   SAL_App_Id_T app_id;
   SAL_Int_T max_numb_of_threads;

   priority = Set_Up(argc, argv);

   if (!priority)
   {
      return 1;
   }

   if ((app_id = SAL_Get_App_Id()) != APP_ID)
   {
      SAL_Exit(1);
   }

   if ((max_numb_of_threads = SAL_Get_Max_Number_Of_Threads()) != MAX_NUMB_OF_THREADS)
   {
      SAL_Exit(1);
   }

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   Create_Thread_Light_Test();

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Wait_Destroyed_Timeout_Test();

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Thread_Fun_Test();

   if (priority == 1)
   {
      ret = ret || Priority_Test();
   }

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Wait_Ready_Timeout_Test_1();

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Wait_Ready_Timeout_Test_2();

   Tear_Down();

   printf("Test %s\n", ((ret)?("FAILED"):("PASSED")));

   return ret;
}
