#include "hal_2440_cfg.h"
.global HAL_REAL_SLEEP
.global HAL_REAL_RESUME
.extern flush_cache
.extern regs_save_ptr

HAL_REAL_SLEEP:
	stmfd	sp!, { r4 - r12, lr }
	mrs  r4,cpsr
	stmfd   sp!,{r4}
	@@ store co-processor registers

	mrc	p15, 0, r4, c15, c1, 0	@ CP access register
	mrc	p15, 0, r5, c13, c0, 0	@ PID
	mrc	p15, 0, r6, c3, c0, 0	@ Domain ID
	mrc	p15, 0, r7, c2, c0, 0	@ translation table base address
	mrc	p15, 0, r8, c2, c0, 0	@ auxiliary control register
	mrc	p15, 0, r9, c1, c0, 0	@ control register

	stmia	r0, { r4 - r13 }

	@ Flush the caches to ensure everything is back out to
	@ SDRAM before the core powers down

	bl 	flush_cache

	@ prepare cpu to sleep

	ldr	r4, =REFRESH
	ldr	r5, =MISCCR
	ldr	r6, =CLKCON
	ldr	r7, [ r4 ]		@ get REFRESH (and ensure in TLB)
	ldr	r8, [ r5 ]		@ get MISCCR (and ensure in TLB)
	ldr	r9, [ r6 ]		@ get CLKCON (and ensure in TLB)

	orr	r7, r7, #SELF_REFRESH	@ SDRAM sleep command
	orr	r8, r8, #MISCCR_SDSLEEP @ SDRAM power-down signals
	orr	r9, r9, #CLKCON_POWER	@ power down command

	teq	pc, #0			@ first as a trial-run to load cache
	bl	HAL_DO_SLEEP
	teq	r0, r0			@ now do it for real
	b	HAL_DO_SLEEP	        @

	@@ align next bit of code to cache line

	.align	8
HAL_DO_SLEEP:

	streq	r7, [ r4 ]			@ SDRAM sleep command
	
	mov	r1, #16                         @loop until self-refresh is ok
1:	subs	r1, r1, #1	
	bne	1b

	@Power down sdram
	streq	r8, [ r5 ]			@ SDRAM power-down config

	streq	r9, [ r6 ]			@ CPU sleep
1:	beq	1b
	mov	pc, r14


HAL_REAL_RESUME:
	mov	r0, #PSR_I_BIT | PSR_F_BIT | SYS_MODE
	msr	cpsr_c, r0
	mrs     r0,cpsr
	bic     r0,r0,#MODE_MASK
	orr     r1,r0,#SYS_MODE|NOINT
	msr     cpsr_cxsf,r1    	@

	mov	r1, #0
	mcr	p15, 0, r1, c8, c7, 0		@@ invalidate I & D TLBs
	mcr	p15, 0, r1, c7, c7, 0		@@ invalidate I & D caches

	ldr	r0, =regs_save_ptr	        @ address of restore block
	ldr     r0,[r0]
	ldmia	r0, { r4 - r13 }

	mcr	p15, 0, r4, c15, c1, 0		@ CP access register
	mcr	p15, 0, r5, c13, c0, 0		@ PID
	mcr	p15, 0, r6, c3, c0, 0		@ Domain ID
	mcr	p15, 0, r7, c2, c0, 0		@ translation table base
	mcr	p15, 0, r8, c1, c1, 0		@ auxilliary control
	mcr	p15, 0, r9, c1, c0, 0	        @ control register


	nop			
	ldmfd   sp!,{r4}
	msr  cpsr_cxsf,r4
	ldmfd	sp!, { r4 - r12, pc }
