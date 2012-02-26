#include"acoral.h"
#include"../include/test.h"
acoral_spinlock_t lock;
ACORAL_COMM_THREAD spintest(){
  	acoral_sr sr;
	acoral_spin_lock_stamp(&lock);
	acoral_print("1 own lock\n");
	acoral_spin_unlock_stamp(&lock);
	acoral_print("1 open lock\n");
}

ACORAL_COMM_THREAD spintest1(){
  	acoral_sr sr;
	acoral_delay_self(2);
	acoral_spin_lock_stamp(&lock);
	acoral_print("2 own lock\n");
	acoral_spin_unlock_stamp(&lock);
	acoral_print(" 2 open lock\n");
}

void test_spin_init()
{
	acoral_spin_init(&lock);
	acoral_create_thread(spintest, 24, 128,NULL,NULL,0);
	acoral_create_thread(spintest1, 25, 128,NULL,NULL,1);
}
TEST_CALL(test_spin_init)
