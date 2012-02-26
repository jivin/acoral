/*======================================
 *       测试上下文切换时间
 *       by pegasus
 *   06.03 实现一个核心上测试任务切换时间
 *    
 *======================================*/

#include "acoral.h"
#include"../include/test.h"

#define TIMERS   10
#define PER      30000
static acoral_thread_t *pta, *ptb, *ptc;
acoral_u32 tick_sw1[TIMERS];
acoral_u32 tick_sw2[TIMERS];

ACORAL_COMM_THREAD task_a()
{
	acoral_32 i,j;
	acoral_u32 tick1,tick2;
	
	/* 测试交替悬置/恢复任务时间*/
	/* 任务切换时间(两次)+悬置(1次)+恢复(1次)*/
	j = TIMERS;
	while (j-->0)
	{
		i = PER;
		while(i-->0)
		{
			acoral_suspend_self();
		}
	}

	acoral_delay_self(30);
	
	
	/* 悬置/恢复时间*/
	/* 悬置/恢复(各1次)*/
	j = TIMERS;
	while(j-->0)
	{
		i = PER;
		tick1 = acoral_ticks;
		while(i-->0)
		{
			acoral_suspend_thread(ptb);
			acoral_resume_thread(ptb);
		}
		tick2 = acoral_ticks;
		tick_sw2[j] = (tick2 - tick1);
		acoral_print("\ntest for suspend/resume's time. %d of the %d ......\n",j,TIMERS);
		acoral_delay_self(10);
	}
	
	/* 输出测试信息*/
	acoral_print("\n测试任务切换时间，共%d回\n", TIMERS);
	acoral_print("\n每回中切换任务%d次\n",PER);
	for (j=TIMERS-1; j>=0; j--)
	{
		acoral_print("\n");
		acoral_print("tick_sw1 %d=%d   ", j, tick_sw1[j]);
		acoral_print("tick_sw2 %d=%d",    j, tick_sw2[j]);
		acoral_print("\n%d task sw time[%d]: %dms", PER,  j, 
				(tick_sw1[j] - tick_sw2[j])*1000/ACORAL_TICKS_PER_SEC );
	}
}

ACORAL_COMM_THREAD task_b()
{
	acoral_32 i,j;
	acoral_u32 tick1, tick2;
	acoral_delay_self(10);
	
	/* 测试交替悬置/恢复任务时间*/
	/* 任务切换时间(两次)+悬置(1次)+恢复(1次)*/
	j = TIMERS;
	while (j-->0)
	{
		tick1 = acoral_ticks;
		i = PER;
		while (i-->0)
		{
			acoral_resume_thread(pta);
		}
		tick2 = acoral_ticks;
		tick_sw1[j] = (tick2 - tick1);
		acoral_print("\ntest for suspend/resume/taskswitch' time. %d of the %d ......\n",j,TIMERS);
		acoral_delay_self(10);
	}

	/* 悬置/恢复时间*/
	/* 悬置/恢复(各1次)*/
	while(1)
	{}
}

ACORAL_COMM_THREAD task_c()
{
	acoral_32 i = TIMERS;
	acoral_delay_self(5);
	while(i--)
	{
		/*acoral_suspend_self();*/
		/*acoral_print("c");*/
		acoral_delay_self(20);
	}
}


void test_tasksw_init()
{
	acoral_id ta,tb,tc;
	acoral_u32 i;
	for (i=0; i<TIMERS; i++)
	{
		tick_sw1[i] = 0;
		tick_sw2[i] = 0;
	}
	ta = acoral_create_thread(task_a, ACORAL_PRINT_STACK_SIZE, NULL, NULL, 20, 0);
	tb = acoral_create_thread(task_b, ACORAL_PRINT_STACK_SIZE, NULL, NULL, 26, 0);
	tc = acoral_create_thread(task_b, ACORAL_PRINT_STACK_SIZE, NULL, NULL, 17, 1);
	pta = (acoral_thread_t *)acoral_get_res_by_id(ta);
	ptb = (acoral_thread_t *)acoral_get_res_by_id(tb);
	ptc = (acoral_thread_t *)acoral_get_res_by_id(tc);
}

TEST_CALL(test_tasksw_init)
