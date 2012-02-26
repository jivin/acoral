#include "autocfg.h"
 .equ NOINT    ,	0xc0
 .equ SVCMODE  ,	0x13

.global HAL_INTR_CTX_SWITCH
.global HAL_CONTEXT_SWITCH
.global HAL_INTR_SWITCH_TO
.global HAL_SWITCH_TO

HAL_SWITCH_TO:
    ldr     sp,[r0]     		@取得新上下文指针
    ldmfd   sp!,{r0}
    msr     cpsr,r0       	@恢复新cpsr,这个不能用spsr，因为sys,user模式没有SPSR
    ldmfd   sp!, {r0-r12,lr,pc} @恢复寄存器,

HAL_INTR_SWITCH_TO:
    stmfd   sp!,{r1-r12,lr} @保存正在服务的中断上下文
    ldr     r12,[r0]        @读取需换入的栈指针
    ldr  r2,=IRQ_stack
#ifdef CFG_CMP              @cmp支持
    bl  HAL_GET_CURRENT_CPU
    cmp r0,#0
    sub r0,r0,#1
    ldrne r2,=irq_stack
    ldrne r2,[r2,r0,lsl#2]
#endif
    sub     r2,r2,#60

    ldmfd   r12!,{r3-r11}   @读取换入线程的CPSR_sys->r3
                            @读取换入线程的r0-r7->r4-r11
    stmea   r2!,{r3-r11}    @保存换入线程的CPSR_sys->SPSR_irq, r0-r7到IRQ栈
    ldmfd   r12!,{r3-r7}    @读取换入线程的r8-r12->r3-r7
    stmea   r2!,{r3-r7}     @保存换入线程的r8-r12到IRQ栈
    ldmfd   r12!,{lr}^      @恢复换入线程的LR_sys到寄存器中
    ldmfd   r12!,{r3}       @读取换入线程的PC->r3
    add     r3,r3,#4        @模拟IRQ保存被中断上下文PC的方式：PC+4->LR_irq
    stmea   r2!,{r3}        @保存换入线程的LR_irq到IRQ栈
                            @就是将r12赋值给sp^,因为无法通过mov，所以要
    stmfd   r12!,{r12}      @读取SP_sys到r12
    ldmfd   r12!,{sp}^      @恢复SP_sys
    mov     r0,r0           @无论是否操作当前状态的SP，操作sp后，不能立即执行函数
                            @返回指令，否则返回指令的结果不可预知。
    ldmfd   sp!,{r1-r12,pc}

HAL_INTR_CTX_SWITCH:
    stmfd   sp!,{r2-r12,lr} @保存正在服务的中断上下文

    @以下几行从正在服务的中断栈顶拷贝上下文环境到旧的线程prev->stack中
    ldr  r2,=IRQ_stack
#ifdef CFG_CMP
    mov r3,r0
    bl  HAL_GET_CURRENT_CPU
    cmp r0,#0
    sub r0,r0,#1
    ldrne r2,=irq_stack
    ldrne r2,[r2,r0,lsl#2]
    mov r0,r3
#endif
    ldmea   r2!,{r3-r10}    @按递增式空栈方式弹栈，结果:
    						@[r2-1]=LR_irq->r10，被中断线程的PC+4
    						@[r2-2]=r12->r9，被中断线程的r12
    						@[r2-3]=r11->r8，被中断线程的PC
    						@.......
    						@[r2-8]=r6->r3，被中断线程的r6
    sub     r10,r10,#4      @中断栈中的LR_irq-4=PC

    @以下三句就是取出就得线程prev的SP_sys，只能通过stmfd指令间接取
    mov     r11,sp	    @下一句不能用SP，故先拷贝到r11
    stmfd   r11!,{sp}^      @被中断线程的SP_sys压入正在服务的中断栈中
    ldmfd   r11!,{r12}      @从正在服务的中断栈中读取 SP_sys->R12

    stmfd   r12!,{r10}      @保存 PC_sys
    stmfd   r12!,{lr}^      @保存 lr_sys
    stmfd   r12!,{r3-r9}    @保存被中断线程的r12-r6到它的栈中
    ldmea   r2!,{r3-r9}     @读被中断线程的r5-r0->r9-r4,SPSR_irq->r3,递增式空栈
    stmfd   r12!,{r3-r9}    @保存被中断线程的r5-r0,CPSR_sys到它的栈中

    str     r12,[r0]        @换出的上下文的栈指针-->old_sp

    @以下几行把新的线程的上下文环境(寄存器)copy到IRQ栈顶
    @与递减式满栈对应,此时IRQ栈用递增式空栈的方式访问

    ldr     r12,[r1]        @读取需换入的栈指针
    ldmfd   r12!,{r3-r11}   @读取换入线程的CPSR_sys->r3
                            @读取换入线程的r0-r7->r4-r11
    stmea   r2!,{r3-r11}    @保存换入线程的CPSR_sys->SPSR_irq, r0-r7到IRQ栈
    ldmfd   r12!,{r3-r7}    @读取换入线程的r8-r12->r3-r7
    stmea   r2!,{r3-r7}     @保存换入线程的r8-r12到IRQ栈
    ldmfd   r12!,{lr}^      @恢复换入线程的LR_sys到寄存器中
    ldmfd   r12!,{r3}       @读取换入线程的PC->r3
    add     r3,r3,#4        @模拟IRQ保存被中断上下文PC的方式：PC+4->LR_irq
    stmea   r2!,{r3}        @保存换入线程的LR_irq到IRQ栈
                            @就是将r12赋值给sp^,因为无法通过mov，所以要
    stmfd   r12!,{r12}      @读取SP_sys到r12
    ldmfd   r12!,{sp}^      @恢复SP_sys
    mov     r0,r0           @无论是否操作当前状态的SP，操作sp后，不能立即执行函数
                            @返回指令，否则返回指令的结果不可预知。
    ldmfd   sp!,{r2-r12,pc}


HAL_CONTEXT_SWITCH:
    stmfd   sp!,{lr}            @保存PC
    stmfd   sp!,{r0-r12,lr}     @保存寄存器和LR
    mrs     r4,CPSR
    stmfd   sp!,{r4}            @保存cpsr
    str     sp,[r0]             @保存旧上下文栈指针到旧的线程prev的堆栈变量

    ldr     sp,[r1]             @取得新上下文指针
    ldmfd   sp!,    {r0}
    msr     cpsr,r0       	@恢复新cpsr,这个不能用spsr，因为sys,user模式没有SPSR
    ldmfd   sp!, {r0-r12,lr,pc} @恢复寄存器,

