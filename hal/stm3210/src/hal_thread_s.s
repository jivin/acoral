.cpu cortex-m3
.fpu softvfp
.syntax unified
.thumb

.text
.equ	NVIC_INT_CTRL,   0xE000ED04   /* interrupt control state register */
.equ	NVIC_SYSPRI2,    0xE000ED20   /* system priority register (2) */
.equ	NVIC_PENDSV_PRI, 0x00FF0000   /* PendSV priority value (lowest) */
.equ	NVIC_PENDSVSET,  0x10000000   /* value to trigger PendSV exception*/

.global HAL_INTR_CTX_SWITCH
.global HAL_START_OS
.global HAL_CONTEXT_SWITCH
.global HAL_SWITCH_TO
.global HAL_INTR_SWITCH_TO
.global HAL_SCHED_BRIDGE
.global HAL_INTR_EXIT_BRIDGE
.global PENDSV_CALL

.type HAL_CONTEXT_SWITCH ,function
.type HAL_INTR_CTX_SWITCH,function
.type HAL_SWITCH_TO,function
.type HAL_INTR_SWITCH_TO,function
.type HAL_START_OS,function
.type HAL_INTR_EXIT_BRIDGE,function
.type HAL_SCHED_BRIDGE,function
.type PENDSV_CALL,function

HAL_INTR_EXIT_BRIDGE:
	ldr     r0,  =NVIC_INT_CTRL          @触发pendsv中断切换进程
	ldr     r1,  =NVIC_PENDSVSET
	str     r1,  [r0]
	bx lr

HAL_SCHED_BRIDGE:
	ldr     r0,  =NVIC_INT_CTRL          @触发pendsv中断切换进程
	ldr     r1,  =NVIC_PENDSVSET
	CPSIE   I                            @必须开中断
	str     r1,  [r0]
	bx lr

HAL_START_OS:
	LDR     R1,=Msp_stack
	MSR     MSP,R1			    @重置主栈指针，主栈将用于异常和中断
	MOV     R1,#2
	MSR     CONTROL,R1          @切换到用户线程模式，栈指针用PSP
	ldr     r0,[r0]
	msr     psp,r0
	ldmfd   sp!, {r0,r4-r11}
	msr  primask,r0
	ldmfd   sp!, {r0-r3}
	ldmfd   sp!,{r4-r7}
	mov  lr,r5  
	mov  pc,r6  

HAL_INTR_SWITCH_TO:
HAL_SWITCH_TO:
	ldr     r1,[r0]     @取待切入线程的psp
	msr     psp,r1   
	bx      lr

HAL_INTR_CTX_SWITCH:
HAL_CONTEXT_SWITCH:
	mrs 	r2,psp
	str 	r2,[r0]	
	ldr     r2,[r1]     @取待切入线程的psp
	msr     psp,r2      @待切入线程的当前栈指针写入psp，中断返回恢复上下文将以此为准
	bx      lr

@===========
@  r0 --> switch form thread stack
@  r1 --> switch to   thread stack
@  psr, pc, lr, r12, r3, r2, r1, r0 are pushed into[from] stack
@===========
PENDSV_CALL:
    mrs r1,primask
    CPSID   I
    push    {r0,lr}			
    mrs     r0,psp
    stmfd   r0!,{r1,r4-r11}       
    msr     psp,r0
    bl acoral_real_intr_sched
    pop     {r0,lr}
    mrs     r0,psp
    ldmfd   r0!,{r1,r4-r11}
    msr     psp,r0
    msr  primask,r1
    bx      lr
