#include "hal_int.h"
.equ    NOINT,              0xc0
.equ    NOIRQ,              0x80
.equ    FIQMODE,            0x11
.equ    IRQMODE,            0x12
.equ    MODEMASK,           0x1f
.equ    SYSMODE,            0x1f
.equ    SVCMODE,            0x13
.equ    INTOFFSET,          0x4a000014
.equ    REALVIEW_TC11MP_GIC_CPU_BASE,	0x1F000100
.equ    REALVIEW_PB11MP_GIC_CPU_BASE,   0x1E000000
.equ    GIC_CPU_PRIMASK ,               0x4
.equ    GIC_CPU_INTACK	,		0x0c
   .global     HAL_INTR_ENTRY
   .global     HAL_VECTR_START
   .global     HAL_VECTR_END
   .global     HAL_INTR_DISABLE
   .global     HAL_INTR_ENABLE
   .global     HAL_INTR_DISABLE_SAVE
   .global     HAL_INTR_RESTORE
   .global     HAL_CLOSE_COMM_INTR
   .extern     IRQstack
   .extern     acoral_intr_entry

HAL_VECTR_START:
	      LDR     pc, VECTOR_TABLE+0    @ Reset
        LDR     pc, VECTOR_TABLE+4    @ Undefined
        LDR     pc, VECTOR_TABLE+8    @ SWI
        LDR     pc, VECTOR_TABLE+0xc     @ Prefetch Abort
        LDR     pc, VECTOR_TABLE+0x10    @ Data Abort
        LDR     pc, VECTOR_TABLE+0x14    @ RESERVED
       	LDR     pc, VECTOR_TABLE+0x18   @ IRQ
        LDR     pc, VECTOR_TABLE+0x1c    @ FIQ

VECTOR_TABLE:
	.long EXP_HANDLER
	.long EXP_HANDLER
	.long EXP_HANDLER
	.long EXP_HANDLER
	.long EXP_HANDLER
	.long EXP_HANDLER
	.long HAL_INTR_ENTRY
	.long EXP_HANDLER
HAL_VECTR_END:

HAL_INTR_ENTRY:
    stmfd   sp!,{r0-r12,lr}                @保护通用寄存器,以及返回地址
    mrs     r1,spsr
    stmfd   sp!,{r1}                       @保护SPSR_irq，以支持中断嵌套
    msr     cpsr_c,#SVCMODE|NOIRQ          @进入SVCMODE,以便允许中断嵌套
    stmfd   sp!,{lr}                 	   @保存SVC模式的lr,
  
    ldr     r0, =REALVIEW_TC11MP_GIC_CPU_BASE
    ldr     r0, [r0,#GIC_CPU_INTACK]       @bit 12-10=src cpu    9-0=irq
    bic     r0, r0,#0x1c00                 @清高位，得到中断号
    mov     lr,pc                          @这两条指令模拟函数BL调用,进入中断总入口
    ldr     pc,=hal_all_entry

    ldmfd   sp!,{lr}                       @恢复lr_svc,
    msr     cpsr_c,#IRQMODE|NOINT          @更新cpsr,进入IRQ模式并禁止中断
    ldmfd   sp!,{r0}                       @spsr->r0
    msr     spsr_cxsf,r0                   @恢复spsr
    ldmfd   sp!,{r0-r12,lr}
    subs    pc,lr,#4                       @此后，中断被重新打开

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

HAL_CLOSE_COMM_INTR:
        ldr r1,=PRIMASK
        ldr r1,[r1]

        ldr r0,=REALVIEW_TC11MP_GIC_CPU_BASE
        str r1,[r0,#GIC_CPU_PRIMASK]

        ldr r0,=REALVIEW_PB11MP_GIC_CPU_BASE
        str r1,[r0,#GIC_CPU_PRIMASK]

EXP_HANDLER:
	stmfd   sp!,{lr}                @保护寄存器,以及返回地址
	mov     r0,sp
	stmfd   r0!,{sp}^      @出错线程的SP_sys压入exp中断栈中
	ldmfd   r0!,{r1}      @从exp中断栈中读取 SP_sys->R1
	mov     r0,lr
	bl acoral_fault_entry
	ldmfd   sp!,{lr}      @从exp中断栈中读取 SP_sys->R1
	subs pc,lr,#0
