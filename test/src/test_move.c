#include "acoral.h"
#include"../include/test.h"
acoral_id id;
ACORAL_COMM_THREAD test(){
	acoral_print("In thread mover\n");
	acoral_soft_delay(10);
	acoral_print("I will Move goods thread to cpu1,please waiting 5s\n");
	acoral_soft_delay(50);
	acoral_moveto_thread_by_id(id,1);
	while(1){
		acoral_print("In mover thread-cpu:%d\n",acoral_current_cpu);
		acoral_soft_delay(36);
	}
}

ACORAL_SLICE_THREAD test1(){
	acoral_print("In thread which be moved\n");
	while(1){
		acoral_print("In goods thread-cpu:%d\n",acoral_current_cpu);
		acoral_soft_delay(9);
	}
}

void move_init()
{
	acoral_create_thread(test,ACORAL_PRINT_STACK_SIZE,1,"mover",60,0);
	id=acoral_create_thread(test1,ACORAL_PRINT_STACK_SIZE,1,"goods",62,0);
}

TEST_CALL(move_init)
