#include <stdio.h>
#include <malloc.h>
#include <sys/timeb.h>

#include "include/xsal.h"

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 12

typedef enum Thread_Id_Tag {
   MAIN_THREAD = 1,
   LOCK1_THREAD,
   LOCK2_THREAD,
   LOCK3_THREAD,
   LOCK4_THREAD,
   LOCK5_THREAD,
   LOCK6_THREAD,
   UNLOCK1_THREAD,
   UNLOCK2_THREAD,
   TRYLOCK1_THREAD,
   TRYLOCK2_THREAD,
   REC_THREAD
} Thread_Id_T;

typedef enum Test_Nr_Tag {
   LONG_TIMEOUT = 1,
   SHORT_TIMEOUT
} Test_Nr_T;

typedef struct Param_Tag {
   Test_Nr_T test_nr;
   SAL_Thread_Id_T destroyed_list[2];
   size_t list_size;
} Param_T;

Param_T Par;

SAL_Mutex_T Mutex;

short Flag = 0;
SAL_Int_T Long_Timeout = 3000;

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

void Lock_Timeout_Fun_11(void* param)
{
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;
   bool ret_timeout;

   printf("Lock_Timeout_Fun_11: Start\n");

   ftime(&time_1);

   /* lock the mutex */
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         printf("SAL_Lock_Mutex_Timeout: suspends the thread\n");
         SAL_Exit(1);     
      }
   }
   else /* !ret_timeout */
   {
      printf("SAL_Lock_Mutex_Timeout: thread doesn't lock the mutex\n");
      SAL_Exit(1);
   }

   SAL_Signal_Ready();

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD thread is suspended */
   if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      printf("SAL_Lock_Mutex_Timeout: LOCK2_THREAD has not been suspended\n");
      SAL_Exit(1);
   }
  
   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      printf("SAL_Unlock_Mutex: returns 0\n");
      SAL_Exit(1);
   }

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD unlocked the mutex */
   if (SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      printf("SAL_Unlock_Mutex: Doesn't unlock the mutex\n");
      SAL_Exit(1);
   }

   printf("Lock_Timeout_Fun_11: Stop\n");
}

/**  Function of LOCK2_THREAD.
 *
 *   - tries to lock the mutex which is already locked
 *   - finally locks the mutex because timeout is long enough (first mutex in LOCK1_THREAD
 *     is unlocked)
 *   - unlocks the mutex
 */

void Lock_Timeout_Fun_12(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { LOCK1_THREAD };
   struct timeb time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool ret_timeout;

   printf("Lock_Timeout_Fun_12: Start");

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      printf("Timeout has been reached\n");
      SAL_Exit(1);
   }

   ftime(&time_1);

   /* try to lock the mutex which is locked by LOCK1_THREAD */
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, 2000);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   res_time = delta_time - 1000;

   if (ret_timeout)
   {
      if (abs(res_time) > 25)
      {
         printf("SAL_Lock_Mutex_Timeout: error: thread has been suspended for %d ms, expected time: 1000 ms\n", delta_time);
         SAL_Exit(1);
      }
      if (abs(res_time) > 10)
      {
         printf("SAL_Lock_Mutex_Timeout: warning: thread has been suspended for %d ms, expected time: 1000 ms\n", delta_time);
      }
   }
   else /* !ret_timeout */
   {
      printf("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex\n");
      SAL_Exit(1);
   }

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      printf("SAL_Unlock_Mutex: returns 0\n");
      SAL_Exit(1);
   }

   printf("Lock_Timeout_Fun_12: Stop\n");
}


