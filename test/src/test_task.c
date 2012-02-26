#include "acoral.h"
#include"../include/test.h"
int num1=0;
ACORAL_COMM_THREAD test(acoral_u32 num){
	num1=num;
	acoral_print("in cpu:%dtask%d\n",acoral_current_cpu,num);
}

void test_task_init()
{
	acoral_u32 i;
	for(i=0;i<CFG_TEST_TASK_NUM-1;i++)
		acoral_create_thread(test,ACORAL_PRINT_STACK_SIZE,i,"task",i+1, -1);
}

TEST_CALL(test_task_init)
