#include <stdio.h>
#include <signal.h>
#include <malloc.h>
#include <sys/timeb.h>

#include "include/xsal.h"
#include "include/xsal_i_assert.h"
#include "include/xsal_i_time.h"
#include "include/xsal_i_timer.h"

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 2

#define TIMER1 1

#define EV_STOP 3

#define EV_1 (10 + 1)
#define EV_2 (10 + 2)
#define EV_3 (10 + 3)

typedef enum Thread_Id_Tag
{
   MAIN_THREAD = 1,
   TIMER_THREAD,
   Single_Shot_Thread,
   Last_Thread_Id
} Thread_Id_T;

SAL_Int_T Timeout = 3000;

void Send_Thread_1(void* param);
void Send_Thread_2(void* param);


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

/** \name Functions of Periodic_Start_Timer_Thread
 *
 *  \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts the timer
 *   - receives timer events
 *   - then stops and destroys the timer
 */

void Period_Start_Timer(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;
   SAL_Int_T i = 0;

   printf("Period_Start_Timer: Start\n");

   SAL_Create_Queue(3, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   SAL_Start_Timer(timer_id, 100, 1);

   SAL_Signal_Ready();

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif

   if (msg->event_id == EV_STOP)
   {
       SAL_Exit(1);
   }

   while (i<2)
   {

#if defined (XSAL)

      msg = SAL_Receive();

#else

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         SAL_Exit(1);
      }

#endif

      printf("%d\n", msg->event_id);

      i++;
   } 

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   printf("Period_Start_Timer: Stop\n");
}


/**  Test of SAL_Create_Timer and SAL_Start_Timer functions.
 *
 *   Tests if periodic timer generates events after being started.
 */

SAL_Int_T Periodic_Start_Timer_Test()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD, MAIN_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Period_Start_Timer, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Send_Thread_1, 
      NULL,
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }

#endif

   return ret;
}

/** \}
 */

/**  \name Functions of Periodic_Stop_Timer_Test
 *
 *   \{
 */

/**  Function of TIMER_THREAD.
 *   
 *   - creates and starts the timer
 *   - receives timer events
 *   - stops the timer
 */

void Period_Stop_Timer(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;
   SAL_Int_T i = 0;

   printf("Period_Stop_Timer: Start\n");

   SAL_Create_Queue(4, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   SAL_Start_Timer(timer_id, 100, 1);

   while (i < 3)
   {

#if defined (XSAL)

      msg = SAL_Receive();

#else

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         SAL_Exit(1);
      }

#endif

      printf("%d\n", msg->event_id);

      i++;
   } 

   SAL_Stop_Timer(timer_id);

   SAL_Signal_Ready();

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif

   if (msg->event_id != EV_STOP)
   {
      SAL_Exit(1);
   }

   SAL_Destroy_Timer(timer_id); /* !!! */

   printf("Period_Stop_Timer: Stop\n");

}

/**  Test of SAL_Stop_Timer function for periodic timer.
 * 
 *   Tests if periodic timer generates events after being stopped.
 */

SAL_Int_T Periodic_Stop_Timer_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD, MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Period_Stop_Timer, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Send_Thread_2, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }
#endif

   return ret;
}

/**  \}
 */

/**  \name Functions of Single_Start_Timer_Test_1
 *
 *   \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts the timer
 *   - receives timer event
 *   - stops and destroys timer
 */

void Single_Start_Timer_1(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD };

   printf("Single_Start_Timer_1: Start\n");

   SAL_Create_Queue(2, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   SAL_Start_Timer(timer_id, 100, 0); 

   SAL_Signal_Ready();

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif

   printf("%d\n", msg->event_id);

   if (msg->event_id == EV_STOP)
   {
      SAL_Exit(1);      
   }

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

   SAL_Destroy_Timer(timer_id);

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

   printf("Single_Start_Timer_1: Stop\n");
}



/** Tests of SAL_Start_Timer function for single timer.
 *
 *  Tests if single timer generates event after being started.
 */

SAL_Int_T Single_Start_Timer_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD, MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   
   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Single_Start_Timer_1, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Send_Thread_1, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }
#endif

   return ret;
}

/**  \}
 */

/**  \name Functions of Single_Start_Timer_Test_2
 *
 *   \{
 */


