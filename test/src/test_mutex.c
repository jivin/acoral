#include "acoral.h"
#include"../include/test.h"
acoral_evt_t * mutex_evt;
ACORAL_COMM_THREAD test1()
{
   acoral_delay_self(10);
   acoral_print("Test1:before pend-my prio is%d\n",acoral_cur_thread->prio);
   acoral_mutex_pend(mutex_evt, 0);
   acoral_delay_self(400);
   acoral_print("Test1:after pend-my prio is%d\n",acoral_cur_thread->prio);
/*   acoral_mutex_post(mutex_evt);*/
   acoral_print("Test1:after post-my prio is%d\n",acoral_cur_thread->prio);
       	while(1){
		acoral_delay_self(500);
		acoral_print("Test1\n");
	}
}

ACORAL_COMM_THREAD test2()
{

   acoral_delay_self(200);
   acoral_print("Test2:before pend\n");
   acoral_mutex_pend(mutex_evt, 100);
 /*  acoral_delay_self(100);*/
 /*  acoral_print("Test2:after pend1\n");*/
 /*  acoral_mutex_post(mutex_evt);*/
 /*  acoral_print("Test2:after post\n");*/
   while(1){
		acoral_delay_self(500);
		acoral_print("Test2\n");
  }
}

ACORAL_COMM_THREAD test3()
{

   acoral_delay_self(200);
   acoral_print("Test3:before pend\n");
   acoral_mutex_pend(mutex_evt, 100);
 /*  acoral_delay_self(100);*/
 /*  acoral_print("Test3:after pend1\n");*/
 /*  acoral_mutex_post(mutex_evt);*/
 /*  acoral_print("Test3:after post\n");*/
   while(1){
		acoral_delay_self(500);
		acoral_print("Test3\n");
  }
}


void test_mutex_init()
{
    acoral_u32 err;
    mutex_evt= acoral_mutex_create(15,NULL);
    acoral_create_thread(test3,ACORAL_PRINT_STACK_SIZE,NULL,NULL,24, 0);
    acoral_create_thread(test1,ACORAL_PRINT_STACK_SIZE,NULL,NULL,26, 0);
/*   acoral_create_thread(test2,ACORAL_PRINT_STACK_SIZE,NULL,NULL,20, 1);*/
}

TEST_CALL(test_mutex_init)
