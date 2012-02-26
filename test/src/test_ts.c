#include "driver.h"
#include"../include/test.h"
#include "ts.h"
#include "kernel.h"
acoral_ts_info ts_info;
acoral_id ts_id;
static void test(void *args){
	ts_id=acoral_dev_open("ts");
	acoral_dev_read(ts_id,&ts_info,0,0,0);
}
void ts_test(){
	acoral_create_thread(test,ACORAL_PRINT_STACK_SIZE,NULL,NULL,26,1);
}

TEST_CALL(ts_test)
