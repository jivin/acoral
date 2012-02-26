/*===============================
 *    消息测试1  发送消息个数i
 *    及消息超时，等待任务超时
 *       by pegasus
 *===============================*/
#include "acoral.h"
#include "message.h"
#include"../include/test.h"

static acoral_msgctr_t * pmsgctr;
static acoral_msgctr_t   msgctr;

ACORAL_COMM_THREAD task_send()
{
	acoral_u8 i;

	acoral_8 *dat1 = "12345dd67";
	acoral_8 *dat2 = "-=-=-=-=-";
	
	acoral_u32  id1  = 1;
	acoral_u32  id2  = 2;

	acoral_msg_t*   pmsg;
	acoral_u32      err;
	
	acoral_delay_self(20);
	
	/*--------------------*/
	/*  消息创建*/
	/*--------------------*/
	for (i=0; i<12; i++)
	{
		pmsg = acoral_msg_create(2, &err, id1, 30, dat1 );
		err = acoral_msg_send (pmsgctr, pmsg);
		acoral_print("\n=== %d ===", err);
	}

	acoral_print("\n======================");
	acoral_print_all_msg(pmsgctr);

	acoral_print("\n======================");
	acoral_print_waitThreadsNum(pmsgctr);

	acoral_delay_self(100);

	acoral_print("\n======================");
	acoral_print_all_msg(pmsgctr);

	acoral_print("\n======================");
	acoral_print_waitThreadsNum(pmsgctr);


	while(1)
		acoral_delay_self(500);
}

static void test_msg_init()
{
    acoral_u32 err;
    pmsgctr = acoral_msgctr_create(&err);
    acoral_create_thread(task_send, ACORAL_PRINT_STACK_SIZE,NULL,NULL,26,0);
}

TEST_CALL(test_msg_init)
