.equ   NOINT    ,              0xc0
.equ   NOIRQ    ,              0x80
.equ   FIQMODE    ,            0x11
.equ   IRQMODE    ,            0x12
.equ   MODEMASK    ,           0x1f
.equ   SYSMODE    ,            0x1f
.equ   SVCMODE    ,            0x13
.equ   I_ISPR    ,          0x01E00020   @哪个中断请求在INTPND寄存器之中

   .global     HAL_INTR_ENTRY
   .global     HAL_INTR_DISABLE
   .global     HAL_INTR_ENABLE
   .global     HAL_INTR_DISABLE_SAVE
   .global     HAL_INTR_RESTORE
   .global 	  EXP_HANDLER
   .extern     acoral_intr_entry
   .extern acoral_fault_entry	
	
  .macro HANDLE handler,num

        .global  \handler                    @ The label for .globals 输出的标号

\handler:
	stmfd   sp!,{r0-r12,lr} 
	mov r0,#\num
	b HAL_INTR_ENTRY
 .endm
    

        .global  IRQ_ISRDEF                      @ The label for .globals 输出的标号

IRQ_ISRDEF:
	stmfd   sp!,{r0-r12,lr} 
	ldr r0,=0xFFFFF000
	ldr r1,[r0]
	and r1,r1,#0x10000
	movne r0,#16
	and r1,r1,#0x20000
	movne r0,#17
	and r1,r1,#0x40000
	movne r0,#18
	b HAL_INTR_ENTRY
    
HANDLE  IRQ_ISR0 , 0
HANDLE  IRQ_ISR1 , 1
HANDLE  IRQ_ISR2 , 2
HANDLE  IRQ_ISR3 , 3
HANDLE  IRQ_ISR4 , 4
HANDLE  IRQ_ISR5 , 5
HANDLE  IRQ_ISR6 , 6
HANDLE  IRQ_ISR7 , 7
HANDLE  IRQ_ISR8 , 8
HANDLE  IRQ_ISR9 , 9
HANDLE  IRQ_ISR10 , 10
HANDLE  IRQ_ISR11 , 11
HANDLE  IRQ_ISR12 , 12
HANDLE  IRQ_ISR13 , 13
HANDLE  IRQ_ISR14 , 14
HANDLE  IRQ_ISR15 , 15 
	
HAL_INTR_ENTRY:
           @保护寄存器,以及返回地址
   	mrs     r1,spsr
    stmfd   sp!,{r1}            @保护SPSR，以支持中断嵌套
    msr     cpsr_c,#SVCMODE|NOIRQ @进入SVCMODE,以便允许中断嵌套
    stmfd   sp!,{lr}            @保存lr_svc,

    mov     lr,pc           @这两条指令模拟函数调用，调用用户中断处理函数,
    ldr     pc,=acoral_intr_entry


    ldmfd   sp!,{lr}        @恢复lr_svc,
   
    msr     cpsr_c,#IRQMODE|NOINT    @更新cpsr,进入IRQ模式并禁止中断
    ldmfd   sp!,{r0}        @spsr->r0
    msr     spsr_cxsf,r0    @恢复spsr
    ldmfd   sp!,{r0-r12,lr}
    subs    pc,lr,#4        @此后，中断被重新打开

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

EXP_HANDLER:
	stmfd   sp!,{lr}      @保护寄存器,以及返回地址
	mov     r0,sp
	stmfd   r0!,{sp}^     @出错线程的SP_sys压入exp中断栈中
	ldmfd   r0!,{r1}      @从exp中断栈中读取 SP_sys->R1
	mov     r0,lr
	bl acoral_fault_entry
	ldmfd   sp!,{lr}      @从exp中断栈中读取 SP_sys->R1
	subs pc,lr,#0
