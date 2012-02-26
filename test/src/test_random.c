#include "acoral.h"
#include"../include/test.h"
ACORAL_COMM_THREAD test(){
	acoral_u32 i;
	acoral_u32 str[20];
	acoral_rand_start();
	for(i=0;i<40;i++){
		acoral_sprint(str,sizeof(str),"%u\t",acoral_rand());
		acoral_print(str);
	}
}

void test_random_init()
{
	acoral_print("Random test Init\n");
	acoral_create_thread(test, ACORAL_PRINT_STACK_SIZE,NULL,"Random",22, 0);
}

TEST_CALL(test_random_init)
