#ifndef POLICY_H
#define POLICY_H
#include<type.h>
#include<list.h>
#include<thread.h>
#define ACORAL_COMM_THREAD static void 
#define ACORAL_RM_THREAD static void 
#define ACORAL_SLICE_THREAD static void 
#define ACORAL_POSIX_THREAD static void 

#define ACORAL_SCHED_POLICY 20
#define ACORAL_SCHED_POLICY_COMM ACORAL_SCHED_POLICY+1
#define ACORAL_SCHED_POLICY_PERIOD ACORAL_SCHED_POLICY+2
#define ACORAL_SCHED_POLICY_RM ACORAL_SCHED_POLICY+3
#define ACORAL_SCHED_POLICY_SLICE ACORAL_SCHED_POLICY+4
#define ACORAL_SCHED_POLICY_POSIX ACORAL_SCHED_POLICY+5
#define posix_malloc(x) acoral_malloc2(x)
#define posix_free(x) acoral_free2(x)
typedef struct{
	acoral_list_t list;
	acoral_u8 type;
	acoral_id (*policy_thread_init)(acoral_thread_t *,void (*route)(void *args),void *,void *);
	void (*policy_thread_release)(acoral_thread_t *);
	void (*delay_deal)();
	acoral_char *name;
}acoral_sched_policy_t;

acoral_id create_thread_ext(void (*route)(void *args),acoral_u32 stack_size,void *args,acoral_char *name,void *stack,acoral_u32 sched_policy,void *data);
void acoral_policy_delay_deal();
void acoral_register_sched_policy(acoral_sched_policy_t *policy);
acoral_sched_policy_t *acoral_get_policy_ctrl(acoral_u8 type);
acoral_id acoral_policy_thread_init(acoral_u32 policy,acoral_thread_t *thread,void (*route)(void *args),void *args,void *data);
void acoral_sched_policy_init();
#endif
