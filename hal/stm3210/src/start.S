.cpu cortex-m3
.fpu softvfp 
.syntax unified
.thumb

.text
.global RESET_HANDLER
.global __ENTRY 
.extern acoral_start
.type RESET_HANDLER,function
.type FAULT_HANDLER,function
	.word Msp_stack 
	.word RESET_HANDLER
	.word FAULT_HANDLER 
        .word FAULT_HANDLER  
__ENTRY:
RESET_HANDLER:
    ldr     r0,=Psp_stack
    msr     psp,r0			@初始化线程栈指针，主栈指针是自动初始化的
    CPSID   I               @PRIMASK=1，关中断
    @CPSID   F               @FAULTMASK=1,关异常
    mov		r0,#0
    msr		control,r0		@初始化代码在特权级，栈指针选择msp

    movs  r1, #0
    bl  LoopCopyDataInit

CopyDataInit:
    ldr  r3, =text_end
    ldr  r3, [r3, r1]
    str  r3, [r0, r1]
    adds  r1, r1, #4

LoopCopyDataInit:
    ldr  r0, =data_start
    ldr  r3, =data_end
    adds  r2, r0, r1
    cmp  r2, r3
    bcc  CopyDataInit
    ldr  r2, =bss_start
    ldr  r3, =bss_end
    b  LoopFillZerobss

FillZerobss:
    movs  r1, #0
    str  r1, [r2], #4

LoopFillZerobss:
    cmp  r2, r3
    bcc  FillZerobss

    mov r0,r0
    bl  acoral_start
    b .

FAULT_HANDLER:
    bx lr
