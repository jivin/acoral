#include<type.h>
#include<queue.h>
#include<thread.h>
#include<print.h>
#include<hal.h>
#include<cpu.h>
#include<lsched.h>
#include<policy.h>
#include<mem.h>
#include<rm_thrd.h>
acoral_u32 prio=ACORAL_HARD_RT_PRIO_MIN; 
acoral_queue_t tmp_rm_queue;
acoral_queue_t *rm_queue=&tmp_rm_queue;
acoral_fl right[10]={1,0.8284,0.7797,0.7568,0.7435,0.7350,0.7282,0.7240,0.7209,0.7280};
struct{
	acoral_u32 num;
	acoral_fl sum;
}left[HAL_MAX_CPU];

acoral_id rm_policy_thread_init(acoral_thread_t *thread,void (*route)(void *args),void *args,void *data){
	acoral_sr cpu_sr;
	acoral_rm_policy_data_t *policy_data;
	rm_policy_data_t *private_data;
	policy_data=(acoral_rm_policy_data_t *)data;
	private_data=(rm_policy_data_t *)acoral_malloc2(sizeof(rm_policy_data_t));
	if(private_data==NULL){
		acoral_printerr("No level2 mem space for private_data:%s\n",thread->name);
		HAL_ENTER_CRITICAL();
		acoral_release_res((acoral_res_t *)thread);
		HAL_EXIT_CRITICAL();
		return -1;
	}
	private_data->t=100*policy_data->t;
	private_data->lt=private_data->t;
	private_data->e=100*policy_data->e;
	private_data->le=private_data->e;
	private_data->period_data.args=args;
	private_data->period_data.route=route;
	private_data->period_data.time=private_data->t;
	thread->private_data=private_data;
	rm_queue_ascend_add(thread);
	return thread->res.id;
}

void rm_policy_thread_release(acoral_thread_t *thread){
	acoral_printk("Release Rm thread\n");
	acoral_free2(thread->private_data);	
}

void rm_discard_thread(acoral_thread_t *thread){
	acoral_list_del(&thread->ready);
	acoral_free2(thread->private_data);	
	acoral_release_res((acoral_res_t *)thread);
}

void rm_queue_ascend_add(acoral_thread_t *new){
      rm_policy_data_t *new_data;
      rm_policy_data_t *old_data;
      acoral_thread_t * thread;
      acoral_list_t *tmp,*head;
      head=&rm_queue->head;
      new_data=new->private_data; 
      for (tmp=head->next;tmp!=head; tmp=tmp->next){
		thread =list_entry(tmp, acoral_thread_t,ready);
		old_data=thread->private_data;
      	        if(old_data->lt>new_data->lt)
			break;		  
     }
     acoral_list_add(&new->ready,tmp->prev);
}

acoral_u8 rm_check(acoral_thread_t *thread,acoral_u32 cpu){
	rm_policy_data_t *data;
	acoral_fl tmp;
	data=thread->private_data;
	tmp=(acoral_fl)data->e/(acoral_fl)data->t;		
	if(left[cpu].sum+tmp<right[left[cpu].num]){
		left[cpu].sum+=tmp;
		left[cpu].num++;
		return 1;
	}
	return 0;
}

void rm_thread_dispatch(){
	acoral_sr cpu_sr;
	acoral_u32 i;
	acoral_thread_t * thread;
	acoral_list_t *tmp,*head;
	HAL_ENTER_CRITICAL();
	head=&rm_queue->head;
	for (tmp=head->next;tmp!=head;){
		thread =list_entry(tmp, acoral_thread_t,ready);
		tmp=tmp->next;
		if(prio>=ACORAL_HARD_RT_PRIO_MAX){
				acoral_printerr("RM thread %s Err,Prio is not enough\n",thread->name);
				/*这个线程还没初始化，包括它的stack,lock等,原来是用acoral_release_thread导致很多错误*/
				rm_discard_thread(thread);
		}else{
			thread->prio=prio;
			prio++;
			for(i=0;i<HAL_MAX_CPU;i++){
				if(rm_check(thread,i)){
					thread->cpu=i;
					break;
				}
			}
			if(i==HAL_MAX_CPU){
				acoral_printerr("Discard thread %s\n",thread->name);
				/*这个线程还没初始化，包括它的stack,lock等*/
				rm_discard_thread(thread);
			}
		} 
	}
	HAL_EXIT_CRITICAL();
	return;
}

void rm_init(){
      acoral_u32 i;
      acoral_init_list(&rm_queue->head);
      for(i=0;i<HAL_MAX_CPU;i++){
		left[i].sum=0;
		left[i].num=0;
      }
}

acoral_sched_policy_t rm_policy;
void rm_policy_init(){
	rm_init();
	rm_policy.type=ACORAL_SCHED_POLICY_RM;
	rm_policy.policy_thread_init=rm_policy_thread_init;
	rm_policy.policy_thread_release=rm_policy_thread_release;
	rm_policy.delay_deal=NULL;
	rm_policy.name="rm";
	acoral_register_sched_policy(&rm_policy);
}

void rm_end(){
      acoral_sr cpu_sr;
      acoral_thread_t * thread;
      rm_policy_data_t *private_data;
      acoral_list_t *tmp,*head;
      rm_thread_dispatch();
      HAL_ENTER_CRITICAL();
      head=&rm_queue->head;
      for (tmp=head->next;tmp!=head;){
	  thread =list_entry(tmp, acoral_thread_t,ready);
	  tmp=tmp->next;
	  acoral_list_del(&thread->ready); /*从队列上取下线程*/
	  private_data=thread->private_data;
	  acoral_policy_thread_init(ACORAL_SCHED_POLICY_PERIOD,thread,private_data->period_data.route,private_data->period_data.args,NULL);
     }
     HAL_EXIT_CRITICAL();
}
