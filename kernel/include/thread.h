/*--------------------*/
/*  acoral_thread_t 增加 timeout*/
/*  链表，用来处理超时，挂g_timeout_queue*/
/*  pegasus  0719*/
/*--------------------*/

#ifndef ACORAL_THREAD_H
#define ACORAL_THREAD_H
#include<autocfg.h>
#include<type.h>
#include<list.h>
#include<resource.h>
#include<queue.h>
#include<event.h>

#define ACORAL_BASE_PRIO  1<<1
#define ACORAL_ABSOLUTE_PRIO 1<<2 

#define ACORAL_IDLE_PRIO ACORAL_MINI_PRIO
#define ACORAL_TMP_PRIO ACORAL_MINI_PRIO-1
#define ACORAL_STAT_PRIO ACORAL_MINI_PRIO-2
#define ACORAL_DAEMON_PRIO ACORAL_MINI_PRIO-3
#ifndef CFG_THRD_RM
#define CFG_HARD_RT_PRIO_NUM 0
#endif
#define ACORAL_HARD_RT_PRIO_MIN ACORAL_MAX_PRIO+1
#define ACORAL_HARD_RT_PRIO_MAX ACORAL_HARD_RT_PRIO_MIN+CFG_HARD_RT_PRIO_NUM

#ifdef CFG_THRD_POSIX
#define ACORAL_POSIX_PRIO_MAX ACORAL_TMP_PRIO
#define ACORAL_POSIX_PRIO_MIN ACORAL_POSIX_PRIO_MAX-CFG_POSIX_STAIR_NUM
#define ACORAL_BASE_PRIO_MAX ACORAL_POSIX_PRIO_MIN
#else
#define ACORAL_BASE_PRIO_MAX ACORAL_TMP_PRIO
#endif
#define ACORAL_BASE_PRIO_MIN ACORAL_HARD_RT_PRIO_MAX


#define ACORAL_THREAD_STATE_MINI  0
#define ACORAL_THREAD_STATE_READY (1<<ACORAL_THREAD_STATE_MINI)
#define ACORAL_THREAD_STATE_SUSPEND (1<<(ACORAL_THREAD_STATE_MINI+1))
#define ACORAL_THREAD_STATE_RUNNING (1<<(ACORAL_THREAD_STATE_MINI+2))
#define ACORAL_THREAD_STATE_EXIT (1<<(ACORAL_THREAD_STATE_MINI+3))
#define ACORAL_THREAD_STATE_RELEASE (1<<(ACORAL_THREAD_STATE_MINI+4))
#define ACORAL_THREAD_STATE_DELAY (1<<(ACORAL_THREAD_STATE_MINI+5))
#define ACORAL_THREAD_STATE_MOVE (1<<(ACORAL_THREAD_STATE_MINI+6))
#define acoral_init_stack() HAL_INIT_STACK()
typedef struct{
  	acoral_res_t res;
#ifdef CFG_CMP
  	acoral_spinlock_t move_lock;
#endif
	acoral_u8 state;
	acoral_u8 prio;
	acoral_8 cpu;
	acoral_u32 cpu_mask;
	acoral_u8 policy;
	acoral_list_t ready;
	acoral_list_t timeout;
	acoral_list_t waiting;
	acoral_list_t global_list;
	acoral_evt_t *evt;
	acoral_u32 *stack;
	acoral_u32 *stack_buttom;
	acoral_u32 stack_size;
	acoral_32 delay;
	acoral_u32 slice;
	acoral_char *name;
	acoral_id console_id;
	void*	private_data;
	void*	data;
}acoral_thread_t;

#define acoral_create_thread(route,stack_size,args,name,prio,cpu) create_comm_thread(route,stack_size,args,name,prio,cpu);
#define acoral_create_thread_ext(route,stack_size,args,name,stack,policy,policy_data) create_thread_ext(route,stack_size,args,name,stack,policy,policy_data);
void acoral_release_thread(acoral_res_t *thread);
void acoral_suspend_self(void);
void acoral_suspend_thread_by_id(acoral_u32 thread_id);
void acoral_suspend_thread(acoral_thread_t *thread);
void acoral_resume_thread_by_id(acoral_u32 thread_id);
void acoral_resume_thread(acoral_thread_t *thread);
acoral_err acoral_wait_thread(acoral_thread_t *thread);
void acoral_delay_self(acoral_u32 ticks);
void acoral_kill_thread(acoral_thread_t *thread);
void acoral_kill_thread_by_id(acoral_id id);
void acoral_thread_exit(void);
void acoral_wake_waiting(acoral_thread_t *thread);
acoral_err acoral_thread_init(acoral_thread_t *thread,void (*route)(void *args),void (*exit)(void),void *args);
acoral_thread_t *acoral_alloc_thread(void);
void acoral_thread_pool_init(void);
void acoral_waitqueue_del(acoral_thread_t *thread);
void acoral_thread_sys_init(void);
void acoral_unrdy_thread(acoral_thread_t *thread);
void acoral_rdy_thread(acoral_thread_t *thread);
void acoral_thread_move2_tail_by_id(acoral_id thread_id);
void acoral_thread_move2_tail(acoral_thread_t *thread);
extern acoral_queue_t acoral_threads_queue;
#endif

