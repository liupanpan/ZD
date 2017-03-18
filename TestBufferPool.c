#include <stdio.h>
#include <malloc.h>

#include "include/xsal.h"
#include "include/xsal_i_buffer_pools.h"

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 3
#define MAX_NUMB_OF_BUFFERS 100

typedef enum Thread_Id_Tag
{
   BUFFER_POOL = 1,
   CREATE_BUFFER_POOL,
   READ_BUFFER_POOL
} Thread_Id_T;

typedef struct Test_Case_Tag
{
   size_t numb_of_buffers;
   size_t buffer_size;
} Test_Case_T;

void* Buffer_Id[MAX_NUMB_OF_BUFFERS + 1];

Test_Case_T Test[] = 
{
   { 10, 1 },
   { 11, 2 },
   { 12, 3 },
   { 13, 4 },
   { 14, 5 },
   { 15, 6 },
   { 16, 7 },
   { 17, 8 },
   { 18, 9 },

   { 30, 1*1024 },
   { 50, 2*1024 },
   { 70, 4*1024 },
   { 90, 8*1024 }
};

SAL_Buffer_Pool_Id_T Buffer_Pool_Id_1;
SAL_Buffer_Pool_Id_T Buffer_Pool_Id_2;

SAL_Int_T Timeout = 3000;

const SAL_Thread_Attr_T* Init_Thread_Attr(
         const char* name, 
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

void Buffer_Pool_Fun(void* param)
{
   void* buffer_id[MAX_NUMB_OF_BUFFERS + 1];
   size_t i, j;
   Test_Case_T* par = (Test_Case_T*)param;
   uint8_t* addr;

   printf("Buffer_Pool_Fun: Start %d\n", SAL_Get_Thread_Id());
   
   if (!SAL_Create_Buffer_Pool(par->numb_of_buffers, par->buffer_size, &Buffer_Pool_Id_1))
   {
      SAL_Exit(1);
   }

   for(i = 0; i < par->numb_of_buffers; i++)
   {
      if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) == NULL)
      {
         SAL_Exit(1);
      }
   }

   if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) != NULL)
   {
      SAL_Exit(1);
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      for (j = 0; j < par->buffer_size; j++)
      {
         addr = (uint8_t*)buffer_id[i] + j;

         *addr = (uint8_t)((par->numb_of_buffers + i) % 256);
      }
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      for (j = 0; j < par->buffer_size; j++)    
      {
         addr = (uint8_t*)buffer_id[i] + j;

         if (*addr != (par->numb_of_buffers + i) % 256)
         {
            SAL_Exit(1);            
         }
      }
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(buffer_id[i]);
   }

   for(i = 0; i < par->numb_of_buffers; i++)
   {
      if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) == NULL)
      {
         SAL_Exit(1);
      }
 
   }

   if ((buffer_id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_1)) != NULL)
   {
      SAL_Exit(1);
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(buffer_id[i]);
   }

   printf("Buffer_Pool_Fun: End %d\n", SAL_Get_Thread_Id());
}

void Create_Buffer_Pool_Fun(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { READ_BUFFER_POOL };
   Test_Case_T* par = (Test_Case_T*)param;
   size_t i, j;
   uint8_t* addr;

   printf("Create_Buffer_Pool_Fun: Start %d\n", SAL_Get_Thread_Id());

   if (!SAL_Create_Buffer_Pool(par->numb_of_buffers, par->buffer_size, &Buffer_Pool_Id_2))
   {
      SAL_Exit(1);
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
       if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) == NULL)
      {
         SAL_Exit(1);
      }
   }

   if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) != NULL)
   {
      SAL_Exit(1);
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      for (j = 0; j < par->buffer_size; j++)
      {
         addr = (uint8_t*)Buffer_Id[i] + j;

         *addr = (uint8_t)((par->numb_of_buffers + i) % 256);
      }
   }
   SAL_Signal_Ready();

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      SAL_Exit(1);
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(Buffer_Id[i]);
   }

   for(i = 0; i < par->numb_of_buffers; i++)
   {
      if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) == NULL)
      {
         SAL_Exit(1);
      }
   }

   if ((Buffer_Id[i] = SAL_Alloc_Buffer(Buffer_Pool_Id_2)) != NULL)
   {
      SAL_Exit(1);
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      SAL_Free_Buffer(Buffer_Id[i]);
   }
   
   printf("Create_Buffer_Pool_Fun: End %d\n", SAL_Get_Thread_Id());
}

void Read_Buffer_Pool_Fun(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { CREATE_BUFFER_POOL };
   static SAL_Thread_Id_T destroyed_list [] = { CREATE_BUFFER_POOL };
   Test_Case_T* par = (Test_Case_T*)param;
   size_t i, j;
   uint8_t* addr;

   printf("Read_Buffer_Pool_Fun: Start %d\n", SAL_Get_Thread_Id());

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      SAL_Exit(1);
   }

   for (i = 0; i < par->numb_of_buffers; i++)
   {
      for (j = 0; j < par->buffer_size; j++)
      {

         addr = (uint8_t*)Buffer_Id[i] + j;

         if (*addr != (par->numb_of_buffers + i) % 256)
         {
            SAL_Exit(1);            
         }
                                
      }

   }

   SAL_Signal_Ready();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      SAL_Exit(1);
   }

   printf("Read_Buffer_Pool_Fun: End %d\n", SAL_Get_Thread_Id());
}

SAL_Int_T Run_Thread(Test_Case_T data)
{
   SAL_Int_T status;
   Test_Case_T param;
   SAL_Thread_Id_T destroyed_list[] = { CREATE_BUFFER_POOL, READ_BUFFER_POOL };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   param.buffer_size = data.buffer_size;
   param.numb_of_buffers = data.numb_of_buffers;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Buffer_Pool_Fun, 
      (void*)&param, 
      Init_Thread_Attr("BP", BUFFER_POOL, priority, &thread_attr));

   SAL_Create_Thread(
      Create_Buffer_Pool_Fun, 
      (void*)&param, 
      Init_Thread_Attr("CBP", CREATE_BUFFER_POOL, priority, &thread_attr));

   SAL_Create_Thread(
      Read_Buffer_Pool_Fun, 
      (void*)&param, 
      Init_Thread_Attr("RBP", READ_BUFFER_POOL, priority, &thread_attr));

   status = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      status = 1;
   }
   
   return status;
}

SAL_Int_T Buffer_Pool_Test()
{
   SAL_Int_T ret = 0;
 
   ret = ret || Run_Thread(Test[0]);
   ret = ret || Run_Thread(Test[1]);
   ret = ret || Run_Thread(Test[2]);
   ret = ret || Run_Thread(Test[3]);
   ret = ret || Run_Thread(Test[4]);
   ret = ret || Run_Thread(Test[5]);
   ret = ret || Run_Thread(Test[6]);
   ret = ret || Run_Thread(Test[7]);
   ret = ret || Run_Thread(Test[8]);

   ret = ret || Run_Thread(Test[9]);
   ret = ret || Run_Thread(Test[10]);
   ret = ret || Run_Thread(Test[11]);
   ret = ret || Run_Thread(Test[12]);

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

   ret = ret || Buffer_Pool_Test();

   Tear_Down();

   printf("Test %s\n", ((ret) ? ("FAILED") : ("PASSED")));

   return ret;
}
