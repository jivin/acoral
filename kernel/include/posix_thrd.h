#ifndef POSIX_THRD_H
#define POSIX_THRD_H
#include<type.h>
#include<autocfg.h>
#include<queue.h>
#include<thread.h>
#include<slice_thrd.h>
#define ACORAL_POSIX_STAIR_NUM CFG_POSIX_STAIR_NUM
#define POSIX_JOINABLE 1
#define POSIX_DETACH 0
#define POSIX_CANCEL_DEFER 0
#define POSIX_CANCEL_ASYN 1
#define POSIX_SCHED_FIFO 0
#define POSIX_SCHED_RR 1
#define POSIX_SCHED_OTHER 2
#define POSIX_NICE_DEFAULT 20
#define POSIX_BASEPRIO_DEFAULT 40
#define POSIX_TICKS_RATIO 1<<5 
#define POSIX_DEFAULT_STACK_SIZE 5*1024
typedef struct{
	acoral_char *name;
	acoral_u8 attach_state;
	acoral_u8 cancel_type;
	acoral_u8 cancel_enable;
	acoral_u32 cpu_mask;
	acoral_u32 stack_size;
}acoral_posix_policy_data_t;

typedef struct{
	slice_policy_data_t data;
	acoral_u8 attach_state;
	acoral_u8 cancel_type;
	acoral_u8 cancel_enable;
	acoral_u8 cpu_mask;
	acoral_u8 stair;
	acoral_u8 time;
}posix_policy_data_t;

acoral_id posix_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data);
void posix_policy_data_init(posix_policy_data_t *private_data);
void acoral_posix_create(acoral_id* id,acoral_posix_policy_data_t *data,void (*route)(void *args),void *args);
#endif
