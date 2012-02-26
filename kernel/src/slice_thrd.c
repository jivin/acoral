#include<type.h>
#include<cpu.h>
#include<queue.h>
#include<thread.h>
#include<lsched.h>
#include<print.h>
#include<hal.h>
#include<policy.h>
#include<timer.h>
#include<mem.h>
#include<slice_thrd.h>
acoral_sched_policy_t slice_policy;
acoral_id slice_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data){
	acoral_sr cpu_sr;
	acoral_u32 prio;
	acoral_slice_policy_data_t *policy_data;
	slice_policy_data_t *private_data;
	if(thread->policy==ACORAL_SCHED_POLICY_SLICE){
		policy_data=(acoral_slice_policy_data_t *)data;
		thread->cpu=policy_data->cpu;
		prio=policy_data->prio;
		if(policy_data->prio_type==ACORAL_BASE_PRIO){
			prio+=ACORAL_BASE_PRIO_MIN;
			if(prio>=ACORAL_BASE_PRIO_MAX)
				prio=ACORAL_BASE_PRIO_MAX-1;
		}
		thread->prio=prio;
		private_data=(slice_policy_data_t *)acoral_malloc2(sizeof(slice_policy_data_t));
		if(private_data==NULL){
			acoral_printerr("No level2 mem space for private_data:%s\n",thread->name);
			HAL_ENTER_CRITICAL();
			acoral_release_res((acoral_res_t *)thread);
			HAL_EXIT_CRITICAL();
			return -1;
		}
		private_data->slice_ld=TIME_TO_TICKS(policy_data->slice);
		thread->slice=private_data->slice_ld;
		thread->private_data=private_data;
		thread->cpu_mask=-1;
	}
	if(acoral_thread_init(thread,route,acoral_thread_exit,args)!=0){
		acoral_printerr("No thread stack:%s\n",thread->name);
		HAL_ENTER_CRITICAL();
		acoral_release_res((acoral_res_t *)thread);
		HAL_EXIT_CRITICAL();
		return -1;
	}
        /*将线程就绪，并重新调度*/
	acoral_resume_thread(thread);
	return thread->res.id;
}

void slice_policy_thread_release(acoral_thread_t *thread){
	acoral_free2(thread->private_data);	
}

void slice_delay_deal(){
	acoral_thread_t *cur;
	slice_policy_data_t *data;
#ifndef CFG_TICKS_PRIVATE
	acoral_u32 i;
	for(i=0;i<HAL_MAX_CPU;i++){
		cur=acoral_get_running_thread(i);
#else
		cur=acoral_cur_thread;
#endif
		if(cur->policy==ACORAL_SCHED_POLICY_SLICE){
			cur->slice--;
			if(cur->slice<=0){
				data=(slice_policy_data_t *)cur->private_data;
				cur->slice=data->slice_ld;
				acoral_thread_move2_tail(cur);
			}
		}
#ifndef CFG_TICKS_PRIVATE
	}
#endif
}

void slice_policy_init(){
	slice_policy.type=ACORAL_SCHED_POLICY_SLICE;
	slice_policy.policy_thread_release=slice_policy_thread_release;
	slice_policy.policy_thread_init=slice_policy_thread_init;
	slice_policy.delay_deal=slice_delay_deal;
	slice_policy.name="slice";
	acoral_register_sched_policy(&slice_policy);
}
