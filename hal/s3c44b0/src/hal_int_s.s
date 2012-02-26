.equ    NOINT,              0xc0
.equ    NOIRQ,              0x80
.equ    FIQMODE,            0x11
.equ    IRQMODE,            0x12
.equ    MODEMASK,           0x1f
.equ    SYSMODE,            0x1f
.equ    SVCMODE,            0x13
.equ    I_ISPR,          0x01E00020   @哪个中断请求在INTPND寄存器之中

   .global     HAL_INTR_ENTRY
   .global     HAL_INTR_DISABLE
   .global     HAL_INTR_ENABLE
   .global     HAL_INTR_DISABLE_SAVE
   .global     HAL_INTR_RESTORE
   .extern IRQ_stack
   .extern     acoral_intr_entry

HAL_INTR_ENTRY:
    stmfd   sp!,{r0-r12,lr}         @保护通用寄存器,以及返回地址
    ldr r0,=I_ISPR
    ldr r0,[r0]
    mov   r8,#0x00
1:
    movs   r0,r0,lsr   #1
    @   I_ISPR=I_ISPR < <1;
    Bcs   loop1   @ 跳转到1
    Add   r8,r8,#1
    B     1b
loop1:
    mov     r0,r8

    mrs     r1,spsr
    stmfd   sp!,{r1}            @保护SPSR，以支持中断嵌套
    msr     cpsr_c,#SVCMODE|NOIRQ @进入SVCMODE,以便允许中断嵌套
    stmfd   sp!,{lr}            @保存svc模式的lr,

    mov     lr,pc           @这两条指令模拟函数调用，调用用户中断处理函数,
    ldr     pc,=acoral_intr_entry


    ldmfd   sp!,{lr}        @恢复svc的lr,
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
