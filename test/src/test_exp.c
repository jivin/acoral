#include <acoral.h>
#include"../include/test.h"
volatile acoral_u32 * exp_adr=0xffffffff;
ACORAL_COMM_THREAD test(){
	acoral_print("Enter Exception test\n");
	*exp_adr=0;
	/*这个是缓冲作用*/
	*exp_adr=1;
}

void test_exp_init(){
	acoral_create_thread(test,ACORAL_PRINT_STACK_SIZE,NULL,"Exception Test",26,1);
}

TEST_CALL(test_exp_init)
