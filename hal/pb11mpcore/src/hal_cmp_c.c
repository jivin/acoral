#include "acoral.h"
acoral_u32 active_map[1]={0};
acoral_u32 stack[150];
acoral_u32 *tmp_stack=&stack[149];
acoral_u32 *irq_stack[HAL_MAX_CPU-1];
acoral_spinlock_t cmp_lock;
struct stack{
	acoral_u32 irq[100];
	acoral_u32 abt[10];
	acoral_u32 und[10];
	acoral_u32 svc[100];
	acoral_u32 end[1];
}stacks[HAL_MAX_CPU-1];

acoral_u32 hal_cpu_is_active(acoral_u32 cpu){
	return acoral_get_bit(cpu,active_map);
}

void hal_core_cpu_init(){
	acoral_set_bit(acoral_current_cpu,active_map);
}

void hal_follow_cpu_init(){
	/*初始化stack*/
	fellow_stack_init();
       /*初始化intr_cpu_interface*/
	gic_cpu_init(0,REALVIEW_TC11MP_GIC_CPU_BASE);
	gic_cpu_init(1,REALVIEW_PB11MP_GIC_CPU_BASE);
}

void hal_cmp_ack(){
    acoral_spin_unlock(&cmp_lock);
}

void hal_wait_ack(){
    acoral_spin_lock(&cmp_lock);
}

void HAL_FOLLOW_CPU_START();
void hal_prepare_cpus(){
	acoral_u32 i;
	InvalidateIDCache();
	HAL_REG(BRD_BASE,SYS_FLAGSSET)=HAL_FOLLOW_CPU_START;
	HAL_MB();	
	for(i=1;i<HAL_MAX_CPU;i++)
		irq_stack[i-1]=&stacks[i-1].abt[0];	
	acoral_spin_init_lock(&cmp_lock);
}

void hal_start_cpu(acoral_u32 cpu){
	HAL_IPI_SEND(cpu,1);
	HAL_WAIT_ACK();
}

void fellow_stack_init(void)
{
	acoral_u32 cpu = acoral_current_cpu;
	struct stack *stk = &stacks[cpu-1];
#define PLC	"I"
	/*
	 * setup stacks for re-entrant exception handlers
	 */
	__asm__ (
	"msr	cpsr_c, %1\n\t"
	"add	r14, %0, %2\n\t"
	"mov	sp, r14\n\t"
	"msr	cpsr_c, %3\n\t"
	"add	r14, %0, %4\n\t"
	"mov	sp, r14\n\t"
	"msr	cpsr_c, %5\n\t"
	"add	r14, %0, %6\n\t"
	"mov	sp, r14\n\t"
	"msr	cpsr_c, %7\n\t"
	"add	r14, %0, %8\n\t"
	"mov	sp, r14\n\t"
	"msr	cpsr_c, %9"
	    :
	    : "r" (stk),
	      PLC (PSR_F_BIT | PSR_I_BIT | MODE_IRQ),
	      "I" (offsetof(struct stack, abt[0])),
	      PLC (PSR_F_BIT | PSR_I_BIT | MODE_ABORT),
	      "I" (offsetof(struct stack, und[0])),
	      PLC (PSR_F_BIT | PSR_I_BIT | MODE_UNDEF),
	      "I" (offsetof(struct stack, svc[0])),
	      PLC (PSR_F_BIT | PSR_I_BIT | MODE_SVC),
	      "I" (offsetof(struct stack, end[0])),
	      PLC (PSR_F_BIT | PSR_I_BIT | MODE_SYSTEM)
	    : "r14");
}


