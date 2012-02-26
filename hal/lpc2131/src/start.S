#include "../include/hal_cfg.h"
	.extern text_end
	.extern bss_start
	.extern bss_end
	.extern HAL_INTR_ENTRY
	.extern acoral_start
	.global HandleIRQ
	.global __ENTRY
#define WDMOD 	0xE0000000
#define VICINTENCLR 	0XFFFFF014
#define VICSOFTINTCLEAR 	0XFFFFF01C
#define PLLCON 		0xE01FC080
#define VPBDIV 		0xE01FC100
#define PLLCFG 		0xE01FC084
#define PLLFEED 	0xE01FC08C
#define PLLSTAT 	0xE01FC088
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
@*******setup*********************************************************

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

	ldr	r1, =WDMOD
	mov	r2, #0x0
	str	r2, [r1]

	@ disable all interrupts
	ldr 	r1,=VICINTENCLR
	ldr 	r0, =0xFFFFFFFF
	str 	r0, [r1]

	ldr 	r1, =VICSOFTINTCLEAR
	ldr 	r0, =0xFFFFFFFF
	str 	r0, [r1]


	@ initialise system clocks
	ldr 	r1, =PLLCON
	ldrb 	r0, =vPLLCON_ENABLE
	strb 	r0, [r1]

	ldr 	r1,=VPBDIV    /*设定PCLK=CCLK/2*/
	ldr 	r0,=0x2
	str 	r0,[r1]


	ldr 	r0,=PLLCFG
	ldrb 	r1,=vPLLCFG
	strb 	r1,[r0]

	mov 	r0,#0xAA
	mov 	r1,#0x55
	ldr 	r2,=PLLFEED
	str 	r0,[r2]
	str 	r1,[r2]
		
t: 	ldr 	r1,=PLLSTAT
	ldr 	r0,[r1]
	tst 	r0,#0x400
	BEQ 	t

	mov 	r0,#0xAA
	mov 	r1,#0x55
	ldr 	r2,=PLLFEED
	str 	r0,[r2]
	str 	r1,[r2]


	
@****************************************************************
@ relocate 
@****************************************************************
cur:	ldr     r0,=text_end             /* r0 <- current position of code   */
        ldr     r1, =data_start           /* test if we run from flash or RAM */
        cmp     r0, r1             /* don't reloc during debug         */
        beq     end_copy 

@****************************************************************
@ Copy and paste RW data/zero initialized data 
@****************************************************************
	ldr     r1,=text_end
        ldr     r2,=data_start
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

	b	acoral_start	@ call sys_start
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
@ clear memory
@ r0: start address
@ r1: length
@***************************************************************

mem_clear:
	mov r2,#0
1:	str r2,[r0],#4
	cmp r0,r1
	bne 1b
	mov pc,lr