/**  Test of SAL_Lock_Mutex_Timeout and SAL_Unlock_Mutex functions.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Lock_Mutex_Timeout_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] = { LOCK1_THREAD, LOCK2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Lock_Timeout_Fun_11, 
      NULL, 
      Init_Thread_Attr("LOCK1", LOCK1_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Lock_Timeout_Fun_12, 
      NULL, 
      Init_Thread_Attr("LOCK2", LOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      printf("Timeout has been reached\n");
      ret = 1;
   }

   return ret;
}

void Lock_Timeout_Fun_21(void* param)
{
   struct timeb time_1, time_2;
   SAL_Int_T delta_time;
   bool ret_timeout;

   printf("Lock_Timeout_Fun_21: Start\n");

   ftime(&time_1);

   /* lock the mutex */
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         printf("SAL_Lock_Mutex_Timeout: suspends the thread\n");
         SAL_Exit(1);     
      }
   }
   else /* !ret_timeout */
   {
      printf("SAL_Lock_Mutex_Timeout: thread doesn't lock the mutex\n");
      SAL_Exit(1);
   }

   SAL_Signal_Ready();

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD has terminated */
   if (SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      printf("SAL_Lock_Mutex_Timeout: Thread has not been suspended\n");
      SAL_Exit(1);
   }

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      printf("SAL_Unlock_Mutex: returns 0\n");
      SAL_Exit(1);
   }

   printf("Lock_Timeout_Fun_21: Stop\n");
}

void Lock_Timeout_Fun_22(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { LOCK1_THREAD };
   struct timeb time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool ret_timeout;

   printf("Lock_Timeout_Fun_22: Start\n");

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      printf("Timeout has been reached\n");
      SAL_Exit(1);
   }

   ftime(&time_1);

   /* try to lock the mutex which is locked by LOCK1_THREAD */   
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, 100);

   ftime(&time_2);

   delta_time = (SAL_Int_T)((time_2.time - time_1.time)*1000 + time_2.millitm - time_1.millitm);

   res_time = delta_time - 100;

   if (!ret_timeout)
   {
      if (abs(res_time) > 20)
      {
         printf("SAL_Lock_Mutex_Timeout: error: invalid interval of timeout, expected: 100, achieved: %d\n", delta_time);
         SAL_Exit(1);
      }
      if (abs(res_time) > 10)
      {
         printf("SAL_Lock_Mutex_Timeout: warning: invalid interval of timeout, expected: 100, achieved: %d\n", delta_time);
      }
   }
   else /* ret_timeout */
   {
      printf("SAL_Lock_Mutex_Timeout: returns 1\n");
      SAL_Exit(1);
   }

   printf("Lock_Timeout_Fun_22: Stop\n");
}


/**  Test of SAL_Lock_Mutex and SAL_Unlock_Mutex functions.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Lock_Mutex_Timeout_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] = { LOCK1_THREAD, LOCK2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Lock_Timeout_Fun_21, 
      NULL, 
      Init_Thread_Attr("LOCK1", LOCK1_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Lock_Timeout_Fun_22, 
      NULL, 
      Init_Thread_Attr("LOCK2", LOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      printf("Timeout has been reached\n");
      ret = 1;
   }

   return ret;

}

void Try_Lock_Fun_1(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { TRYLOCK2_THREAD };

   printf("Try_Lock_Fun_1: Start\n");

   /* try to lock the mutex */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      printf("SAL_Try_Lock_Mutex: Thread doesn't lock the mutex\n");
      SAL_Exit(1);
   }

   SAL_Signal_Ready();

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      printf("Timeout has been reached\n");
      SAL_Exit(1);
   }

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      printf("SAL_Unlock_Mutex: returns 0\n");
      SAL_Exit(1);
   }

   Flag = 1;

   printf("Try_Lock_Fun_1: Stop\n");
}

void Try_Lock_Fun_2(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { TRYLOCK1_THREAD };

   printf("Try_Lock_Fun_2: Start\n");

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      printf("Timeout has been reached\n");
      SAL_Exit(1);
   }

   if (SAL_Try_Lock_Mutex(&Mutex))
   {
      printf("SAL_Try_Lock_Mutex: returns 1 when mutex is locked\n");
      SAL_Exit(1);
   }

   SAL_Signal_Ready();

   while (!Flag)
   {
      SAL_Sleep(10);
   }

   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      printf("SAL_Try_Lock_Mutex: doesn't lock the mutex after it has been unlockedi\n");
      SAL_Exit(1);
   }

   if (!SAL_Unlock_Mutex(&Mutex))
   {
      printf("SAL_Unlock_Mutex: can't unlock mutex\n");
      SAL_Exit(1);
   }

   printf("Try_Lock_Fun_2: Stop\n");
}

