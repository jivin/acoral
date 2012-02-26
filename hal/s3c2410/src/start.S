#include "hal_cfg.h"
#include "autocfg.h"
	.extern text_start
	.extern bss_start
	.extern bss_end
	.extern HAL_INTR_ENTRY
	.extern acoral_start

	.global  __ENTRY
	.global HandleIRQ
__ENTRY:
	b	ResetHandler
	b	HandleUndef	@handler for Undefined mode
	b	HandleSWI       @handler for SWI interrupt
	b	HandlePabort	@handler for PAbort
	b	HandleDabort	@handler for DAbort
	b       .			        @reserved
	b       HandleIRQ 	@handler for IRQ interrupt
	b	HandleFIQ	@handler for FIQ interrupt
@ 0x20: magic number so we can verify that we only put
	.long   0
@ 0x24:
	.long   0
@ 0x28: where this was linked, so we can put it in memory in the right place
	.long   __ENTRY
@ 0x2C: this contains the platform, cpu and machine id
	.long   2410
@ 0x30:  capabilities
	.long   0
@ 0x34:
	b   	.

@****************************************************************
@ intvector setup
@****************************************************************

HandleFIQ:
	ldr pc,=acoral_start
HandleIRQ:
	ldr pc,=HAL_INTR_ENTRY
HandleUndef:
	ldr pc,=EXP_HANDLER
HandleSWI:
	ldr pc,=EXP_HANDLER
HandleDabort:
	ldr pc,=EXP_HANDLER
HandlePabort:
	ldr pc,=EXP_HANDLER



.align 4
_text_start:
        .long   text_start
_bss_start:
	.long   bss_start
_bss_end:
        .long   bss_end

@****************************************************************
@             ResetHandler fuction
@****************************************************************

