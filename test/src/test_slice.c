#include "acoral.h"
#include"../include/test.h"
ACORAL_SLICE_THREAD test(){
	while(1){
		acoral_print("In slice test\n");
		acoral_soft_delay(10);
	}
}

ACORAL_SLICE_THREAD test1(){
	while(1){
		acoral_print("In slice test1\n");
		acoral_soft_delay(10);
	}
}

ACORAL_SLICE_THREAD test2(){
	while(1){
		acoral_print("In slice test2\n");
		acoral_soft_delay(10);
	}
}

void slice_init()
{
	acoral_slice_policy_data_t data;
	data.cpu=1;
	data.prio=25;
	data.prio_type=ACORAL_BASE_PRIO;
	data.slice=1000;
	acoral_create_thread_ext(test,ACORAL_PRINT_STACK_SIZE,1,"slice",NULL,ACORAL_SCHED_POLICY_SLICE,&data);
	data.cpu=1;
	data.prio=25;
	data.slice=2000;
	acoral_create_thread_ext(test1,ACORAL_PRINT_STACK_SIZE,2,"slice1",NULL,ACORAL_SCHED_POLICY_SLICE,&data);
	data.cpu=1;
	data.prio=25;
	data.slice=4000;
	acoral_create_thread_ext(test2,ACORAL_PRINT_STACK_SIZE,3,"slice2",NULL,ACORAL_SCHED_POLICY_SLICE,&data);

}

TEST_CALL(slice_init)
