#include "acoral.h"
#include"../include/test.h"
ACORAL_COMM_THREAD test(){
	acoral_print("Mem test\n");
	acoral_8 * p1,*p2;
	acoral_print("Mem test malloc1\n");
	p1=acoral_malloc2(4);
	acoral_print("Mem test malloc2\n");
	p2=acoral_malloc2(20);
	acoral_print("Mem test free2\n");
	acoral_free2(p2);
	acoral_print("Mem test free1\n");
	acoral_free2(p1);
	acoral_print("Err free action\n");
	acoral_free2(p1);
}

ACORAL_COMM_THREAD test1(){
	acoral_print("Mem test1\n");
	acoral_8 * p1,*p2;
	p1=acoral_malloc2(4);
	p2=acoral_malloc2(20);
	acoral_mem_scan2();
	acoral_free2(p2);
	acoral_mem_scan2();
	acoral_free2(p1);
	acoral_mem_scan2();
}


void test_mem2_init()
{
	acoral_create_thread(test, ACORAL_PRINT_STACK_SIZE,NULL,"Mem2test",42, 0);
	acoral_create_thread(test1, ACORAL_PRINT_STACK_SIZE,NULL,"Mem2test1",42, 1);
}

TEST_CALL(test_mem2_init)
