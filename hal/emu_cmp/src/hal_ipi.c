#include<type.h>
#include<pthread.h>
extern pthread_t cpus_tid[];
void hal_ipi_send(acoral_u32 cpu,acoral_vector vector){
  	pthread_kill(cpus_tid[cpu],vector);
}
