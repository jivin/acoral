#ifndef RM_THRD_H
#define RM_THRD_H
#include<type.h>
#include<thread.h>
#include<period_thrd.h>
typedef struct{
	acoral_u32 t;
	acoral_u32 e;
}acoral_rm_policy_data_t;
typedef struct{
	period_policy_data_t period_data;
	acoral_u32 t;
	acoral_u32 lt;
	acoral_u32 e;
	acoral_u32 le;
}rm_policy_data_t;
acoral_id rm_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data);
void rm_queue_ascend_add(acoral_thread_t *new);
void rm_policy_thread_release(acoral_thread_t *thread);
void rm_discard_thread(acoral_thread_t *thread);
acoral_u8 rm_check(acoral_thread_t *thread,acoral_u32 cpu);
void rm_thread_dispatch();
#endif
