#include"acoral.h"
#include"../include/test.h"
acoral_evt_t * sem_evt;
/*
ACORAL_COMM_THREAD  test1()
{
	acoral_sem_pend(sem_evt);
	acoral_delay_self(100);
	acoral_print("\ndelay--1--%d", acoral_sem_getnum(sem_evt));
	acoral_print("\n=======%d", acoral_sem_getnum(sem_evt));
	acoral_sem_post(sem_evt);
	while(1)
	{
		acoral_delay_self(6000);
		acoral_print("\ntest11111111");
	}
}

ACORAL_COMM_THREAD test2()
{
	acoral_sem_pend(sem_evt, 0);
	acoral_delay_self(200);
	acoral_print("\n========%d", acoral_sem_getnum(sem_evt));
	acoral_print("\ndelay--2--%d", acoral_sem_getnum(sem_evt));
	acoral_sem_post(sem_evt);
	while(1)
	{
		acoral_delay_self(1000);
		acoral_print("\ntest2222222222");
	}
}

ACORAL_COMM_THREAD  test3()
{
	acoral_sem_pend(sem_evt, 0);
	acoral_delay_self(300);
	acoral_print("\ndelay--3--%d", acoral_sem_getnum(sem_evt));
	acoral_print("\n========%d", acoral_sem_getnum(sem_evt));
	acoral_sem_post(sem_evt);
	while(1)
	{
		acoral_delay_self(2000);
		acoral_print("\ntest33333333333");
	}
}

ACORAL_COMM_THREAD  test4()
{
	acoral_sem_pend(sem_evt, 0);
	acoral_delay_self(400);
	acoral_print("\ndelay--4--%d", acoral_sem_getnum(sem_evt));
	acoral_print("\n=====%d", acoral_sem_getnum(sem_evt));
	acoral_sem_post(sem_evt);
	while(1)
	{
		acoral_delay_self(3000);
		acoral_print("\ntest4444444444");
	}
}
*/

/*====================================*/
/*        pv 操作测试*/
/*====================================*/
ACORAL_COMM_THREAD  test1()
{
	acoral_sem_pend(sem_evt, 0);
	acoral_delay_self(100);
	acoral_print("\ndelay--1--%d", acoral_sem_getnum(sem_evt));
	acoral_print("\n=======%d", acoral_sem_getnum(sem_evt));
	acoral_sem_post(sem_evt);
	while(1)
	{
		acoral_delay_self(6000);
		acoral_print("\ntest11111111");
	}
}

ACORAL_COMM_THREAD test2()
{
	acoral_sem_pend(sem_evt, 0);
	acoral_delay_self(200);
	acoral_print("\n========%d", acoral_sem_getnum(sem_evt));
	acoral_print("\ndelay--2--%d", acoral_sem_getnum(sem_evt));
	acoral_sem_post(sem_evt);
	while(1)
	{
		acoral_delay_self(1000);
		acoral_print("\ntest2222222222");
	}
}

ACORAL_COMM_THREAD  test3()
{
	acoral_delay_self(300);
	acoral_print("\ndelay--3--%d", acoral_sem_getnum(sem_evt));
	acoral_print("\n========%d", acoral_sem_getnum(sem_evt));

	acoral_delay_self(2000);
	acoral_print("\ntest33333333333");
	acoral_sem_post(sem_evt);
	
	acoral_delay_self(2000);
	acoral_print("\ntest33333333333");
	acoral_sem_post(sem_evt);
	
	acoral_delay_self(2000);
	acoral_print("\ntest33333333333");
	acoral_sem_post(sem_evt);

	while(1)
	{
		acoral_delay_self(2000);
		acoral_print("\ntest33333333333");
	}
}

ACORAL_COMM_THREAD  test4()
{
	acoral_sem_pend(sem_evt, 0);
	acoral_delay_self(400);
	acoral_print("\ndelay--4--%d", acoral_sem_getnum(sem_evt));
	acoral_print("\n=====%d", acoral_sem_getnum(sem_evt));
	acoral_sem_post(sem_evt);
	while(1)
	{
		acoral_delay_self(3000);
		acoral_print("\ntest4444444444");
	}
}


void test_sem_init()
{
  	acoral_u32 i;
	acoral_u8 t = 0;
	sem_evt = acoral_sem_create(t);
	acoral_create_thread(test1,ACORAL_PRINT_STACK_SIZE,NULL,NULL,22,0);
	acoral_create_thread(test2,ACORAL_PRINT_STACK_SIZE,NULL,NULL,23,1);
	acoral_create_thread(test3,ACORAL_PRINT_STACK_SIZE,NULL,NULL,24,1);
	acoral_create_thread(test4,ACORAL_PRINT_STACK_SIZE,NULL,NULL,25,1);
}

TEST_CALL(test_sem_init)
