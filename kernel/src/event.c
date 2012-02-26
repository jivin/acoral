#include "acoral.h"
#include "event.h"
acoral_pool_ctrl_t acoral_evt_pool_ctrl;
void acoral_evt_sys_init(){
	acoral_evt_pool_init();
}

void acoral_evt_pool_init(){
	acoral_evt_pool_ctrl.type=ACORAL_RES_EVENT;
	acoral_evt_pool_ctrl.size=sizeof(acoral_evt_t);
	acoral_evt_pool_ctrl.num_per_pool=8;
	acoral_evt_pool_ctrl.num=0;
	acoral_evt_pool_ctrl.max_pools=4;
	acoral_pool_ctrl_init(&acoral_evt_pool_ctrl);
#ifdef CFG_MSG
	acoral_msg_sys_init();
#endif
}

acoral_evt_t *acoral_alloc_evt(){
  	return (acoral_evt_t *)acoral_get_res(&acoral_evt_pool_ctrl);
}

void acoral_evt_init(acoral_evt_t *evt){
	acoral_spin_init(&evt->spin_lock);
	acoral_list_init(&evt->wait_queue.head);
}

bool acoral_evt_queue_empty(acoral_evt_t *evt){
	return acoral_list_empty(&evt->wait_queue.head);
}

acoral_thread_t *acoral_evt_high_thread(acoral_evt_t *evt){
	acoral_list_t *head;
	acoral_thread_t *thread;
	head=&evt->wait_queue.head;
	if(acoral_list_empty(head))
		return NULL;
	thread=list_entry(head->next,acoral_thread_t,waiting);
	return thread;	
}

void acoral_evt_queue_add(acoral_evt_t *evt,acoral_thread_t *new){
	acoral_list_t *head,*tmp;
	acoral_thread_t *thread;
  	new->evt=evt;
	head=&evt->wait_queue.head;
	for(tmp=head->next;tmp!=head;tmp=tmp->next){
		thread=list_entry(tmp,acoral_thread_t,waiting);
		/*如果线程资源已经不在使用，即release状态则释放*/
		if(thread->prio>new->prio)
			break;
	}
	acoral_list_add(&new->waiting,tmp->prev);
}

void acoral_evt_queue_del(acoral_thread_t *thread){
	acoral_list_del(&thread->waiting);
	thread->evt=NULL;
}