SAL_Int_T Try_Lock_Mutex_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret;
   SAL_Thread_Id_T destroyed_list[] = { TRYLOCK1_THREAD, TRYLOCK2_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Try_Lock_Fun_1, 
      NULL, 
      Init_Thread_Attr("TRY1_T", TRYLOCK1_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Try_Lock_Fun_2, 
      NULL, 
      Init_Thread_Attr("TRY2_T", TRYLOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      printf("Timeout has been reached\n");
      ret = 1;
   }

   return ret;	
}

void Recursive_Lock_Fun(void* param)
{
   Test_Nr_T test_nr = (Test_Nr_T)param;

   printf("Recursive_Lock_Fun, Timeout: %d  Start\n", test_nr);

   if (test_nr != 0)
   { 
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {
         printf("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex\n");
         SAL_Exit(1);
      }
   }

   SAL_Sleep(150);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         printf("LOCK1_THREAD is not suspended\n");
         SAL_Exit(1);
      }
   }

   if (test_nr != 0)
   {  
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {     
         printf("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex for the second timei\n");
         SAL_Exit(1);
      }
   }

   SAL_Sleep(100);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         printf("LOCK1_THREAD is not suspended\n");
         SAL_Exit(1);
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         printf("LOCK2_THREAD is not suspended\n");
         SAL_Exit(1);
      }
   }

   if (test_nr != 0)
   {
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {
         printf("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex for the third timei\n");
         SAL_Exit(1);
      }
   }

   SAL_Sleep(100);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         SAL_Exit(1);
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         SAL_Exit(1);
      }

         if (!SAL_Is_Thread_Alive(LOCK3_THREAD))
         {
			SAL_Exit(1);
		 }
   }

   /* unlock mutex for the first time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(50);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         SAL_Exit(1);
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         SAL_Exit(1);
      }

      if (!SAL_Is_Thread_Alive(LOCK3_THREAD))
      {
         SAL_Exit(1);
      }
   }
   
   /* unlock mutex for the second time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(50);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         SAL_Exit(1);
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         SAL_Exit(1);
      }

      if (!SAL_Is_Thread_Alive(LOCK3_THREAD))
      {
         SAL_Exit(1);
      }
   }

   /* unlock mutex for the third time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   if (test_nr != SHORT_TIMEOUT)
   {
      SAL_Sleep(50);

      if (SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         SAL_Exit(1);
      }
   }

   if (test_nr == 0)
   {
      if (SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         SAL_Exit(1);
      }

      if (SAL_Is_Thread_Alive(LOCK3_THREAD))
      {
         SAL_Exit(1);
      }
   }

   printf("Recursive_Lock_Fun, Timeout: %d  Stop\n", test_nr);
}



/**  Function of LOCK1_THREAD, LOCK2_THREAD and LOCK3_THREAD.
 *
 *   - locks mutex
 *   - unlocks mutex
 */