/**  Function of TIMER_THREAD.
 *
 *   - creates and starts the timer
 *   - receives timer event 
 *   - stops and destroys the timer
 */

void Single_Start_Timer_2(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;

   printf("Single_Start_Timer_2: Start\n");

   SAL_Create_Queue(2, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   SAL_Start_Timer(timer_id, 100, 0);

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif

   printf("%d\n", msg->event_id);

   SAL_Signal_Ready();

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif

   if (msg->event_id != EV_STOP)
   {
      SAL_Exit(1);
   }

   SAL_Destroy_Timer(timer_id);

   printf("Single_Start_Timer_2: Stop\n");
}


/**  Test of single timer.
 *   
 *   Tests if single timer generates more than one event.
 */

SAL_Int_T Single_Start_Timer_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD, MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Single_Start_Timer_2, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Send_Thread_2, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }

#endif

   return ret;

}

/**  \}
 */


/**  \name Functions of Tick_Timer_Test_1
 *
 *   \{
 */

/**  Function of Tick_Timer_Test_1 thread.
 *
 *   - checks the timer interval
 *   - stores the time of receiving all ten events
 *   - checks if the intervals between receiving times are correct
 */

void Tick_Timer_1(void* param)
{
   SAL_Timer_Id_T timer_id; 
   SAL_Int_T timer_interval;
   const SAL_Message_T* msg;
   struct timeb t[10];
   SAL_Int_T dt[10];
   SAL_Int_T i;
 
   printf("Tick_Timer_1: Start\n");

   SAL_Create_Queue(10, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   timer_interval = 100;
 
   SAL_Start_Timer(timer_id, timer_interval, 1);

   for (i = 0; i < 10; i++)
   {

#if defined (XSAL)

      msg = SAL_Receive();

#else

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         SAL_Exit(1);
      }

#endif

   ftime(&t[i]);

   }

   i = 0;

   for (i = 0; i < 9; i++)
   {
      dt[i] = (SAL_Int_T)((t[i+1].time - t[i].time)*1000 + t[i+1].millitm - t[i].millitm);

      if ((dt[i] < timer_interval - timer_interval/10) 
         || (dt[i] > timer_interval + timer_interval/10))
      {
      }

      if ((dt[i] < timer_interval - timer_interval/6) 
         || (dt[i] > timer_interval + timer_interval/6))
      {
         SAL_Exit(1);
      }

      printf("%d\n", dt[i]);
   }

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   printf("Tick_Timer_1: Stop\n");
}

/**  Test of timer interval.  
 *
 *   Test checks if the periodic timer interval is proper.
 */

SAL_Int_T Tick_Timer_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Tick_Timer_1, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }

#endif

   return ret;

}

/**  Function of Tick_Timer_Test_2 thread.
 *
 *   - checks the timer interval
 *   - stores the time before receiving all ten events and time after receiving all of them
 *   - then checks if the interval before and after receiving events is correct
 */ 

void Tick_Timer_2(void* param)
{
   SAL_Timer_Id_T timer_id; 
   SAL_Int_T timer_interval, tim_int;
   const SAL_Message_T* msg;
   struct timeb t1, t2;
   SAL_Int_T dt;
   SAL_Int_T i;

   printf("Tick_Timer_2: Start\n");

   SAL_Create_Queue(10, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   timer_interval = 100;

   SAL_Start_Timer(timer_id, timer_interval, 1);

   i = 0;

   ftime(&t1);

   while (i < 10)
   {

#if defined (XSAL)

      msg = SAL_Receive();

#else

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         SAL_Exit(1);
      }

#endif

      i++;
   }
 
   ftime(&t2);

   dt = (SAL_Int_T)((t2.time - t1.time)*1000 + t2.millitm - t1.millitm);

   printf("%d\n", dt);

   tim_int = 10 * timer_interval;

   if ((dt < tim_int - tim_int/10) && (dt > tim_int + tim_int/10))
   {
   }

   if ((dt < tim_int - tim_int/6) && (dt > tim_int + tim_int/6))
   {
      SAL_Exit(1);
   }

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   printf("Tick_Timer_2: Stop\n");
}


/**  Test of timer interval.
 *   
 *   Function checks if the timer interval is proper.
 */

SAL_Int_T Tick_Timer_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Tick_Timer_2, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
            sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
            Timeout))
   {
      ret = 1;
   }

