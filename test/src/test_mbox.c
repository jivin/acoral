/*===============================
 *        邮箱测试
 *       by pegasus
 *===============================*/
#include "acoral.h"
#include"../include/test.h"
static acoral_evt_t * box_evt;
ACORAL_COMM_THREAD task_send()
{
	acoral_8 *msg="12345dd67";
	acoral_delay_self(20);
	acoral_mbox_send(box_evt, (void*)msg);
	if (acoral_mbox_send(box_evt, (void*)msg)	== MBOX_ERR_MES_EXIST)
	{
		acoral_delay_self(120);
		acoral_print("===delay=====");
		acoral_delay_self(128);
		acoral_mbox_send(box_evt, (void*)msg);
	}
	while(1)
		acoral_delay_self(500);
}

ACORAL_COMM_THREAD task_recv1()
{
	acoral_u8 *msg1="";
	msg1 = acoral_mbox_recv(box_evt, 50);
	acoral_print("task_recv1: %s", msg1);
	while(1)
		acoral_delay_self(500);
}

ACORAL_COMM_THREAD task_recv2()
{
	acoral_u8 *msg1="";
	acoral_delay_self(60);
	msg1 = acoral_mbox_recv(box_evt, 50);
	acoral_print("task_recv2: %s",msg1);
	while(1)
		acoral_delay_self(500);
}


void test_mbox_init()
{
    acoral_u32 err;
    box_evt= acoral_mbox_create();
    acoral_create_thread(task_send,ACORAL_PRINT_STACK_SIZE,NULL,NULL,26,0);
    acoral_create_thread(task_recv1,ACORAL_PRINT_STACK_SIZE,NULL,NULL,25,0);
    acoral_create_thread(task_recv2,ACORAL_PRINT_STACK_SIZE,NULL,NULL,20,1);
}

TEST_CALL(test_mbox_init)
