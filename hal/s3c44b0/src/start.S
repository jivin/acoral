#include<hal_brd.h>
.extern text_start
.extern bbs_start
.extern bbs_end
.extern HAL_INTR_ENTRY
.extern acoral_start

.global  __ENTRY
.global HandleIRQ
#define USR_MODE		0x10
#define FIQ_MODE		0x11
#define IRQ_MODE		0x12
#define SVC_MODE		0x13
#define ABT_MODE		0x17
#define UND_MODE		0x1b
#define SYS_MODE		0x1f
#define MODE_MASK		0x1f
#define NOINT        	0xc0
#define MEM_CTL_BASE 	0x48000000
#define INTCON (0x01c00000+0x200000)
#define INTMSK (0x01c00000+0x20000c)
#define LOCKTIME (0x01c00000+0x18000c)
#define PLLCON (0x01c00000+0x180000)
#define CLKCON (0x01c00000+0x180004)
#define WTCON (0x01c00000+0x130000)
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
	ldr pc,=acoral_start
HandleSWI:
	ldr pc,=acoral_start
HandleDabort:
	ldr pc,=acoral_start
HandlePabort:
	ldr pc,=acoral_start
@****************************************************************
@             ResetHandler fuction
@****************************************************************

ResetHandler:
	@set the cpu to svc32 mode
	mrs 	r0,cpsr
	bic 	r0,r0,#0x1f
	orr 	r0,r0,#0x13
	msr 	cpsr,r0
       

	@ disable watch dog timer
	ldr	r1, =WTCON
	mov	r2, #0x0
	str	r2, [r1]

	@ disable all interrupts
	ldr 	r1,=INTMSK
	ldr 	r0, =0x03fffeff
	str 	r0, [r1]

	ldr 	r1, =INTCON
	ldr 	r0, =0x05
	str 	r0, [r1]


	@ initialise system clocks
	ldr 	r1, =LOCKTIME
	ldrb 	r0, =800
	strb 	r0, [r1]

	ldrb 	r1, =PLLCON
	ldr 	r0, =vPLLCON
	@ldr 	r0, =0x34031
	str 	r0,[r1]
		
	ldr 	r1,=CLKCON
	ldr 	r0,=0x7ff8
	str 	r0,[r1]

	bl	memsetup
	
@****************************************************************
@ relocate 
@****************************************************************
cur:	adr     r0, cur             /* r0 <- current position of code   */
        ldr     r1, =cur           /* test if we run from flash or RAM */
        cmp     r0, r1             /* don't reloc during debug         */
        beq     end_copy 

@****************************************************************
@ Copy and paste RW data/zero initialized data 
@****************************************************************
	mov     r1,#0
        ldr     r2,=text_start
        ldr     r3,=bss_start
copy_loop:
	ldr     r0,[r1],#4
	str     r0,[r2],#4
	cmp     r2,r3
        blt     copy_loop
end_copy:

	ldr  r0,=bss_start
	ldr  r1,=bss_end
	bl    mem_clear

	bl     InitStacks
	mrs     r0,cpsr
       	bic     r0,r0,#MODE_MASK
	orr     r1,r0,#SYS_MODE|NOINT
	msr     cpsr_cxsf,r1    	@ userMode
	ldr     sp,=SYS_stack

	ldr     pc,=acoral_start	@ call sys_start
	b 	.


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
	mov	pc,r2

@***************************************************************
@ initialise the static memory
@ set memory control registers
@***************************************************************

memsetup:
	ldr	r1, =MEM_CTL_BASE
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
