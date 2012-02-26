#include<type.h>
#include<queue.h>
#include<thread.h>
#include<print.h>
#include<hal.h>
#include<policy.h>
#include<mem.h>
#include<timer.h>
#include<period_thrd.h>
acoral_queue_t period_delay_queue;
acoral_id period_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data){
	acoral_sr cpu_sr;
	acoral_u32 prio;
	acoral_period_policy_data_t *policy_data;
	period_policy_data_t *private_data;
	if(thread->policy==ACORAL_SCHED_POLICY_PERIOD){
		policy_data=(acoral_period_policy_data_t *)data;
		thread->cpu=policy_data->cpu;
		prio=policy_data->prio;
		if(policy_data->prio_type==ACORAL_BASE_PRIO){
			prio+=ACORAL_BASE_PRIO_MIN;
			if(prio>=ACORAL_BASE_PRIO_MAX)
				prio=ACORAL_BASE_PRIO_MAX-1;
		}
		thread->prio=prio;
		private_data=(period_policy_data_t *)acoral_malloc2(sizeof(period_policy_data_t));
		if(private_data==NULL){
			acoral_printerr("No level2 mem space for private_data:%s\n",thread->name);
			HAL_ENTER_CRITICAL();
			acoral_release_res((acoral_res_t *)thread);
			HAL_EXIT_CRITICAL();
			return -1;
		}
		private_data->time=policy_data->time;
		private_data->route=route;
		private_data->args=args;
		thread->private_data=private_data;
		thread->cpu_mask=-1;
	}
	if(acoral_thread_init(thread,route,period_thread_exit,args)!=0){
		acoral_printerr("No thread stack:%s\n",thread->name);
		HAL_ENTER_CRITICAL();
		acoral_release_res((acoral_res_t *)thread);
		HAL_EXIT_CRITICAL();
		return -1;
	}
        /*将线程就绪，并重新调度*/
	acoral_resume_thread(thread);
	HAL_ENTER_CRITICAL();
	period_thread_delay(thread,((period_policy_data_t *)thread->private_data)->time);
	HAL_EXIT_CRITICAL();
	return thread->res.id;
}

void period_policy_thread_release(acoral_thread_t *thread){
	acoral_free2(thread->private_data);	
}

void acoral_periodqueue_add(acoral_thread_t *new){
	acoral_list_t   *tmp,*head;
	acoral_thread_t *thread;
	acoral_32  delay2;
	acoral_32  delay= new->delay;
	head=&period_delay_queue.head;
	acoral_spin_lock(&head->lock);
	new->state|=ACORAL_THREAD_STATE_DELAY;
	for (tmp=head->next;delay2=delay,tmp!=head; tmp=tmp->next){
		thread = list_entry (tmp, acoral_thread_t, waiting);
		delay  = delay-thread->delay;
		if (delay < 0)
			break;
	}
	new->delay = delay2;
	acoral_list_add(&new->waiting,tmp->prev);
	/* 插入等待任务后，后继等待任务时间处理*/
	if(tmp != head){
		thread = list_entry(tmp, acoral_thread_t, waiting);
		thread->delay-=delay2;
	}
	acoral_spin_unlock(&head->lock);
}

void period_thread_delay(acoral_thread_t* thread,acoral_time time){
	thread->delay=TIME_TO_TICKS(time);
	acoral_periodqueue_add(thread);
}

void period_delay_deal(){
	acoral_list_t *tmp,*tmp1,*head;
	acoral_thread_t * thread;
	period_policy_data_t * private_data;
	head=&period_delay_queue.head;
	if(acoral_list_empty(head))
	    	return;
	thread=list_entry(head->next,acoral_thread_t,waiting);
	ACORAL_ASSERT(thread,"in period thread deal");
	thread->delay--;
	for(tmp=head->next;tmp!=head;){
		thread=list_entry(tmp,acoral_thread_t,waiting);
		ACORAL_ASSERT(thread,"in period thread deal ");
		if(thread->delay>0)
		    break;
		private_data=thread->private_data;
		/*防止add判断delay时取下thread*/
		acoral_spin_lock(&head->lock);
		tmp1=tmp->next;
		acoral_list_del(&thread->waiting);
		tmp=tmp1;
		acoral_spin_unlock(&head->lock);
		if(thread->state&ACORAL_THREAD_STATE_SUSPEND){
			thread->stack=(acoral_u32 *)((acoral_8 *)thread->stack_buttom+thread->stack_size-4);
			HAL_STACK_INIT(&thread->stack,private_data->route,period_thread_exit,private_data->args);
			acoral_rdy_thread(thread);
		}
		period_thread_delay(thread,private_data->time);
	}
}

void period_thread_exit(){
	acoral_suspend_self();
}

acoral_sched_policy_t period_policy;
void period_policy_init(){
	acoral_list_init(&period_delay_queue.head);
	acoral_spin_init(&period_delay_queue.head.lock);
	period_policy.type=ACORAL_SCHED_POLICY_PERIOD;
	period_policy.policy_thread_init=period_policy_thread_init;
	period_policy.policy_thread_release=period_policy_thread_release;
	period_policy.delay_deal=period_delay_deal;
	period_policy.name="period";
	acoral_register_sched_policy(&period_policy);
}
