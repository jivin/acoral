/*===============================
 *    消息测试3  1 vs N发送接收
 *       by pegasus
 *===============================*/
#include"../include/test.h"
#include "acoral.h"
#include "message.h"
static acoral_msgctr_t * pmsgctr;
static acoral_msgctr_t   msgctr;

ACORAL_COMM_THREAD task_send()
{
	acoral_u8 i;
	acoral_msg_t* pmsg;
	acoral_8 *dat1 = "12345dd67";
	acoral_8 *dat2 = "-=-=-=-=-";
	
	acoral_u32  id1  = 1;
	acoral_u32  id2  = 2;

	acoral_msg_t*   pmsg1;
	acoral_u32      err;
	
	
	/*--------------------*/
	/*  消息创建  */
	/*  id1 被接 1 次*/
	/*  id2 被接 2 次*/
	/*--------------------*/
	pmsg = acoral_msg_create(1, &err, id1, -1, dat1 );
	acoral_msg_send (pmsgctr, pmsg);
	
	pmsg = acoral_msg_create(2, &err, id2, -1, dat2 );
	acoral_msg_send (pmsgctr, pmsg);

	while(1)
		acoral_delay_self(500);
}

ACORAL_COMM_THREAD task_recv1()
{
	acoral_u8   *msg1;
	acoral_u32   err;
	acoral_u32  id1 = 1, id2 = 2;

	/*---接收 id1  第一次---*/
	msg1 = acoral_msg_recv(pmsgctr, id1, 100, &err);
	if (msg1 != NULL)
		acoral_print("\ntask_recv1: %s", msg1);
	else
		acoral_print("\ntask_recv1:121212121212");


	/*---接收 id2  第一次---*/
	msg1 = acoral_msg_recv(pmsgctr, id2, 100, &err);
	if (msg1 != NULL)
		acoral_print("\ntask_recv1: %s", msg1);
	else
		acoral_print("\ntask_recv1:121212121212");

	while(1)
		acoral_delay_self(500);
}

ACORAL_COMM_THREAD task_recv2()
{
	acoral_u8   *msg1;
	acoral_u32   err;
	acoral_u32  id1 = 1, id2 = 2;

	/*---接收 id1  第二次---*/
	/*---因id1只收一次，会因没有而收不到---*/
	msg1 = acoral_msg_recv(pmsgctr, id1, 100, &err);
	if (msg1 != NULL)
		acoral_print("\ntask_recv2: %s", msg1);
	else
		acoral_print("\ntask_recv2:121212121212");

	/*---接收 id2 第二次---*/
	msg1 = acoral_msg_recv(pmsgctr, id2, 100, &err);
	if (msg1 != NULL)
		acoral_print("\ntask_recv2: %s", msg1);
	else
		acoral_print("\ntask_recv2:121212121212");

	while(1)
		acoral_delay_self(500);
}


static void test_msg_init()
{
	acoral_u32 err;
	pmsgctr = acoral_msgctr_create(&err);
	acoral_create_thread(task_send, ACORAL_PRINT_STACK_SIZE,NULL,NULL,26,0);
	acoral_create_thread(task_recv1,ACORAL_PRINT_STACK_SIZE,NULL,NULL,25,0);
	acoral_create_thread(task_recv2,ACORAL_PRINT_STACK_SIZE,NULL,NULL,20,1);
}

TEST_CALL(test_msg_init)
