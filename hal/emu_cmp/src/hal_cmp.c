#include <type.h>
#include <cpu.h>
#include <print.h>
#include <hal.h>
#define GNU_SOURCE
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/sysinfo.h>
#include<unistd.h>
#define __USE_GNU
#include<sched.h>
#include<ctype.h>
#include<string.h> 
#include<pthread.h> 

#define self_id pthread_self()
acoral_u32 cpus_active[HAL_MAX_CPU];
acoral_u32 cpus_tid[HAL_MAX_CPU];
volatile acoral_u32 cmp_begin;
volatile acoral_u32 cpus_ack=1;

acoral_u32 hal_cpu_is_active(acoral_u32 cpu){
  	return cpus_active[cpu];
}

void hal_core_cpu_init(){
     cpus_tid[0]=self_id;
     cpus_active[0]=1; 
     cmp_begin=0;
}

void hal_follow_cpu_init(){
      acoral_u32 cpu=acoral_current_cpu;
      cpus_active[cpu]=1;
}

void hal_cmp_ack(){
     cpus_ack=0;
}

void hal_wait_ack(){
    while(cpus_ack);
}

void hal_cpu_bridge(acoral_u32 cpu){
	pthread_t id;
	id=self_id;
  	cpus_tid[cpu]=id;
	acoral_start();
}

void hal_start_cpu(acoral_u32 cpu){
      pthread_t id;
      pthread_attr_t attr;
      cpu_set_t cpuset;
      pthread_attr_init(&attr);
      pthread_create(&id,&attr,(void *)hal_cpu_bridge,(void *)cpu);
      pthread_detach(id);
      CPU_ZERO(&cpuset);
      CPU_SET(cpu, &cpuset);
     pthread_attr_setaffinity_np (&attr, sizeof(cpu_set_t), &cpuset );
      hal_wait_ack();
}

acoral_32 hal_get_current_cpu(){
     pthread_t id;
     acoral_u32 i;
     id=self_id;     
     for(i=0;i<HAL_MAX_CPU;i++)
	    if(cpus_tid[i]==id)
	      	return i;
     return -1;
}
