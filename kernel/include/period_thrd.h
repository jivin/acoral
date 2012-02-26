#ifndef PERIOD_THRD_H
#define PERIOD_THRD_H
#include<type.h>
#include<thread.h>
typedef struct{
	acoral_8 cpu;
	acoral_u8 prio;
	acoral_8 prio_type;
	acoral_time time;
}acoral_period_policy_data_t;
typedef struct{
	acoral_time time;
	void (*route)(void *args);
	void *args;
}period_policy_data_t;

void period_thread_exit();
void period_thread_delay(acoral_thread_t* thread,acoral_time time);
void period_delay_deal();
acoral_id period_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data);
#endif
