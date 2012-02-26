/*===========================
 *    file: ipi.c
 *    func: 核间通信的相关实现
 *    by jivin
 *===========================*/

#include<type.h>
#include<hal.h>
#include<ipi.h>
#include<int.h>
#include<cpu.h>
#include<thread.h>
#include<hal.h>
acoral_ipi_cmd_t ipi_cmd[HAL_MAX_CPU];

/*===========================
 *    inter-processor interrupt initialize 
 *    核间中断初始化
 *===========================*/
void acoral_ipi_init(){
	acoral_intr_attach(HAL_CMD_IPI,acoral_ipi_cmd_entry);
	acoral_intr_unmask(HAL_CMD_IPI);
  	acoral_ipi_cmd_init();
}

/*===========================
 *    inter-core command initialize 
 *    核间命令初始化
 *===========================*/
void acoral_ipi_cmd_init(){
	acoral_u32 i;	
	for(i=0;i<HAL_MAX_CPU;i++)
		acoral_spin_init(&ipi_cmd[i].lock);
}

/*===========================
 *    send command to the given cpu core 
 *    给特定的cpu发送命令
 *===========================*/
void acoral_ipi_cmd_send(acoral_u32 cpu,acoral_u32 cmd,acoral_id thread_id,void *data){
	acoral_ipi_cmd_t *cmd_data;
	/*设置命令数据*/
	if(cpu>=HAL_MAX_CPU)
		cpu=HAL_MAX_CPU-1;	
	cmd_data=ipi_cmd+cpu;
	/*占用cmd数据结构锁*/
	acoral_spin_lock(&cmd_data->lock);
	
	cmd_data->cmd=cmd;
	cmd_data->thread_id=thread_id;
	cmd_data->data=data;
	/*发送核间中断*/
	HAL_IPI_SEND(cpu,HAL_CMD_IPI);
}


/*===========================
 *    inter-core command interrupt handle func
 *    核间命令中断处理函数
 *===========================*/
void acoral_ipi_cmd_entry(acoral_vector vector){
	acoral_ipi_cmd_t *cmd_data;
	acoral_u32 cmd;
	acoral_id thread_id;
	void *data;
	cmd_data=ipi_cmd+acoral_current_cpu;
	cmd=cmd_data->cmd;
	thread_id=cmd_data->thread_id;
	data=cmd_data->data;
	acoral_spin_unlock(&cmd_data->lock);
	/*释放cmd数据结构锁*/
	switch(cmd){
		case ACORAL_IPI_THREAD_KILL:
			acoral_kill_thread_by_id(thread_id);
			break;
		case ACORAL_IPI_THREAD_RESUME:
			acoral_resume_thread_by_id(thread_id);
			break;
		case ACORAL_IPI_THREAD_SUSPEND:
			acoral_suspend_thread_by_id(thread_id);
			break;
		case ACORAL_IPI_THREAD_CHG_PRIO:
			acoral_thread_change_prio_by_id(thread_id,data);
			break;
		case ACORAL_IPI_THREAD_MOVE2_TAIL:
			acoral_thread_move2_tail_by_id(thread_id);
			break;
		case ACORAL_IPI_THREAD_MOVETO:
			acoral_moveto_thread_by_id(thread_id,data);
			break;
		case ACORAL_IPI_THREAD_MOVEIN:
			acoral_movein_thread_by_id(thread_id,data);
			break;
		default:
			break;
	}
}

/*===========================
 *    get given command's data in the cpu core
 *    获取核间命令对应的核上的数据结构
 *===========================*/
acoral_ipi_cmd_t *acoral_get_ipi_cmd(acoral_u32 cmd,acoral_u32 cpu){
  	return ipi_cmd+cpu;
}



