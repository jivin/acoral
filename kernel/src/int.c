#include <type.h>
#include <hal.h>
#include <lsched.h>
#include <print.h>
#include <cpu.h>
#include <int.h>
acoral_intr_ctr_t intr_table[HAL_INTR_NUM];

/*===========================                                                                                                                 
*Initialize the interrupt      
*中断初始化函数
*===========================*/
void acoral_intr_sys_init(){
	acoral_u32 i;
	acoral_vector index;
      
    /*关中断*/
	acoral_intr_disable();

    /*中断嵌套标志初始化*/
    HAL_INTR_NESTING_INIT();

    /*中断底层初始化函数*/
	HAL_INTR_INIT();


    /*对于每个中断，设置默认的服务处理程序，然后屏蔽该中断*/
	for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		HAL_TRANSLATE_VECTOR(i,index);
		intr_table[index].isr=acoral_default_isr;
		intr_table[index].type=ACORAL_COMM_INTR;
		acoral_intr_mask(i);
	}
}



/*===========================                                                                                                                 
*Binding the isr t0 the Vector      
*将服务函数isr绑定到中断向量Vector
*===========================*/
acoral_32 acoral_intr_attach(acoral_vector vector,void (*isr)(acoral_vector)){
  	acoral_vector index;
   	HAL_TRANSLATE_VECTOR(vector,index);
	if(intr_table[index].type!=ACORAL_RT_INTR)
		intr_table[index].isr =isr;
	else
		HAL_INTR_ATTACH(vector,isr);
	return 0;
}


/*===========================                                                                                                                 
*Detach the  isr from the Vector      
*将中断的服务函数设为默认的
*===========================*/
acoral_32 acoral_intr_detach(acoral_vector vector){
  	acoral_vector index;
   	HAL_TRANSLATE_VECTOR(vector,index);
	intr_table[index].isr =acoral_default_isr;
	return 0;
}


/*===========================                                                                                                                 
*Enable the interrupt      
*使能中断
*===========================*/
void acoral_intr_unmask(acoral_vector vector){
  	acoral_vector index;
	if(vector>HAL_INTR_MAX)
		return;
   	HAL_TRANSLATE_VECTOR(vector,index);

        /*如果中断的使能函数不为空，就调用该使能函数*/
	if(intr_table[index].unmask!=NULL&&intr_table[index].type==ACORAL_COMM_INTR){
		intr_table[index].unmask(vector);
		/*将中断状态设为真*/
		intr_table[index].state=true;
	}
}


/*===========================                                                                                                                 
*Disable the interrupt       
*除能中断
*===========================*/
void acoral_intr_mask(acoral_vector vector){
  	acoral_vector index;
	if(vector>HAL_INTR_MAX)
		return;
   	HAL_TRANSLATE_VECTOR(vector,index);

        /*如果中断的除能函数不为空，就调用该除能函数 */
	if(intr_table[index].mask!=NULL&&intr_table[index].type==ACORAL_COMM_INTR){
		intr_table[index].mask(vector);
		/*将中断状态设为假    */
		intr_table[index].state=false;
	}
}


/*===========================                                                                                                                 
*the commen isr of vector      
*中断公共服务入口函数
*===========================*/
void acoral_intr_entry(acoral_vector vector){
	acoral_vector index;
#ifdef CFG_DEBUG
  	acoral_print("isr in cpu:%d\n",acoral_current_cpu);
#endif
   	HAL_TRANSLATE_VECTOR(vector,index);
	acoral_intr_nesting_inc();
	if(intr_table[index].type==ACORAL_EXPERT_INTR){
		intr_table[index].isr(vector);
		acoral_intr_disable();
	}else{
		/*这个之前都要是关中断的，调用中断进入函数*/
		if(intr_table[index].enter!=NULL)
			intr_table[index].enter(vector);
	
		acoral_intr_mask(vector);	
		/*开中断*/
		acoral_intr_enable();

		/*调用该中断的服务处理函数*/
		intr_table[index].isr(vector);
	
		/*关中断*/
		acoral_intr_disable();
	
		acoral_intr_unmask(vector);	
		/*调用中断退出函数*/
		if(intr_table[index].exit!=NULL)
			intr_table[index].exit(vector);
	}
	acoral_intr_nesting_dec();
	acoral_intr_exit();
}


