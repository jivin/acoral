#include "acoral.h"
#include"../include/test.h"
ACORAL_COMM_THREAD test1(){
  while(1){
	acoral_delay_self(100);
  	acoral_soft_delay(10);
  }
}

ACORAL_COMM_THREAD test2(){
  acoral_u32 i;
  acoral_print("in test2\n");
  while(1){
  	acoral_delay_self(2020);
        for(i=0;i<HAL_MAX_CPU;i++){
		acoral_print("cpu%d usage:%d\n",i,acoral_get_cpu_usage(i));
	}
  }
}

void test_stat_init()
{
	acoral_create_thread(test1, ACORAL_PRINT_STACK_SIZE,NULL,NULL,23, -1);
	acoral_create_thread(test2, ACORAL_PRINT_STACK_SIZE,NULL,NULL,22, -1);
}

TEST_CALL(test_stat_init)
