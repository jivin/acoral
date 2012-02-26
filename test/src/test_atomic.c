#include "acoral.h"
#include"../include/test.h"
acoral_atomic_t atomic;
ACORAL_COMM_THREAD atomictest(void *args){
        acoral_atomic_set(5,&atomic);
    	acoral_print("%d\n",atomic.val);
        acoral_atomic_inc(&atomic);
    	acoral_print("%d\n",atomic.val);
        acoral_atomic_dec(&atomic);
    	acoral_print("%d\n",atomic.val);
        acoral_atomic_add(2,&atomic);
    	acoral_print("%d\n",atomic.val);
        acoral_atomic_sub(3,&atomic);
    	acoral_print("%d\n",atomic.val);
}

void test_atomic_init(){
	acoral_create_thread(atomictest,ACORAL_PRINT_STACK_SIZE,NULL,NULL,26,1);
}

TEST_CALL(test_atomic_init)