#endif

   return ret;

}

/** \}
 */

/**  \name Functions of Tick_Timer_Test_3
 *
 *   \{
 */

/**  Function of Tick_Timer_Test_3 thread.
 *
 *   - checks the timer interval
 *   - stores the time of receiving all ten events, the time is stored from timestamps
 *   - checks if the intervals between receiving times are correct
 */ 

void Tick_Timer_3(void* param)
{
   SAL_Timer_Id_T timer_id; 
   SAL_Int_T timer_interval;
   const SAL_Message_T* msg;
   struct timeb t[10];
   SAL_Int_T dt[10];
   SAL_Int_T i;

   printf("Tick_Timer_3: Start\n");

   SAL_Create_Queue(10, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   timer_interval = 100;

   SAL_Start_Timer(timer_id, timer_interval, 1);

   i = 0;

   while (i < 10)
   {

#if defined (XSAL)

      msg = SAL_Receive();

#else 

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         SAL_Exit(1);
      }

#endif

      t[i].time = msg->timestamp_sec;
      t[i].millitm = msg->timestamp_msec;

      i++;
   }

   for (i = 0; i < 9; i++)
   {
      dt[i] = (SAL_Int_T)((t[i+1].time - t[i].time)*1000 + t[i+1].millitm - t[i].millitm);
   }

    for (i = 0; i < 9; i++)
   {
      if ((dt[i] < timer_interval - timer_interval/10) || 
         (dt[i] > timer_interval + timer_interval/10))
      {
      }
      
      if ((dt[i] < timer_interval - timer_interval/6) || 
         (dt[i] > timer_interval + timer_interval/6))
      {
         SAL_Exit(1);
      }

      printf("%d\n", dt[i]);
   }

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   printf("Tick_Timer_3: Stop\n");
}


/**  Test of timer interval.  
 *
 *   Test checks if the periodic timer interval is proper.
 */

SAL_Int_T Tick_Timer_Test_3()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Tick_Timer_3, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }

#endif

   return ret;
}

/** \}
 */

/**  \name Functions of Change_Tick_Timer_Test_1
 *
 *   \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts periodic timer with period of 0,5 sec
 *   - after generating 10 events starts the same timer (without stopping it) with period of 1 sec
 *   - after generating 10 events of period 1 sec timer is stopped and destroyed
 */

void Change_Tick_Timer_1(void* param)
{
   const SAL_Message_T* msg;
   SAL_Timer_Id_T timer_id;
   SAL_Int_T i = 0;
   struct timeb t[21];
   SAL_Int_T dt[20];
   SAL_Int_T timer_interval_1 = 500;
   SAL_Int_T timer_interval_2 = 1000;


   SAL_Create_Queue(20, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   SAL_Start_Timer(timer_id, timer_interval_1, 1);

   while (i < 10)
   {
      ftime(&t[i]);

#if defined (XSAL)

      msg = SAL_Receive();

#else

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         SAL_Exit(1);
      }

#endif

      printf("%d %d\n", i+1, msg->event_id);      
       
      i++;
   }

   SAL_Start_Timer(timer_id, timer_interval_2, 1);

   while (i < 20)
   {
      ftime(&t[i]);

#if defined (XSAL)

      msg = SAL_Receive();

#else

      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         SAL_Exit(1);
      }

#endif

      printf("%d %d\n", i+1, msg->event_id);      

      i++;
   }

   ftime(&t[i]);

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);


   for (i = 0; i < 9; i++)
   {
      dt[i] = (SAL_Int_T)((t[i+1].time - t[i].time)*1000 + t[i+1].millitm - t[i].millitm);

      if ((dt[i] < timer_interval_1 - timer_interval_1/10) 
         || (dt[i] > timer_interval_1 + timer_interval_1/10))
      {
      }

      if ((dt[i] < timer_interval_1 - timer_interval_1/6) 
         || (dt[i] > timer_interval_1 + timer_interval_1/6))
      {
         SAL_Exit(1);
      }
   }   

   for (i = 10; i < 19; i++)
   {
      dt[i] = (SAL_Int_T)((t[i+1].time - t[i].time)*1000 + t[i+1].millitm - t[i].millitm);

      if ((dt[i] < timer_interval_2 - timer_interval_2/10) 
         || (dt[i] > timer_interval_2 + timer_interval_2/10))
      {
      }

      if ((dt[i] < timer_interval_2 - timer_interval_2/6) 
         || (dt[i] > timer_interval_2 + timer_interval_2/6))
      {
         SAL_Exit(1);
      }
   }
}

