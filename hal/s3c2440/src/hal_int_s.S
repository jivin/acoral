#include"../include/hal_2440_cfg.h"

   .global     HAL_INTR_ENTRY
   .global     EXP_HANDLER
   .global     HAL_INTR_DISABLE
   .global     HAL_INTR_ENABLE
   .global     HAL_INTR_DISABLE_SAVE
   .global     HAL_INTR_RESTORE
   .extern     IRQ_stack

HAL_INTR_ENTRY:
    stmfd   sp!,    {r0-r12,lr}           @保护通用寄存器及PC 
    mrs     r1,     spsr
    stmfd   sp!,    {r1}                  @保护spsr，以支持中断嵌套

    msr     cpsr_c, #SVC_MODE|NOIRQ        @进入SVC_MODE,以便允许中断嵌套
    stmfd   sp!,    {lr}                  @保存SVc模式的专用寄存器lr

    ldr     r0,     =INTOFFSET 		  @读取中断向量号
    ldr     r0,     [r0]
    mov     lr,    pc                     @求得lr的值
    ldr     pc,    =hal_all_entry 

    ldmfd   sp!,    {lr}                    @恢复svc模式下的lr,
    msr     cpsr_c,#IRQ_MODE|NOINT       @更新cpsr,进入IRQ模式并禁止中断
    ldmfd   sp!,{r0}                    @spsr->r0
    msr     spsr_cxsf,r0                @恢复spsr
    ldmfd   sp!,{r0-r12,lr}
    subs    pc,lr,#4                    @此后，中断被重新打开

EXP_HANDLER:
	stmfd   sp!,{lr}                @保护寄存器,以及返回地址
	mov     r0,sp  
	stmfd   r0!,{sp}^               @出错线程的SP_sys压入exp中断栈中
	ldmfd   r0!,{r1}                @从exp中断栈中读取 SP_sys->R1
	mov     r0,lr
	bl acoral_fault_entry
	ldmfd   sp!,{lr}                  @从exp中断栈中读取 SP_sys->R1
	subs pc,lr,#0

HAL_INTR_ENABLE:
    mrs r0,cpsr
    bic r0,r0,#NOINT
    msr cpsr_cxsf,r0
    mov pc,lr

HAL_INTR_DISABLE:
    mrs r0,cpsr
    mov r1,r0
    orr r1,r1,#NOINT
    msr cpsr_cxsf,r1
    mov pc ,lr

HAL_INTR_RESTORE:
	MSR     CPSR_c, R0
	MOV     PC, LR

HAL_INTR_DISABLE_SAVE:
	MRS     R0, CPSR				@ Set IRQ and FIable all interrupts
	ORR     R1, R0, #0xC0
	MSR     CPSR_c, R1
	MRS     R1, CPSR				@ Confirm that Cpt disable flags
	AND     R1, R1, #0xC0
	CMP     R1, #0xC0
	BNE     HAL_INTR_DISABLE_SAVE			@ Not properly dsabled (try again)
	MOV     PC, LR					@ Disabled, return thcontents in R0
