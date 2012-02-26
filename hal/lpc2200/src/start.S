#define USR_MODE		0x10
#define FIQ_MODE		0x11
#define IRQ_MODE		0x12
#define SVC_MODE		0x13
#define ABT_MODE		0x17
#define UND_MODE		0x1b
#define SYS_MODE		0x1f
#define MODE_MASK		0x1f
#define NOINT        	0xc0

.equ   PINSEL2     , 0xE002C014
.equ   BCFG0       , 0xFFE00000
.equ   BCFG1       , 0xFFE00004
.equ   BCFG2       , 0xFFE00008
.equ   BCFG3       , 0xFFE0000C

	.extern text_end
	.extern bss_start
	.extern bss_end
	.extern HAL_INTR_ENTRY
	.extern acoral_start
	.global __ENTRY

__ENTRY:
	LDR     PC, ResetAddr
        LDR     PC, Exp_Addr
        LDR     PC, Exp_Addr
        LDR     PC, Exp_Addr
        LDR     PC, Exp_Addr
        .long     0xb9205f80
        LDR     PC, [PC, #-0xff0]
        LDR     PC, FIQ_Addr

ResetAddr:           
	.long     ResetHandler
Exp_Addr:
	.long	EXP_HANDLER
IRQ_Addr:
        .long  HAL_INTR_ENTRY
FIQ_Addr :
        .long   HAL_INTR_ENTRY

@****************************************************************
@             ResetHandler fuction
@****************************************************************

ResetHandler:
	LDR     R0, =PINSEL2
    @IF :DEF: EN_CRP
    @    LDR     R1, =0x0f814910
    @ELSE
        LDR     R1, =0x0f814914
    @ENDIF
        STR     R1, [R0]

        LDR     R0, =BCFG0
        LDR     R1, =0x1000ffef				
        STR     R1, [R0]

        LDR     R0, =BCFG1
        LDR     R1, =0x10001460            	@/* 配置外部RAM访问速度为最快 */ 
        STR     R1, [R0]

        LDR     R0, =BCFG2
        LDR     R1, =0x10001460			@/* 配置Bank2为16位总线模试，前速度为最快 */
        STR     R1, [R0]

	
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