void Lock_Fun(void* param)
{
   struct timeb in_time_1, in_time_2;
   SAL_Int_T in_delta_time, in_theor_time, in_res_time;
   Param_T par = *(Param_T*)param;
   bool ret_timeout;

   printf("Lock_Fun, Timeout: %d Start\n", par.test_nr);

   if (par.test_nr == LONG_TIMEOUT)
   {
      ftime(&in_time_1);

      /* try to lock mutex which is locked by REC_THREAD */
      ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout);

      ftime(&in_time_2);

      in_delta_time = (SAL_Int_T)((in_time_2.time - in_time_1.time)*1000 + in_time_2.millitm - in_time_1.millitm);

      in_theor_time = 350; 

      in_res_time = in_delta_time - in_theor_time;
      
      if (ret_timeout)
      {
         if (abs(in_res_time) > 60)
         {
            SAL_Exit(1);
         }
         else if (abs(in_res_time) > 50)
         {
         }
         else
         {
         }
      }
      else /* !ret_timeout */
      {
         SAL_Exit(1);
      }

   } /* if test_nr == LONG_TIMEOUT */
   else if (par.test_nr == SHORT_TIMEOUT)
   {
      ftime(&in_time_1);

      /* try to lock mutex which is locked by REC_THREAD */
      ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, 100);

      ftime(&in_time_2);

      in_delta_time = (SAL_Int_T)((in_time_2.time - in_time_1.time)*1000 + in_time_2.millitm - in_time_1.millitm);

      in_res_time = in_delta_time - 100;

      if (!ret_timeout)
      {
         if (abs(in_res_time) > 20)
         {
            SAL_Exit(1);
         }
         else if (abs(in_res_time) > 10)
         {
         }
         else
         {
         }  
      }
      else /* ret_timeout */
      {
         SAL_Exit(1);
      }     
   } /* if SHORT_TIMEOUT */
   else
   {
   }

   /* unlock the mutex */
   if (par.test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Unlock_Mutex(&Mutex))
      {
         SAL_Exit(1);
      }
   }

   printf("Lock_Fun, Timeout: %d  Stop\n", par.test_nr);
}

void Main_Thread_Fun(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   Test_Nr_T test_nr = (Test_Nr_T)param;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Recursive_Lock_Fun,
      param, 
      Init_Thread_Attr("REC_T", REC_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   Par.test_nr = (Test_Nr_T)param;

   if (test_nr == 0)
   {
      Par.destroyed_list[0] = LOCK2_THREAD;
      Par.destroyed_list[1] = LOCK3_THREAD;
      Par.list_size = 2;
   }
   else
   {
      Par.destroyed_list[0] = 0;
      Par.list_size = 0;
   }

   SAL_Create_Thread(
      Lock_Fun,
      (void*)&Par, 
      Init_Thread_Attr("LOCK1_T", LOCK1_THREAD, priority, &thread_attr));

   if (test_nr == 0)
   {
      SAL_Sleep(100);

      Par.test_nr = (Test_Nr_T)param;
      Par.destroyed_list[0] = LOCK1_THREAD;
      Par.destroyed_list[1] = LOCK3_THREAD;
      Par.list_size = 2;
   

      SAL_Create_Thread(
         Lock_Fun,
         (void*)&Par, 
         Init_Thread_Attr("LOCK2_T", LOCK2_THREAD, priority, &thread_attr));

      SAL_Sleep(100);

      Par.test_nr = (Test_Nr_T)param;
      Par.destroyed_list[0] = LOCK1_THREAD;
      Par.destroyed_list[1] = LOCK2_THREAD;
      Par.list_size = 2;

      SAL_Create_Thread(
         Lock_Fun,
         (void*)&Par, 
         Init_Thread_Attr("LOCK3_T", LOCK3_THREAD, priority, &thread_attr));

   }
}


/**  Tests recursive mutex.
 *
 *   Creates MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Recursive_Mutex_Test(Test_Nr_T test_nr)
{
   SAL_Thread_Id_T destroyed_list_t[] = { REC_THREAD, LOCK1_THREAD, MAIN_THREAD };
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Main_Thread_Fun,
      (void*)test_nr, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (test_nr != 0) /* LONG_TIMEOUT or SHORT_TIMEOUT */
   {
      if (!SAL_Wait_Destroyed_Timeout(destroyed_list_t, sizeof(destroyed_list_t)/sizeof(SAL_Thread_Id_T), Long_Timeout))
      {
         ret = 1;
      }
   }

   return ret;
}

