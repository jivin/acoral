#include<kernel.h>
#define _GNU_SOURCE
#include<hal_port.h>
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
extern pthread_t cpus_tid[];
extern void acoral_start();
extern volatile acoral_u32 acoral_start_sched;
int main(){
  pthread_t id;
  pthread_attr_t attr;
  cpu_set_t cpuset;
  pthread_attr_init( &attr );
  pthread_create(&id,&attr,(void *)acoral_start,NULL);
  CPU_ZERO(&cpuset );
  CPU_SET(0, &cpuset);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  acoral_intr_disable();
  while(!acoral_start_sched);
  while(1){
    	usleep(1000000/ACORAL_TICKS_PER_SEC);

#ifdef CFG_CMP
	pthread_kill(cpus_tid[0],HAL_TICKS_INTR);
#else
	pthread_kill(id,HAL_TICKS_INTR);
#endif
  }
  pthread_join(id,NULL);
  return 0;
}