/*===========================                                                                                                                 
*The default isr      
*默认中断处理程序
*===========================*/
void acoral_default_isr(acoral_vector vector){
  	acoral_printdbg("in Default interrupt route\n");
}

/*===========================                                                                                                                 
*The exit function of the vector      
*中断退出函数
*===========================*/
void acoral_intr_exit(){
      	if(!acoral_need_sched)
	    return;

	if(acoral_intr_nesting)
	    return;

	if(acoral_sched_is_lock)
	    return;

	if (!acoral_start_sched)
	    return;
      
      /*如果需要调度，则调用此函数*/
	HAL_INTR_EXIT_BRIDGE();
}



/*===========================                                                                                                                 
*Set the enter function of the vector      
*设置中断进入函数为isr
*===========================*/
void acoral_set_intr_enter(acoral_vector vector,void (*enter)(acoral_vector)){
	acoral_vector index;
	HAL_TRANSLATE_VECTOR(vector,index);
	intr_table[index].enter=enter;
}


/*===========================                                                                                                                 
*Set the exit  function of the vector 
*设置中断退出函数为isr
*===========================*/
void acoral_set_intr_exit(acoral_vector vector,void (*exit)(acoral_vector)){
	acoral_vector index;
	HAL_TRANSLATE_VECTOR(vector,index);
	intr_table[index].exit=exit;
}


/*===========================                                                                                                                 
*Set the mask  function of the vector 
*设置中断屏蔽函数为isr
*===========================*/
void acoral_set_intr_mask(acoral_vector vector,void (*mask)(acoral_vector)){
	acoral_vector index;
	HAL_TRANSLATE_VECTOR(vector,index);
	intr_table[index].mask=mask;
}


/*===========================                                                                                                                 
*Set the unmask function of the vector 
*设置中断使能函数为isr
*===========================*/
void acoral_set_intr_unmask(acoral_vector vector,void (*unmask)(acoral_vector)){
	acoral_vector index;
	HAL_TRANSLATE_VECTOR(vector,index);
	intr_table[index].unmask=unmask;
}

/*===========================                                                                                                                 
*Set interrupt type
*设置中断类型
*===========================*/
void acoral_intr_set_type(acoral_vector vector,acoral_u8 type){
  	acoral_vector index;
   	HAL_TRANSLATE_VECTOR(vector,index);
	intr_table[index].type =type;
}

/*===========================                                                                                                                 
*     exception err output
*        异常错误输出
*===========================*/

void acoral_fault_entry(acoral_u32 lr,acoral_u32 *stack){
	acoral_intr_disable();
	if(!acoral_start_sched)
		while(1);	
	acoral_printerr("Exception occur\n");
	acoral_printerr("******************\n");
	acoral_printerr("CPU:%d\n",acoral_current_cpu);;
	acoral_printerr("Thread name:%s\n",acoral_cur_thread->name);;
	acoral_printerr("Thread prio:%d\n",acoral_cur_thread->prio);;
	acoral_printerr("Thread stack_size:%d\n",acoral_cur_thread->stack_size);;
	acoral_printerr("Thread stack_buttom:0x%x\n",acoral_cur_thread->stack_buttom);;
	acoral_printerr("Thread stack:0x%x\n",acoral_cur_thread->stack);;
	acoral_printerr("Pc:0x%x\n",lr);
	acoral_printerr("Stack:0x%x\n",stack);
	acoral_printerr("******************\n");
	while(1);
}