void Recursive_Try_Lock_Fun(void* param)
{
   printf("Recursive_Try_Lock_Fun: Start\n");

   /* lock mutex for the first time */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(150);

   if (SAL_Is_Thread_Alive(LOCK1_THREAD))
   {
      SAL_Exit(1);
   }

   /* lock mutex for the second time */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      SAL_Exit(1);
   }

   /* lock mutex for the third time */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK3_THREAD))
   {
      SAL_Exit(1);
   }

   /* unlock mutex for the first time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK4_THREAD))
   {
      SAL_Exit(1);
   }

   /* unlock mutex for the second time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK5_THREAD))
   {
      SAL_Exit(1);
   }

   /* unlock mutex for the third time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      SAL_Exit(1);
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK6_THREAD))
   {
      SAL_Exit(1);
   }

   printf("Recursive_Try_Lock_Fun: Stop\n");
}

void Try_Lock_Fun(void* param)
{
   SAL_Int_T lock = *(SAL_Int_T*)param;

   printf("Try_Lock_Fun: Start\n");

   /* mutex should not be locked */
   if (!lock)
   {
      if (SAL_Try_Lock_Mutex(&Mutex))
      {
         SAL_Exit(1);
      }
   }
   /* mutex should be locked */
   else /* lock */
   {
      if (!SAL_Try_Lock_Mutex(&Mutex))
      {
         SAL_Exit(1);
      }

      if (!SAL_Unlock_Mutex(&Mutex))
      {
         SAL_Exit(1);
      }
   }
   
   printf("Try_Lock_Fun: Stop\n");
}

void Main_Thread_Try_Fun(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T lock;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Recursive_Try_Lock_Fun,
      NULL, 
      Init_Thread_Attr("REC_T", REC_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* mutex should not be locked */
   lock = false;

   /* after first try lock in REC_THREAD */
   SAL_Create_Thread(
      Try_Lock_Fun,
      (void*)&lock, 
      Init_Thread_Attr("LOCK1_T", LOCK1_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after second try lock in REC_THREAD */
   SAL_Create_Thread(
      Try_Lock_Fun,
      (void*)&lock, 
      Init_Thread_Attr("LOCK2_T", LOCK2_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after third try lock in REC_THREAD */
   SAL_Create_Thread(
      Try_Lock_Fun,
      (void*)&lock, 
      Init_Thread_Attr("LOCK3_T", LOCK3_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after first unlock in REC_THREAD */
   SAL_Create_Thread(
      Try_Lock_Fun,
      (void*)&lock, 
      Init_Thread_Attr("LOCK4_T", LOCK4_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after second unlock in REC_THREAD */
   SAL_Create_Thread(
      Try_Lock_Fun,
      (void*)&lock, 
      Init_Thread_Attr("LOCK5_T", LOCK5_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* mutex should be locked */
   lock = true;

   /* after third unlock in REC_THREAD */
   SAL_Create_Thread(
      Try_Lock_Fun,
      (void*)&lock, 
      Init_Thread_Attr("LOCK6_T", LOCK6_THREAD, priority, &thread_attr));
}

SAL_Int_T Recursive_Try_Mutex_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Main_Thread_Try_Fun,
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

	ret = SAL_Run();   

   return ret;
}

SAL_Int_T Mutex_Test_Suite()
{
   SAL_Int_T ret = 0;
   SAL_Mutex_Attr_T mutex_attr;

   SAL_Init_Mutex_Attr(&mutex_attr);

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      return 1;
   }

   ret = ret || Lock_Mutex_Timeout_Test_1();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      return 1;
   }

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      return 1;
   }

   ret = ret || Lock_Mutex_Timeout_Test_2();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      return 1;
   }

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      return 1;
   }

   ret = ret || Try_Lock_Mutex_Test(); 

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      return 1;
   }

   /* tests for recursive mutex */

   mutex_attr.recursive = true;

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      return 1;
   }

   ret = ret || Recursive_Mutex_Test(LONG_TIMEOUT);

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      return 1;
   }

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      return 1;
   }

   ret = ret || Recursive_Try_Mutex_Test();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      return 1;
   }


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


SAL_Int_T main(SAL_Int_T argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up())
   {
      printf("Set_Up failed\n");
      SAL_Exit(1);
   }

   ret = ret || Mutex_Test_Suite();

   Tear_Down();

   printf("Test %s\n", ((ret)?("FAILED"):("PASSED")));

   return ret;
}


