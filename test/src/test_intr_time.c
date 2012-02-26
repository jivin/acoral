#include"acoral.h"
#include"../include/test.h"
#define TIME_RANDOM 1
#define PRESCALER1  0
#define TIME PCLK/(8*(PRESCALER1+1)*TIME_RANDOM)
#define M 	10
acoral_u32 data[M];
acoral_u32 time0=0;
acoral_u32 time1=0;
acoral_thread_t *thread_p[M];

void Timer2_init()
{
	rTCON=rTCON &(~0xf000);
	rTCFG0 &=0xff0000;
	rTCFG0 |=(0xF9|(PRESCALER1<<8)); 			//prescaler1=0;
	rTCFG1 &= ~0xf00;
	rTCFG1 |=0x200; 		//time2 divider=8;
	
	rTCNTB2 =TIME;
	rTCON = rTCON &(~0xf000) |0x2000;
	rTCON = rTCON &(~0xf000) |0x9000;
}
void Timer2_handler(acoral_vector vector)
{
	time0=rTCNTO2;
	time1=rTCNTB2;
	time1=time1-time0;
		acoral_print("%d\n",time1);
}
ACORAL_COMM_THREAD test(void* pdata)
{
	acoral_32 test_times=M;
	acoral_intr_attach(IRQ_TIMER2,Timer2_handler);
	Timer2_init();
	acoral_intr_unmask(IRQ_TIMER2);
	acoral_print("test\n");
	while(1)
	{
		acoral_suspend_self();
	}
}
ACORAL_COMM_THREAD TASK_i(void* pdata)
{ 	
	acoral_u32 i=*(acoral_u32 *)pdata;
	while(1)
	{ 	
		acoral_resume_thread(thread_p[(i+1)]);
		acoral_suspend_self();		
	}
}
ACORAL_COMM_THREAD TASK_N(void* pdata)
{
	pdata=pdata;
	while(1)
	{
		acoral_resume_thread(thread_p[0]);
	}
}
void test_intr_time_init()
{ 
	acoral_u32 i,temp;
	acoral_create_thread(test,100,NULL,"test",5,0);
	for(i=0;i<M-1;i++)
	{ 	
		data[i]=i;
		temp=acoral_create_thread(TASK_i,100,&data[i],NULL,6+i,0);
		thread_p[i] = (acoral_thread_t *)acoral_get_res_by_id(temp);
	}
	temp=acoral_create_thread(TASK_N,100,NULL,NULL,5+M,0);
	thread_p[M-1]=(acoral_thread_t *)acoral_get_res_by_id(temp);
}
TEST_CALL(test_intr_time_init)