/**  Test of changing timer interval.
 */

SAL_Int_T Change_Tick_Timer_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   
   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Change_Tick_Timer_1, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }

#endif

   return ret;

}

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts single timer with period 1 sec
 *   - 0,5 sec after starting starts the same timer with period 2 sec
 *   - the timer event should appear after 2,5 sec after starting timer the first time
 *   - at the end timer is stopped and destroyed
 */
void Change_Tick_Timer_2(void* param)
{
   const SAL_Message_T* msg;
   SAL_Timer_Id_T timer_id;
   struct timeb t1, t2;
   SAL_Int_T dt;
   SAL_Int_T timer_interval_1 = 1000;
   SAL_Int_T timer_interval_2 = 2000;
   SAL_Int_T time_interval = 2500;


   SAL_Create_Queue(1, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      SAL_Exit(1);
   }

   SAL_Start_Timer(timer_id, timer_interval_1, 0);

   ftime(&t1);

   SAL_Sleep(500);

   SAL_Start_Timer(timer_id, timer_interval_2, 0);

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

    if (msg == NULL)
   {
      SAL_Exit(1);
   }

#endif
   
   ftime(&t2);

   printf("%d\n", msg->event_id);      

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   dt = (SAL_Int_T)((t2.time - t1.time)*1000 + t2.millitm - t1.millitm);

   if ((dt < time_interval - time_interval/10) || (dt > time_interval + time_interval/10))
      {
      }

   if ((dt < time_interval - time_interval/6) || (dt > time_interval + time_interval/6))
      {
         SAL_Exit(1);
      }
}


/**  Test of changing timer interval.
 */

SAL_Int_T Change_Tick_Timer_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] = { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Change_Tick_Timer_2, 
      NULL, 
      Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      ret = 1;
   }

#endif

   return ret;

}

/**  Function of MAIN_THREAD for PeriodicStartTimerThread, SingleStartTimerTest.
 *
 *   - sends EV_STOP event to the TIMER_THREAD
 */

void Send_Thread_1(void* param)
{
   static SAL_Thread_Id_T wait_list[] = { TIMER_THREAD };

   printf("PeriodStartThread: Start\n");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, 
           sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      SAL_Exit(1);
   }

#endif 

   SAL_Sleep(200);

   SAL_Send(APP_ID, TIMER_THREAD, EV_STOP, NULL, 0);

   printf("PeriodStartThread: Stop\n");
}

/**  Function of MAIN_THREAD for Periodic_Stop_Timer_Test, SingleStopTimerTest2, Single_Stop_Timer_Test.
 *
 *   - sends EV_STOP event to TIMER_THREAD
 */

void Send_Thread_2(void* param)
{
   static SAL_Thread_Id_T wait_list[] = { TIMER_THREAD };

   printf("PeriodStopThread: Start\n");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, 
           sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      SAL_Exit(1);
   }

#endif

   SAL_Send(APP_ID, TIMER_THREAD, EV_STOP, NULL, 0);

   printf("PeriodStopThread: Stop\n");
}

SAL_Int_T Set_Up()
{
   bool status = true;
   SAL_Config_T config;

   SAL_Get_Config(&config);
   config.app_id = APP_ID;
   config.max_number_of_threads = Last_Thread_Id-1;
   if (!SAL_Init(&config))
   {
      status = false;
   }
   return status;
}

void Tear_Down()
{
#if defined(XSAL)
   SAL_Deinit();
#endif /* XSAL */
}

