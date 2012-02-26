#ifndef SLICE_THRD_H
#define SLICE_THRD_H
#include<type.h>
#include<thread.h>
typedef struct{
	acoral_8 cpu;
	acoral_u8 prio;
	acoral_u8 prio_type;
	acoral_u32 slice;
}acoral_slice_policy_data_t;

typedef struct{
	acoral_u32 slice_ld;
}slice_policy_data_t;

void slice_delay_deal();
acoral_id slice_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data);
#endif
