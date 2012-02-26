#include<type.h>
#include<cpu.h>
#include<queue.h>
#include<thread.h>
#include<print.h>
#include<hal.h>
#include<lsched.h>
#include<policy.h>
#include<mem.h>
#include<timer.h>
#include<posix_thrd.h>
#define POSIX_SLICE_RATE 5
void posix_delay_deal(){
	acoral_thread_t *cur;
	posix_policy_data_t *data;
#ifndef CFG_TICKS_PRIVATE
	acoral_u32 i;
	for(i=0;i<HAL_MAX_CPU;i++){
		cur=acoral_get_running_thread(i);
#else
		cur=acoral_cur_thread;
#endif
		if(cur->policy==ACORAL_SCHED_POLICY_POSIX){
			cur->slice--;
			if(cur->slice==0){
				data=(posix_policy_data_t *)cur->private_data;
				data->stair++;
				if(data->stair>=ACORAL_POSIX_PRIO_MAX){
					if(data->time<ACORAL_POSIX_STAIR_NUM){
						data->stair=ACORAL_POSIX_PRIO_MIN+data->time;
						data->time++;
					}
					else{
						cur->policy=ACORAL_SCHED_POLICY_SLICE;
						((slice_policy_data_t *)data)->slice_ld=TIME_TO_TICKS(data->time<<POSIX_SLICE_RATE);
						data->stair--;
					}
				}
				cur->slice=TIME_TO_TICKS(data->time<<POSIX_SLICE_RATE);
				acoral_thread_change_prio(cur,data->stair);
			}
		}
#ifndef CFG_TICKS_PRIVATE
	}
#endif
}

void acoral_posix_create(acoral_id* id,acoral_posix_policy_data_t *data,void (*route)(void *args),void *args){
	acoral_thread_t *thread;
	thread=acoral_alloc_thread();
	if(NULL==thread){
		acoral_printerr("Alloc Posix thread fail\n");
		acoral_printk("No Mem Space or Beyond the max thread\n");
		return ;
	}
	thread->stack_size=0;
	thread->stack_buttom=NULL;
	thread->name=NULL;
	thread->policy=ACORAL_SCHED_POLICY_POSIX;
	if(id!=NULL)
		*id=posix_policy_thread_init(thread,route,args,data);
	else
		posix_policy_thread_init(thread,route,args,data);
}

acoral_id posix_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data){
	acoral_sr cpu_sr;
	acoral_posix_policy_data_t *policy_data;
	posix_policy_data_t *private_data;
	policy_data=(acoral_posix_policy_data_t *)data;
	private_data=(posix_policy_data_t *)posix_malloc(sizeof(posix_policy_data_t));
	if(private_data==NULL){
		acoral_printerr("No mem space for posix private_data:%s\n",thread->name);
		HAL_ENTER_CRITICAL();
		acoral_release_res((acoral_res_t *)thread);
		HAL_EXIT_CRITICAL();
		return -1;
	}
	posix_policy_data_init(private_data);
	if(policy_data!=NULL){
		private_data->attach_state=policy_data->attach_state;
		private_data->cpu_mask=policy_data->cpu_mask;
		private_data->cancel_type=policy_data->cancel_type;
		private_data->cancel_enable=policy_data->cancel_enable;
		thread->stack_size=policy_data->stack_size;
	}
	thread->private_data=private_data;
	if(thread->stack_size==0)
		thread->stack_size=POSIX_DEFAULT_STACK_SIZE;
	thread->cpu=-1;
	thread->cpu_mask=-1;
	thread->prio=private_data->stair;
	thread->slice=TIME_TO_TICKS(private_data->time<<POSIX_SLICE_RATE);
	if(acoral_thread_init(thread,route,acoral_thread_exit,args)!=0){
		acoral_printerr("No thread stack:%s\n",thread->name);
		HAL_ENTER_CRITICAL();
		acoral_release_res((acoral_res_t *)thread);
		HAL_EXIT_CRITICAL();
		return -1;
	}
	acoral_resume_thread(thread);
	return thread->res.id;
}

void posix_policy_thread_release(acoral_thread_t *thread){
	posix_free(thread->private_data);
}

void posix_policy_data_init(posix_policy_data_t *private_data){
	private_data->attach_state=POSIX_JOINABLE;
	private_data->cancel_type=POSIX_CANCEL_DEFER;
	private_data->cancel_enable=true;
	private_data->cpu_mask=0xff;
	private_data->stair=ACORAL_POSIX_PRIO_MIN;
	private_data->time=1;
}

acoral_sched_policy_t posix_policy;
void posix_policy_init(){
	posix_policy.type=ACORAL_SCHED_POLICY_POSIX;
	posix_policy.policy_thread_init=posix_policy_thread_init;
	posix_policy.policy_thread_release=posix_policy_thread_release;
	posix_policy.delay_deal=posix_delay_deal;
	posix_policy.name="posix";
	acoral_register_sched_policy(&posix_policy);
}