bool Single_Shot_Test(void)
{
   bool tm_ready[3] = { false, false, false };
   SAL_I_Time_Spec_T tm[3];
   SAL_Timer_Id_T timer1, timer2, timer3;

   printf("Single_Shot_Test: BEGIN\n");

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      return false;
   }

   SAL_I_Get_Time(&tm[0]);
   SAL_Start_Timer(timer1, 100, 0);

   SAL_I_Get_Time(&tm[1]);
   SAL_Start_Timer(timer2, 250, 0);

   SAL_I_Get_Time(&tm[2]);
   SAL_Start_Timer(timer3, 1000, 0);

   do
   {
      uint32_t diff;
      SAL_I_Time_Spec_T now;
      const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
      if (msg == NULL)
      {
         return false;
      }

      SAL_I_Get_Time(&now);
      switch(msg->event_id)
      {
      case EV_1:
         diff = SAL_I_Time_Diff(&tm[0], &now);
         if (diff > 110)
         {
            return false;
         }
         tm_ready[0] = true;
         printf("diff=%d, should be %d\n", diff, 100);
         break;

      case EV_2:
         diff = SAL_I_Time_Diff(&tm[1], &now);
         if (diff > 275)
         {
            return false;
         }
         tm_ready[1] = true;
         printf("diff=%d, should be %d\n", diff, 250);
         break;

      case EV_3:
         diff = SAL_I_Time_Diff(&tm[2], &now);
         if (diff > 1100)
         {
            return false;
         }
         tm_ready[2] = true;
         printf("diff=%d, should be %d\n", diff, 1000);
         break;
      }
   } while(!tm_ready[0] || !tm_ready[1] || !tm_ready[2]);

   SAL_Stop_Timer(timer1);
   SAL_Destroy_Timer(timer1);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);
   printf("Single_Shot_Test: PASSED\n");

   return true;
}

bool Single_Shot_Restart_Test(void)
{
   SAL_I_Time_Spec_T tm[3];
   SAL_Timer_Id_T timer1, timer2, timer3;

   printf("Single_Shot_Restart_Test: BEGIN\n");

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      return false;
   }

   {
      /** Test Case 1: restart single timer
       */
      uint32_t diff;
      SAL_I_Time_Spec_T now;
      const SAL_Message_T* msg;
         
      SAL_I_Get_Time(&tm[0]);
      SAL_Start_Timer(timer1, 2000, 0);
      msg = SAL_Receive_Timeout(250);
      if (msg != NULL)
      {
         return false;
      }
      SAL_Start_Timer(timer1, 250, 0);
      msg = SAL_Receive_Timeout(500);
      SAL_I_Get_Time(&now);
      if (msg == NULL)
      {
         return false;
      }
      diff = SAL_I_Time_Diff(&tm[0], &now);
      if (diff > 800)
      {
         return false;
      }
      printf("Single_Shot_Restart_Test TC1: diff=%d, should be %d\n", diff, 750);
   }

   {
      /** Test Case 2: after Tm1 expired, swap Tm2 & Tm3
       */
      bool tm_ready[3] = { false, false, false };

      SAL_I_Get_Time(&tm[0]);
      SAL_Start_Timer(timer1, 500, 0);

      SAL_I_Get_Time(&tm[1]);
      SAL_Start_Timer(timer2, 1000, 0);

      SAL_I_Get_Time(&tm[2]);
      SAL_Start_Timer(timer3, 2000, 0);

      do
      {
         uint32_t diff;
         SAL_I_Time_Spec_T now;
         const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
         if (msg == NULL)
         {
            return false;
         }

         SAL_I_Get_Time(&now);
         switch(msg->event_id)
         {
         case EV_1:
            SAL_Start_Timer(timer2, 1500, false);
            SAL_Start_Timer(timer3, 500, false);
            diff = SAL_I_Time_Diff(&tm[0], &now);
            tm_ready[0] = true;
            if (diff > 550)
            {
               return false;
            }
            printf("Single_Shot_Restart_Test TC2: diff=%d, should be %d\n", diff, 500);
            break;

         case EV_2:
            diff = SAL_I_Time_Diff(&tm[1], &now);
            if (diff > 2200)
            {
               return false;
            }
            tm_ready[1] = true;
            printf("Single_Shot_Restart_Test TC2: diff=%d, should be %d\n", diff, 2000);
            break;

         case EV_3:
            diff = SAL_I_Time_Diff(&tm[2], &now);
            if (diff > 1100)
            {
               return false;
            }
            tm_ready[2] = true;
            printf("Single_Shot_Restart_Test TC2: diff=%d, should be %d\n", diff, 1000);
            break;
         }
      } while(!tm_ready[0] || !tm_ready[1] || !tm_ready[2]);
   }

   {
      /** Test Case 3: after Tm1 expired, rester Tm2 and Tm3
       */
      bool tm_ready[3] = { false, false, false };

      SAL_I_Get_Time(&tm[0]);
      SAL_Start_Timer(timer1, 500, 0);

      SAL_I_Get_Time(&tm[1]);
      SAL_Start_Timer(timer2, 1000, 0);

      SAL_I_Get_Time(&tm[2]);
      SAL_Start_Timer(timer3, 1500, 0);

      do
      {
         uint32_t diff;
         SAL_I_Time_Spec_T now;
         const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
         if (msg == NULL)
         {
            return false;
         }

         SAL_I_Get_Time(&now);
         switch(msg->event_id)
         {
         case EV_1:
            SAL_Start_Timer(timer2, 500, false);
            SAL_Start_Timer(timer3, 1000, false);
            diff = SAL_I_Time_Diff(&tm[0], &now);
            tm_ready[0] = true;
            if (diff > 550)
            {
               return false;
            }
            printf("Single_Shot_Restart_Test TC3: diff=%d, should be %d\n", diff, 500);
            break;

         case EV_2:
            diff = SAL_I_Time_Diff(&tm[1], &now);
            if (diff > 1100)
            {
               return false;
            }
            tm_ready[1] = true;
            printf("Single_Shot_Restart_Test TC3: diff=%d, should be %d\n", diff, 1000);
            break;

         case EV_3:
            diff = SAL_I_Time_Diff(&tm[2], &now);
            if (diff > 1550)
            {
               return false;
            }
            tm_ready[2] = true;
            printf("Single_Shot_Restart_Test TC3: diff=%d, should be %d\n", diff, 1500);
            break;
         }
      } while(!tm_ready[0] || !tm_ready[1] || !tm_ready[2]);
   }
   
   SAL_Destroy_Timer(timer1);
   SAL_Stop_Timer(timer2);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);

   printf("Single_Shot_Restart_Test: PASSED\n");
   return true;
}


