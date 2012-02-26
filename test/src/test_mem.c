#include "acoral.h"
#include"../include/test.h"
typedef struct{
   int a;
   int b;
	long c;
}usr_ctrl_t;
acoral_pool_ctrl_t usr_pool_ctrl;
ACORAL_COMM_THREAD test(){
/*	acoral_u32 * p1;
	acoral_u32 * p2;
	acoral_u32 * p3,*p4;
	p1=acoral_malloc(1024);
	p2=acoral_malloc(1024);
	p3=acoral_malloc(1024);
	p4=acoral_malloc(1024);
	acoral_free(p1);
//	acoral_free(p1);
	acoral_free(p1);
	acoral_free(p2);
	acoral_free(p3);
	acoral_free(p4);
//	acoral_free(0x601a8b9);
*/
	usr_ctrl_t *a,*b,*c;	
	usr_pool_ctrl.size=sizeof(usr_ctrl_t);
	usr_pool_ctrl.num_per_pool=10;
	usr_pool_ctrl.max_pools=3;
	acoral_obj_pool_init(&usr_pool_ctrl);
	a=acoral_get_obj(&usr_pool_ctrl);
	acoral_print("Obj %x\n",a);
	b=acoral_get_obj(&usr_pool_ctrl);
	acoral_print("Obj %x\n",b);
	acoral_free_obj(b);
	c=acoral_get_obj(&usr_pool_ctrl);
	acoral_print("Obj %x\n",c);
}

void test_mem_init()
{
	acoral_create_thread(test, ACORAL_PRINT_STACK_SIZE,NULL,"Memtest",42, 0);
//	acoral_create_thread(test, ACORAL_PRINT_STACK_SIZE,NULL,"Memtest",42, 1);
}

TEST_CALL(test_mem_init)
