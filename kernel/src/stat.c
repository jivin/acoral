#include <type.h>
#include <thread.h>
#include <comm_thrd.h>
#include <cpu.h>
#include <policy.h>
#include <hal.h>
acoral_u32 sample_10ms=0;
acoral_32 cpu_usage[HAL_MAX_CPU];
volatile acoral_u32 idle_count[HAL_MAX_CPU];
void stat(void *args){
	acoral_u32 i;
	acoral_sr cpu_sr;
	while(1){
	     for(i=0;i<HAL_MAX_CPU;i++)
			idle_count[i]=0;
	      acoral_delay_self(1000);
	      for(i=0;i<HAL_MAX_CPU;i++){
			cpu_usage[i]=100-idle_count[i]*10/sample_10ms;/* (100/10)*/
			if(cpu_usage[i]<0)
				cpu_usage[i]=0;
	       }
	}
}

acoral_32 acoral_get_cpu_usage(acoral_u32 cpu){
	cpu=cpu>=HAL_MAX_CPU?HAL_MAX_CPU-1:cpu;
	return cpu_usage[cpu];
}

void stat_init(){
	acoral_sr cpu_sr;
	acoral_comm_policy_data_t data;
	idle_count[acoral_current_cpu]=0;
	acoral_delay_self(100);
	sample_10ms=idle_count[acoral_current_cpu];
	data.cpu=acoral_current_cpu;
	data.prio=ACORAL_STAT_PRIO;
	data.prio_type=ACORAL_ABSOLUTE_PRIO;
	acoral_create_thread_ext(stat,256,NULL,"stat",NULL,ACORAL_SCHED_POLICY_COMM,&data);
}
