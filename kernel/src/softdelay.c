#include<type.h>
#include<thread.h>
#include<comm_thrd.h>
#include<hal.h>
#include<cpu.h>
#include<policy.h>
acoral_u32 sample_100ms;
volatile acoral_u32 sample;
void delay(){
	volatile acoral_32 tmp=0xfff;
	while(tmp-->0);
	sample++;
}

void delay_task(void *args){
	sample=0;
	for(;;){
		delay();
	}
}

void acoral_soft_delay(acoral_u32 n100ms){
	acoral_u32 i;
	acoral_u32 tmp=n100ms*sample_100ms;	
	for(i=0;i<tmp;i++)
		delay();
}

void soft_delay_init(){
	acoral_sr cpu_sr;
	acoral_comm_policy_data_t data;
	acoral_thread_t *thread;
	acoral_id tmp_id;
	data.cpu=acoral_current_cpu;
	data.prio=ACORAL_TMP_PRIO;
	data.prio_type=ACORAL_ABSOLUTE_PRIO;
	tmp_id=acoral_create_thread_ext(delay_task,256,NULL,"softdelay",NULL,ACORAL_SCHED_POLICY_COMM,&data);
	if(tmp_id==-1)
		return;
	acoral_delay_self(1000);
	sample_100ms=sample/10;
	/*这里daemo回收进程还没启动，不能使用acoral_kill_thread*/
	thread=(acoral_thread_t *)acoral_get_res_by_id(tmp_id);
	HAL_ENTER_CRITICAL();
	acoral_unrdy_thread(thread);
	acoral_release_thread((acoral_res_t *)thread);
	HAL_EXIT_CRITICAL();
}