bool Single_Shot_Stop_Test(void)
{
   bool tm_ready[3] = { false, false, false };
   SAL_I_Time_Spec_T tm[3];
   SAL_Timer_Id_T timer1, timer2, timer3;

   printf("Single_Shot_Stop_Test: BEGIN\n");

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      return false;
   }

   SAL_I_Get_Time(&tm[0]);
   SAL_Start_Timer(timer1, 500, 0);

   SAL_I_Get_Time(&tm[1]);
   SAL_Start_Timer(timer2, 1000, 0);

   SAL_I_Get_Time(&tm[2]);
   SAL_Start_Timer(timer3, 2000, 0);

   do
   {
      uint32_t diff;
      SAL_I_Time_Spec_T now;
      const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
      if (msg == NULL)
      {
         return false;
      }

      SAL_I_Get_Time(&now);
      switch(msg->event_id)
      {
      case EV_1:
         SAL_Stop_Timer(timer2);
         diff = SAL_I_Time_Diff(&tm[0], &now);
         tm_ready[0] = true;
         if (diff > 550)
         {
            return false;
         }
         printf("diff=%d, should be %d\n", diff, 500);
         break;

      case EV_2:
         return false;
         break;

      case EV_3:
         diff = SAL_I_Time_Diff(&tm[2], &now);
         if (diff > 2200)
         {
            return false;
         }
         tm_ready[2] = true;
         printf("diff=%d, should be %d\n", diff, 2000);
         break;
      }
   } while(!tm_ready[0] || !tm_ready[2]);

   SAL_Destroy_Timer(timer1);
   SAL_Stop_Timer(timer2);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);

   printf("Single_Shot_Stop_Test: PASSED\n");
   return true;
}

