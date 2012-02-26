#include <type.h>
#include <hal.h>
#include <cpu.h>
#include <thread.h>
#include <print.h>
#include <ipi.h>
#include <lsched.h>
#include <policy.h>
#include <comm_thrd.h>
#include <cmp.h>
extern acoral_u32 idle_count[HAL_MAX_CPU];
/*多核初始化*/
void acoral_cmp_init(){
	/*为其他核心启动作准备，比如其他核心的启动代码*/
	acoral_prepare_cpus();
	/*启动其他核心，Pb11Mpcore为发送中断*/
	acoral_start_cpus();
	/*核间通信初始化*/
	acoral_ipi_init();
}

void acoral_prepare_cpus(){
	HAL_PREPARE_CPUS();
}

void acoral_start_cpus(){
  	int i;
	for(i=0;i<HAL_MAX_CPU;i++)
	    if(!HAL_CPU_IS_ACTIVE(i))
	      	HAL_START_CPU(i); 	
}

void idle_follow(void *args){
	HAL_CMP_ACK();
	while(1){
#ifdef CFG_DEBUG
	  	acoral_printdbg("In cpu:%d idle\n",acoral_current_cpu);
#endif
#ifdef CFG_STAT
		idle_count[acoral_current_cpu]++;
#endif
	}
}

void acoral_follow_cpu_start(){
	acoral_comm_policy_data_t data;
	acoral_id idle_id;
  	acoral_printdbg("In follow\n");
	HAL_FOLLOW_CPU_INIT();
	data.cpu=acoral_current_cpu;
	data.prio=ACORAL_IDLE_PRIO;
	data.prio_type=ACORAL_ABSOLUTE_PRIO;
	idle_id=acoral_create_thread_ext(idle_follow,128,NULL,"idle",NULL,ACORAL_SCHED_POLICY_COMM,&data);
	if(idle_id==-1)
		while(1);
	acoral_start_os();
}

void acoral_moveto_thread(acoral_thread_t *thread,acoral_u8 cpu){
	acoral_sr cpu_sr;
	acoral_thread_t *cur;
	if(cpu>=HAL_MAX_CPU)
		return;
	if(thread->cpu==cpu)
		return;   /*不需要迁移*/
	cur=acoral_cur_thread;
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&thread->move_lock);
	if(thread->cpu!=acoral_current_cpu){
	  	acoral_ipi_cmd_send(thread->cpu,ACORAL_IPI_THREAD_MOVETO,thread->res.id,(void *)cpu);
		acoral_spin_unlock(&thread->move_lock);
		HAL_EXIT_CRITICAL();
		return;
	}
	acoral_unrdy_thread(thread);
	acoral_ipi_cmd_send(cpu,ACORAL_IPI_THREAD_MOVEIN,thread->res.id,NULL);
	if((acoral_32)thread!=(acoral_32)cur){
		acoral_spin_unlock(&thread->move_lock);
		HAL_EXIT_CRITICAL();
		return;
	}else{
		thread->state|=ACORAL_THREAD_STATE_MOVE;
		/*执行这个后不会回来,这个要在关中断下进行，因为涉及多核，要越快越好，要不等待太久*/
		acoral_sched();
	}
	/*这个不会回来的，为了检测。*/
	while(1);
}

void acoral_moveto_thread_by_id(acoral_id thread_id,acoral_u8 cpu){
	acoral_thread_t *thread=(acoral_thread_t *)acoral_get_res_by_id(thread_id);
	acoral_moveto_thread(thread,cpu);
}

void acoral_jump_cpu(acoral_u8 cpu){
	acoral_moveto_thread(acoral_cur_thread,cpu);
}

void acoral_movein_thread(acoral_thread_t *thread){
	acoral_spin_lock(&thread->move_lock);
        thread->cpu=acoral_current_cpu;	
	acoral_rdy_thread(thread);
	acoral_spin_unlock(&thread->move_lock);
}

void acoral_movein_thread_by_id(acoral_id thread_id){
	acoral_thread_t *thread=(acoral_thread_t *)acoral_get_res_by_id(thread_id);
	acoral_movein_thread(thread);
}
