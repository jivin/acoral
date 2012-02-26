.global HAL_GET_CURRENT_CPU
.global HAL_FOLLOW_CPU_START
.global HAL_MOVE_SWITCH_TO
.equ MODE_MASK ,0x1f
.equ MODE_SYSTEM ,0x1f
.equ NOINT ,0xc0

HAL_MOVE_SWITCH_TO:
	str r1,[r0] 			@开锁，这个过后目标核就可能在使用当前线程的堆栈了
					@在SP的值前所以下面指令不能有任何使用堆栈的操作
	mov r0,r2
        bl  HAL_SWITCH_TO               @这个函数会修改sp的值为next，所以可以方便调度。
	b  .       		     	@这个不会回来

HAL_FOLLOW_CPU_START:
	mrs     r0,cpsr
       	bic     r0,r0,#MODE_MASK
	orr     r1,r0,#MODE_SYSTEM|NOINT
	msr     cpsr_cxsf,r1    	@ userMode
	ldr     r0,=tmp_stack
	ldr r0,[r0]
	mov    sp,r0
	b acoral_start 	

HAL_GET_CURRENT_CPU:
	MRC     p15, 0, r0, c0, c0, 5
        ANDS    r0, r0, #0x0f
	MOV PC,lr

