#include"acoral.h"
#include"../include/test.h"
/*一共设置10个任务，任务0-任务9依次执行，
*任务i设置任务i+1的“开始时间”,然后挂起，切换到任务i+1
*任务i+1开始执行后，再用“激活时间”-“开始时间”即是任务切换时间。
*此过程一共执行K次，取平均值。
*/
#define TASK_NUM 		10		//设置TASK_NUM个任务
#define K			100 		//进行K次抢占
#define M 			10000 		//外层进行M次循环抢占过程
acoral_thread_t * thread_p[TASK_NUM];
acoral_u32 	thread_tick1[TASK_NUM]; 	//线程i被调度器选中的时间
acoral_u32 	thread_tick2[TASK_NUM]; 	//线程i开始执行的时间
acoral_u32 	thread_tick3[TASK_NUM]; 	//线程i的切换时间
acoral_u32 	Task_num[TASK_NUM];
acoral_u32 	Test_num=M;
void OS_SUSPEND_AND_RESUME(acoral_thread_t * thread)
{
	acoral_sr cpu_sr;
	HAL_ENTER_CRITICAL();
	acoral_rdyqueue_del(acoral_cur_thread);
	HAL_EXIT_CRITICAL();
	acoral_resume_thread(thread);
}
void Task0(void* pdata)
{
	acoral_u32 i=0,temp=0;
	pdata=pdata;
	while(--Test_num>0)
	{
		for(i=0;i<K;i++)
		{ 
			if(i!=0)
				thread_tick2[0]  = acoral_ticks;
			else
				thread_tick2[0]  = 0;
			thread_tick3[0]  += (thread_tick2[0]-thread_tick1[0]);
			thread_tick1[1]=acoral_ticks;		
			OS_SUSPEND_AND_RESUME(thread_p[1]);
		}
		for(i=0;i<TASK_NUM;i++)
		{
			thread_tick1[i]=0;
			thread_tick2[i]=0;			
			acoral_print("%d\t",(thread_tick3[i]));
		}
		acoral_print("\n");
	}
	for(i=0;i<TASK_NUM;i++)
	{
		 temp +=thread_tick3[i];
	}
	acoral_print("\n%d:\t%d\n",(M*K*TASK_NUM),temp);
	while(1){}
       		
}
void Task_i(void* pdata)
{

	acoral_u32 j=(*(acoral_u32*)pdata);
	acoral_suspend_self(); 	
	while(1)
	{
		thread_tick2[j]  = acoral_ticks;
		thread_tick3[j]  += (thread_tick2[j]-thread_tick1[j]);
		
		thread_tick1[(j+1)%TASK_NUM] = acoral_ticks;
		OS_SUSPEND_AND_RESUME(thread_p[(j+1)%TASK_NUM]);
	}
}
void test_sw_init()
{
	acoral_u32 i;
	acoral_32 temp=-1;
	for(i=0;i<TASK_NUM;i++)
	{
		thread_tick1[i]=0;
		thread_tick2[i]=0;
		thread_tick3[i]=0;
		Task_num[i]=i;
	}
	temp=acoral_create_thread(Task0,100,NULL,NULL,TASK_NUM-1,0);
	thread_p[0]=(acoral_thread_t *)acoral_get_res_by_id(temp);
	for(i=1;i<TASK_NUM;i++)
	{
		temp=acoral_create_thread(Task_i,100,(void*)&Task_num[i],NULL,i-1,0);
		thread_p[i]=(acoral_thread_t*)acoral_get_res_by_id(temp);
	}
}
TEST_CALL(test_sw_init);
