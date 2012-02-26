#include "acoral.h"
#include"../include/test.h"
static void test(int args){
	acoral_print("Period:%ds\n",args);
}

static void test1(){
	acoral_print("In period test1\n");
	acoral_print("Delay funciton doesn't work for period thread\n");
	acoral_print("I will delay myself 4s by acoral_delay_self\n");
	acoral_delay_self(4000);
	acoral_print("I Back after Delay\n");
	acoral_print("Do you see that this thread back soon without delay 4s\n");
}

void period_init()
{
	acoral_period_policy_data_t data;
	data.cpu=0;
	data.prio=25;
	data.prio_type=ACORAL_BASE_PRIO;
	data.time=1220;
	acoral_create_thread_ext(test,ACORAL_PRINT_STACK_SIZE,1,NULL,NULL,ACORAL_SCHED_POLICY_PERIOD,&data);
	data.cpu=1;
	data.prio=26;
	data.time=2010;
	acoral_create_thread_ext(test,ACORAL_PRINT_STACK_SIZE,2,NULL,NULL,ACORAL_SCHED_POLICY_PERIOD,&data);
	data.cpu=1;
	data.prio=20;
	data.time=1700;
	acoral_create_thread_ext(test1,ACORAL_PRINT_STACK_SIZE,NULL,NULL,NULL,ACORAL_SCHED_POLICY_PERIOD,&data);
}

TEST_CALL(period_init)