bool Periodic_Test(void)
{
   uint32_t tm_ticks[3] = { 0, 0, 0 };
   SAL_I_Time_Spec_T tm[3];
   SAL_Timer_Id_T timer1, timer2, timer3;

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      return false;
   }

   SAL_I_Get_Time(&tm[0]);
   SAL_Start_Timer(timer1, 500, 1);

   SAL_I_Get_Time(&tm[1]);
   SAL_Start_Timer(timer2, 1000, 1);

   SAL_I_Get_Time(&tm[2]);
   SAL_Start_Timer(timer3, 5100, 0);

   do
   {
      uint32_t diff;
      SAL_I_Time_Spec_T now;
      const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
      if (msg == NULL)
      {
         return false;
      }

      SAL_I_Get_Time(&now);
      switch(msg->event_id)
      {
      case EV_1:
         tm_ticks[0]++;
         diff = SAL_I_Time_Diff(&tm[0], &now);
         if (diff > (tm_ticks[0]*500+tm_ticks[0]*50)) /* 10 % tolerance */
         {
            return false;
         }
         printf("diff=%d, should be %d\n", diff, tm_ticks[0]*500);
         break;

      case EV_2:
         tm_ticks[1]++;
         diff = SAL_I_Time_Diff(&tm[0], &now);
         if (diff > (tm_ticks[1]*1000+100))
         {
            return false;
         }

         printf("diff=%d, should be %d\n", diff, tm_ticks[1]*1000);
         break;

      case EV_3:
         diff = SAL_I_Time_Diff(&tm[2], &now);
         if (diff > (5100+510))
         {
            return false;
         }
         tm_ticks[2]++;
         printf("diff=%d, should be %d\n", diff, 5100);
         break;
      }
   } while(tm_ticks[2] < 1);

   if ((tm_ticks[0] != 10) || (tm_ticks[1] != 5))
   {
      return false;
   }

   SAL_Destroy_Timer(timer1);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);

   printf("Periodic_Test passed\n");
   return true;
}

bool Interval_0_Test(void)
{
   SAL_Timer_Id_T timer;
 
   if (!SAL_Create_Timer(EV_1, &timer))
   {
      return false;
   }

   /* TC1: Start single shot timer with interval 0
    */
   {
      const SAL_Message_T* msg;

      SAL_Start_Timer(timer, 0, false);
      msg = SAL_Receive_Timeout(0);
      if ((msg == NULL) || (msg->event_id != EV_1))
      {
         return false;
      }
   }

   /* TC2: Start single shot timer with interval 200 and immediately
    * restart the same timer with interval 0.
    */
   {
      const SAL_Message_T* msg;

      SAL_Start_Timer(timer, 200, false);
      SAL_Start_Timer(timer, 0, false);
      msg = SAL_Receive_Timeout(0);
      if ((msg == NULL) || (msg->event_id != EV_1))
      {
         return false;
      }
      msg = SAL_Receive_Timeout(450);
      if (msg != NULL)
      {
         return false;
      }
   }

   /* TC3: Start periodic timer with interval 200 and immediately
    * restart the same timer with interval 0.
    */
   {
      const SAL_Message_T* msg;

      SAL_Start_Timer(timer, 200, true);
      SAL_Start_Timer(timer, 0, false);
      msg = SAL_Receive_Timeout(0);
      if ((msg == NULL) || (msg->event_id != EV_1))
      {
         return false;
      }
      msg = SAL_Receive_Timeout(450);
      if (msg != NULL)
      {
         return false;
      }
   }

#if defined (XSAL)
   /* TC4: Start single shot timer with interval 0
    */
   {
      const SAL_Message_T* msg;

      SAL_Start_Timer_Ex(timer, 0, false, 1);
      msg = SAL_Try_Receive();
      if ((msg == NULL) || (msg->event_id != EV_1) || 
         (msg->data_size != sizeof(uintptr_t)) ||
         (*((uintptr_t*)(msg->data)) != (uintptr_t)1))
      {
         return false;
      }
   }

   /* TC5: Start single shot timer with interval 200 and immediately
    * restart the same timer with interval 0.
    */
   {
      const SAL_Message_T* msg;

      SAL_Start_Timer_Ex(timer, 200, false, 2);
      SAL_Start_Timer_Ex(timer, 0, false, 3);
      msg = SAL_Try_Receive();
      if ((msg == NULL) || (msg->event_id != EV_1) ||
         (msg->data_size != sizeof(uintptr_t)) ||
         (*((uintptr_t*)(msg->data)) != (uintptr_t)3))
      {
         return false;
      }
      msg = SAL_Receive_Timeout(450);
      if (msg != NULL)
      {
         return false;
      }
   }

   /* TC6: Start periodic timer with interval 200 and immediately
    * restart the same timer with interval 0.
    */
   {
      const SAL_Message_T* msg;

      SAL_Start_Timer_Ex(timer, 200, false, 4);
      SAL_Start_Timer_Ex(timer, 0, false, 5);
      msg = SAL_Try_Receive();
      if ((msg == NULL) || (msg->event_id != EV_1) ||
         (msg->data_size != sizeof(uintptr_t)) ||
         (*((uintptr_t*)(msg->data)) != (uintptr_t)5))
      {
         return false;
      }
      msg = SAL_Receive_Timeout(450);
      if (msg != NULL)
      {
         return false;
      }
   }
#endif /* XSAL */

   SAL_Destroy_Timer(timer);
   printf("Interval_0_Test passed\n");

   return true;
}

