#include <acoral.h>
#include"../include/test.h"
ACORAL_COMM_THREAD test1(){
	acoral_print("In test1 prio is%d\n",acoral_cur_thread->prio);
}

ACORAL_COMM_THREAD test(){
	
	acoral_sched_lock();
	acoral_print("In test after lock,prio is%d\n",acoral_cur_thread->prio);
	acoral_create_thread(test1,128,NULL,"lock test1",10, -1);
	acoral_print("In test before unlock,prio is%d\n",acoral_cur_thread->prio);
	acoral_sched_unlock();
}

void test_lock_init()
{
	acoral_create_thread(test,352,NULL,"lock test",20, -1);
}

TEST_CALL(test_lock_init)
