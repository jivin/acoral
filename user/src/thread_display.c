#include<thread.h>
#include<policy.h>
#include<hal.h>
#include"../include/shell.h"
void display_thread(acoral_32 argc,acoral_char **argv){
	acoral_sr cpu_sr;	
	acoral_list_t *head,*tmp;
	acoral_thread_t * thread;
	head=&acoral_threads_queue.head;
	acoral_prints("\t\tSystem Thread Information\r\n");
	acoral_prints("------------------------------------------------------\r\n");
	acoral_prints("Name\t\tType\t\tConsole\t\tState\r\n");
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&acoral_threads_queue.head.lock);
	for(tmp=head->next;tmp!=head;tmp=tmp->next){
		thread=list_entry(tmp,acoral_thread_t,global_list);
		acoral_print("%s\t\t",thread->name);
		switch(thread->policy){
			case ACORAL_SCHED_POLICY_COMM:
				acoral_prints("Common\t\t");
				break;
			case ACORAL_SCHED_POLICY_SLICE:
				acoral_prints("Slice\t\t");
				break;
			case ACORAL_SCHED_POLICY_PERIOD:
				acoral_prints("Period\t\t");
				break;
			case ACORAL_SCHED_POLICY_RM:
				acoral_prints("RM\t\t");
				break;
			case ACORAL_SCHED_POLICY_POSIX:
				acoral_prints("Posix\t\t");
			default:
				break;
		}
		acoral_print("%d\t\t",thread->console_id);
		if(thread->state&ACORAL_THREAD_STATE_RUNNING)
				acoral_prints("Running\t\t");
		else if(thread->state&ACORAL_THREAD_STATE_READY)
				acoral_prints("Ready\t\t");
		else if(thread->state&ACORAL_THREAD_STATE_DELAY)
				acoral_prints("Delay\t\t");
		else if(thread->state&ACORAL_THREAD_STATE_SUSPEND)
				acoral_prints("Sleep\t\t");
		else if(thread->state==ACORAL_THREAD_STATE_EXIT)
				acoral_prints("Freeze\t\t");
		else
				acoral_prints("Error\t\t");
		acoral_prints("\r\n");
	}
	acoral_prints("------------------------------------------------------\r\n");
	acoral_spin_unlock(&acoral_threads_queue.head.lock);
	HAL_EXIT_CRITICAL();
}

acoral_shell_cmd_t dt_cmd={
	"dt",
	display_thread,
	"View all thread info",
	NULL
};
