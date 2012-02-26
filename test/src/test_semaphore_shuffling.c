#include"acoral.h"
#include"../include/test.h"
/*
*本程序测量信号量混洗时间，即:一个因为申请信号量失败而挂起的任务，从信号量被释放到开始恢复执行所消耗的时间。
*任务1先获取信号量，然后在第一次测试过程中，挂起，导致任务2执行，任务0因为试图获取信号量被挂起，然后任务2
*开始执行，唤醒任务1，任务1继续执行，直到释放信号量，此处取时间time，任务0获取信号量成功再继续执行，直到释
*放信号量，将自身挂起。一次测试过程结束。反复此过程。
*/
#define TASK_NUM 3
acoral_evt_t* mutex_evt;
acoral_thread_t * 	thread_p[TASK_NUM];
acoral_u32      	prio_num[TASK_NUM];
acoral_32 time=0;
acoral_u32 times=0;
acoral_u8 	bit=1;
void OS_SUSPEND_AND_RESUME(acoral_thread_t *thread)
{
	acoral_sr cpu_sr;
	HAL_ENTER_CRITICAL();
	acoral_rdyqueue_del(acoral_cur_thread);
	HAL_EXIT_CRITICAL();
	acoral_resume_thread(thread);
}
ACORAL_COMM_THREAD task0(void* pdata)
{
	acoral_u32 time_get,priot;
	acoral_32 	temp;
	priot=*(acoral_u32*)pdata;
	while(1)
	{
		acoral_suspend_self();
		acoral_mutex_pend(mutex_evt,0);
		time_get=rTCNTO0;
		temp=(acoral_32)(time-time_get);
		acoral_print("task%d:\ttime:%d\n",priot,temp);
		acoral_mutex_post(mutex_evt);
		bit=0;
		acoral_suspend_self();
	}
}
ACORAL_COMM_THREAD task1(void* pdata)
{
	acoral_32 prio;
	prio=*(acoral_u32*)pdata;
	while(1)
	{
		acoral_mutex_pend(mutex_evt,0);
		OS_SUSPEND_AND_RESUME(thread_p[0]);
		time=rTCNTO0;
		acoral_mutex_post(mutex_evt);
	}
}
ACORAL_COMM_THREAD task2(void* pdata)
{
	pdata=pdata;
	while(1)
	{
		acoral_sched_lock();
		if(bit==0)
		{
			acoral_resume_thread(thread_p[0]);
			bit=1;
		}
		acoral_resume_thread(thread_p[1]);
		acoral_sched_unlock();
	}
}
void test_semaphore_shuffling_init()
{ 
	acoral_u32 i,temp;
	for(i=0;i<TASK_NUM;i++)
		prio_num[i]=i+7;
	mutex_evt = acoral_mutex_create(6,NULL);
	
	temp=acoral_create_thread(task0,100,&prio_num[0],"task0",7,0);
	thread_p[0]=(acoral_thread_t*)acoral_get_res_by_id(temp);
	
	temp=acoral_create_thread(task1,100,&prio_num[1],"task1",8,0);
	thread_p[1]=(acoral_thread_t*)acoral_get_res_by_id(temp);
	
	temp=acoral_create_thread(task2,100,NULL,"task2",9,0);
	thread_p[TASK_NUM-1]=(acoral_thread_t *)acoral_get_res_by_id(temp);
}
TEST_CALL(test_semaphore_shuffling_init);