bool Create_Dummy_Timers(void)
{
   SAL_Timer_Id_T timer1, timer2, timer3;

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      return false;
   }
   return true;
}

bool Clock_Test(void)
{
   SAL_Clock_T t1, t2;
   SAL_Int_T elapsed_time;
   SAL_Int_T delta_time;

   t1 = SAL_Clock();
   SAL_Sleep(250);
   t2 = SAL_Clock(); /* Get timestamp for SAL_Delta_Time test */
   elapsed_time = SAL_Elapsed_Time(t1);
   if ((elapsed_time > 270) || (elapsed_time < 230))
   {
      return false;
   }
   printf("Clock_Test: SAL_Elapsed_Time, elapsed_time=%d, should be %d\n", elapsed_time, 250);

   delta_time = SAL_Delta_Time(t1, t2);
   if ((delta_time > 270) || (delta_time < 230))
   {
      return false;
   }
   printf("Clock_Test: SAL_Delta_Time, elapsed_time=%d, should be %d\n", delta_time, 250);

   delta_time = SAL_Delta_Time(t2, t1);
   if ((delta_time > -230) || (delta_time < -270))
   {
      return false;
   }
   printf("Clock_Test: SAL_Delta_Time, elapsed_time=%d, should be %d\n", delta_time, -250);

   return true;
}

/*
Single Shot:
1. Normal single shot timers
2. Single shot with move to future before expiration time
3. Single shot with stop before expiration time


Periodic:
1. Normal periodic timers.
2. Periodic                with move to future before expiration time
*/
void Test_Thread(void* param)
{
   param = param; /* get rid of "unused variable" warning */

   if (!SAL_Create_Queue(3, 0, malloc, free))
   {
      SAL_Exit(1);
   }

   if (!Single_Shot_Test())
   {
      SAL_Exit(1);
   }
   if (!Single_Shot_Restart_Test())
   {
      SAL_Exit(1);
   }
   if (!Single_Shot_Stop_Test())
   {
      SAL_Exit(1);
   }
   if (!Periodic_Test())
   {
      SAL_Exit(1);
   }
   if (!Interval_0_Test())
   {
      SAL_Exit(1);
   }

#if defined (XSAL)
   if (!Single_Shot_Ex_Test())
   {
      SAL_Exit(1);
   }
   if (!Single_Shot_Restart_Ex_Test())
   {
      SAL_Exit(1);
   }
   if (!Single_Shot_Stop_Ex_Test())
   {
      SAL_Exit(1);
   }
   if (!Periodic_Ex_Test())
   {
      SAL_Exit(1);
   }
#endif /* XSAL */

   /* Create a few timers but do not delete
    */
   if (!Create_Dummy_Timers())
   {
      SAL_Exit(1);
   }

   if (!Clock_Test())
   {
      SAL_Exit(1);
   }
}

bool Test_Suite(void)
{
   int32_t status;
   SAL_Priority_T priority;
   SAL_Thread_Id_T destroyed_list[] = { Single_Shot_Thread };
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);
   (void)SAL_Create_Thread(Test_Thread, NULL, Init_Thread_Attr("TIME_T", Single_Shot_Thread, priority, &thread_attr));
   status = SAL_Run();
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(destroyed_list[0]), Timeout))
   {
      return false;
   }

   /* TODO: Check Free Timers list */
   return status == 0 ? true : false;
}

SAL_Int_T main(SAL_Int_T argc, char* argv[])
{
   bool test_status = false;

   if (Set_Up(argc, argv))
   {
      test_status = Test_Suite();
      Tear_Down();
   }

   printf("Test %s\n", (test_status ? "PASSED" : "FAILED"));
   return test_status ? 0 : 1;;
}

