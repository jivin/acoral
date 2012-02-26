#include "acoral.h"
#include"../include/test.h"
ACORAL_COMM_THREAD test1(){
	acoral_print("in test1,this thread's period is 1s\n");
	while(1){
		acoral_delay_self(1000);
		acoral_print("in test1\n");
	}
}

ACORAL_COMM_THREAD test2(){
	acoral_print("in test2,this thread's period is 2s\n");
	while(1){
		acoral_delay_self(4001);
		acoral_print("in test2\n");
	}
}

ACORAL_COMM_THREAD test3(acoral_time time){
	while(1){
		acoral_delay_self(time);
	}
}


void test_delay_init()
{
	acoral_u32 i;
	acoral_id id;
	acoral_create_thread(test1, ACORAL_PRINT_STACK_SIZE,NULL,"test1",22, 0);
	acoral_create_thread(test2, ACORAL_PRINT_STACK_SIZE,NULL,"test2",23, 1);
	for(i=0;i<34;i++){
		acoral_print("%d\n",i);
		if(i==31)
			acoral_print("%d\n",i);
		id=acoral_create_thread(test3,256,500+i*10,"deley",i+1, -1);
	}
}

TEST_CALL(test_delay_init)
