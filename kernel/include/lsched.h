#ifndef SCHED_H
#define SCHED_H
#include<type.h>
#include<cpu.h>
#include<thread.h>
extern acoral_u8 need_sched[];
extern acoral_u8 sched_lock[];
extern acoral_thread_t *running_thread[],*ready_thread[];
typedef struct{
/*	acoral_spin_lock lock;*/
	acoral_prio_array_t array;
}acoral_rdy_queue_t;

void acoral_sched_init();
#define acoral_sched_lock_state() (sched_lock[acoral_current_cpu])
#define acoral_sched_lock() (sched_lock[acoral_current_cpu]=1)
void acoral_sched_unlock();
#define acoral_sched_state() (need_sched[acoral_current_cpu])
#define acoral_set_sched_state(val) (need_sched[acoral_current_cpu]=(val))
#define acoral_get_running_thread(cpu) (running_thread[(cpu)])
#define acoral_get_cur_thread() (running_thread[acoral_current_cpu])
void acoral_set_running_thread(acoral_thread_t *thread);
#define acoral_get_ready_thread() (ready_thread[acoral_current_cpu])
#define acoral_set_ready_thread(thread) (ready_thread[acoral_current_cpu]=(thread))

#define acoral_cur_thread acoral_get_cur_thread()
#define acoral_ready_thread acoral_get_ready_thread()
#define acoral_need_sched acoral_sched_state()
#define acoral_set_need_sched(val) acoral_set_sched_state(val)
#define acoral_sched_is_lock acoral_sched_lock_state()
void acoral_thread_runqueue_init(void);
void acoral_rdyqueue_add(acoral_thread_t *new);
void acoral_rdyqueue_del(acoral_thread_t *old);
acoral_u32 acoral_get_idlest_cpu(void);
void acoral_sched();
void acoral_select_thread();
#endif
