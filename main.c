#include "include/xsal.h"

void Thread1() 
{ 
	printf("This is thread1\n");
}

void Thread2() 
{ 
	printf("This is thread2\n");
}

int main(int argc, char* argv[])
{
   SAL_Config_T sal_config;
   SAL_Thread_Attr_T attr; 
   
   /* Retrieve the default configuration parameters. */
   SAL_Get_Config(&sal_config);
   
   /* Define application ID and max number of threads. */
   sal_config.app_id = 7;
   sal_config.max_number_of_threads = 10;
   
   /* Modify some other values. */
   sal_config.first_unknown_thread_id = 5;
   
   /* Initialize XSAL. */
   SAL_Init(&sal_config);

   /* Initialize the system, create threads, etc. */
   attr.name = "Thread1";
   attr.id = 1;
   attr.priority = 2;
   SAL_Create_Thread(Thread1, NULL, &attr);

   attr.name = "Thread2";
   attr.id = 2;
   attr.priority = 5; 
   SAL_Create_Thread(Thread2, NULL, &attr);

   return SAL_Run(); 
}