ResetHandler:

       	@ disable watch dog timer
	mov	r1, #0x53000000
	mov	r2, #0x0
	str	r2, [r1]

	@ disable all interrupts
	mov	r1, #INT_CTL_BASE
	mov	r2, #0xffffffff
	str	r2, [r1, #oINTMSK]
	ldr	r2, =0x7ff
	str	r2, [r1, #oINTSUBMSK]

	@ initialise system clocks
	mov	r1, #CLK_CTL_BASE
	mvn	r2, #0xff000000
	str	r2, [r1, #oLOCKTIME]

	mov	r1, #CLK_CTL_BASE
	mov	r2, #M_DIVN
	str	r2, [r1, #oCLKDIVN]

	mrc	p15, 0, r1, c1, c0, 0	@ read ctrl register
	orr	r1, r1, #0xc0000000	@ Asynchronous
	mcr	p15, 0, r1, c1, c0, 0	@ write ctrl register

	@mov	r1, #CLK_CTL_BASE
	@ldr 	r2, =vMPLLCON	        @ clock user set
	@str	r2, [r1, #oMPLLCON]

	bl	memsetup

	bl      InitStacks


	adr  r0,__ENTRY
	ldr  r1,_text_start
	cmp  r0,r1
	blne copy_self  

	ldr  r0,_bss_start
	ldr  r1,_bss_end
	bl    mem_clear

    	
	ldr    pc,=acoral_start	
	b 	.



copy_self:

        ldr	r1, =( (4<<28)|(2<<4)|(3<<2) )		/* address of Internal SRAM  0x4000002C*/
	mov	r0, #0		
	str	r0, [r1]


	mov	r1, #0x2c		               /* address of men  0x0000002C*/
	ldr	r0, [r1]
	cmp	r0, #0
	bne	copy_from_rom
        
        ldr	r0, =(2440)
	ldr	r1, =( (4<<28)|(2<<4)|(3<<2) )
	str	r0, [r1]
	b       copy_from_nand 



copy_from_rom:
	ldr   r1,   _text_start
	ldr   r3,   _bss_start
	ldr   r2,   [r0],#4
	str   r2,   [r1],#4
	cmp   r1,   r3
	blt   copy_from_rom	
	mov   pc,  lr

copy_from_nand:
	stmfd   sp!, {lr}
	bl      nand_init
	ldr     r0,  _text_start
	mov     r1,  #0
	ldr     r3,  _bss_start
        sub     r2,  r3, r0

        bl      nand_read
        cmp	r0,  #0x0
	beq     ok_nand_read


bad_nand_read:
         ldr     r0,=0x56000010      @ R0设为GPBCON寄存器。此寄存器
                                        @ 用于选择端口B各引脚的功能：
                                        @ 是输出、是输入、还是其他
         mov     r1,#0x00001000        
         str     r1,[r0]             @ 设置GPB5为输出口, 位[10:9]=0b01
         ldr     r0,=0x56000014      @ R0设为GPBDAT寄存器。此寄存器
                                        @ 用于读/写端口B各引脚的数据
         mov     r1,#0x00000000      @ 此值改为0x00000020,
                                        @ 可让LED2熄灭
         str     r1,[r0]             @ GPB5输出0，LED2点亮

         b       .

	 b	bad_nand_read	@ infinite loop


ok_nand_read:    	

	mov	r0, #0
	ldr	r1, _text_start
	mov	r2, #512	
go_next:
	ldr	r3, [r0], #4
	ldr	r4, [r1], #4
	cmp	r3, r4
	bne	notmatch
	cmp     r0, r2
	beq	out
	b	go_next
	

notmatch:
         ldr     r0,=0x56000010      @ R0设为GPBCON寄存器。此寄存器
                                        @ 用于选择端口B各引脚的功能：
                                        @ 是输出、是输入、还是其他
         mov     r1,#0x00000400        
         str     r1,[r0]             @ 设置GPB5为输出口, 位[10:9]=0b01
         ldr     r0,=0x56000014      @ R0设为GPBDAT寄存器。此寄存器
                                        @ 用于读/写端口B各引脚的数据
         mov     r1,#0x00000000      @ 此值改为0x00000020,
                                        @ 可让LED1熄灭
         str     r1,[r0]             @ GPB5输出0，LED1点亮

         b       .

	 b	notmatch

out:
        ldmfd sp!,{lr} 
	
		
@***************************************************************
@                       堆栈初始化
@***************************************************************

InitStacks:
	mov r2,lr
	mrs	r0,cpsr
	bic	r0,r0,#MODE_MASK
	orr	r1,r0,#UND_MODE|NOINT
	msr	cpsr_cxsf,r1		@UndefMode
	ldr	sp,=UDF_stack		@ UndefStack=0x33FF_5C00

	orr	r1,r0,#ABT_MODE|NOINT
	msr	cpsr_cxsf,r1		@AbortMode
	ldr	sp,=ABT_stack		@ AbortStack=0x33FF_6000

	orr	r1,r0,#IRQ_MODE|NOINT
	msr	cpsr_cxsf,r1		@IRQMode
	ldr	sp,=IRQ_stack		@ IRQStack=0x33FF_7000

	orr	r1,r0,#FIQ_MODE|NOINT
	msr	cpsr_cxsf,r1		@FIQMode
	ldr	sp,=FIQ_stack		@ FIQStack=0x33FF_8000

	bic	r0,r0,#MODE_MASK|NOINT
	orr	r1,r0,#SVC_MODE
	msr	cpsr_cxsf,r1		@SVCMode
	ldr	sp,=SVC_stack		@ SVCStack=0x33FF_5800

	mrs     r0,cpsr
       	bic     r0,r0,#MODE_MASK
	orr     r1,r0,#SYS_MODE|NOINT
	msr     cpsr_cxsf,r1    	@ userMode
	ldr     sp,=SYS_stack

	mov	pc,r2

@***************************************************************
@ initialise the static memory
@ set memory control registers
@***************************************************************
memsetup:
	mov	r1, #MEM_CTL_BASE
	adrl	r2, mem_cfg_val
	add	r3, r1, #52
1:	ldr	r4, [r2], #4
	str	r4, [r1], #4
	cmp	r1, r3
	bne	1b
	mov	pc, lr

@***************************************************************
@ clear memory
@ r0: start address
@ r1: length
@***************************************************************

mem_clear:
	mov r2,#0
1:	str r2,[r0],#4
	cmp r0,r1
	blt 1b
	mov pc,lr


@***************************************************************
@ Data Area
@ Memory configuration values
@***************************************************************

.align 4
mem_cfg_val:
	.long	vBWSCON
	.long	vBANKCON0
	.long	vBANKCON1
	.long	vBANKCON2
	.long	vBANKCON3
	.long	vBANKCON4
	.long	vBANKCON5
	.long	vBANKCON6
	.long	vBANKCON7
	.long	vREFRESH
	.long	vBANKSIZE
	.long	vMRSRB6
	.long	vMRSRB7
