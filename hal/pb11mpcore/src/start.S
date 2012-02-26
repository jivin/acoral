#include "hal_brd_cfg.h"
.global  __ENTRY
.global  ResetHandler
__ENTRY:
ResetHandler:
	MRS     r0, CPSR
        ORR     r0, r0, #(PSR_I_BIT|PSR_F_BIT)  @ Disable IRQ & FIQ
        MSR     CPSR_c, r0

        @
        @ Ensure that the MMU and caches are off
        @
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c5, 0           @ Invalidate I Cache
        MCR     p15, 0, r0, c7, c6, 0           @ Invalidate D Cache

        MRC     p15, 0, r0, c1, c0, 0           @ Get control register
        BIC     r0, r0, #(CTRL_M_BIT|CTRL_C_BIT)@ Disable MMU and D Cache
        BIC     r0, r0, #CTRL_I_BIT             @ Disable I Cache
        MCR     p15, 0, r0, c1, c0, 0           @ Write control register

        @
        @ Handle secondary mpcores
        @
        MRC     p15, 0, r0, c0, c0, 5
        ANDS    r0, r0, #0x0f
        BEQ     clear_leds                      @ Go if core 0 on primary core tile
        BL      __secondary_mpcore              @ Will not return
        
        @
        @ Clear the LED s
        @
clear_leds:
        MOV     r0, #BRD_BASE
        MOV     r1, #0
        STR     r1, [r0, #BRD_LED]

        @
        @ Initialise memory
        @
        BL      __platform_memory_init

        @
        @ Are we running from our load address
        @
        LDR     r4, =dummy
        ADR     r1,dummy
        CMP     r1, r4
        BXEQ    r4

        @
        @ Get ready to copy ourselves to our execution address
        @
        ADR     r1, ResetHandler
        LDR     r2, =text_start
        LDR     r3, =bss_start
        CMP     r3, #0

copyloop:     
        LDR     r0, [r1], #4
        STR     r0, [r2], #4
        CMP     r2, r3
        BCC     copyloop

        @
        @ Ensure we are running the code at the address it is linked at.
        @
        BX      r4          @ Ensure we are at the linked address
dummy:

        @
        @ Initiallise static memory controller
        @
        BL      __platform_static_memory_init

        @
        @ Create a small temporary stack, use the first 4K of memory
        @ as we sould always have some work memory here.  We can now
        @ call some simple C routines with care.
        @
        MOV     sp, #(4 << 10)          @ 4K stack

        @
        @ Basic initialization of the UART, this is so we have something
        @ to output any error message during boot.
        @
        LDR      r1, =UART0_BASE
        MOV      r0, #0                     @ Disable UART
        STR      r0, [r1,#UART_PL011_CR]
        MOV      r0, #0x27                  @ Set speed to 38400bps
        STR      r0, [r1,#UART_PL011_IBRD]
        MOV      r0, #4
        STR      r0, [r1,#UART_PL011_FBRD]
        MOV      r0, #0x70                  @ 8 bits, 1 stop bit, no parity, fifo enabled
        STR      r0, [r1,#UART_PL011_LCRH]
        LDR      r0, =0x301                 @ Enable UART
        STR      r0, [r1,#UART_PL011_CR]

copy_vectors:
        LDR     r0, =HAL_VECTR_START
        CMP     r0, #0
        BEQ     real_code
        LDR     r1, =HAL_VECTR_END
        MOV     r2, #0
1:    
        LDR     r3, [r0], #4
        STR     r3, [r2], #4
        CMP     r0, r1
        BLO     1b

real_code:

	bl InitStacks
	ldr  r0,=bss_start
	ldr  r1,=bss_end
	bl    mem_clear

	mrs     r0,cpsr
       	bic     r0,r0,#MODE_MASK
	orr     r1,r0,#MODE_SYSTEM|NOINT
	msr     cpsr_cxsf,r1    	@ userMode

	ldr     sp,=SYS_stack


        bl acoral_start
@
@ ------------------------------------------------------------
@

__platform_memory_init:

        MOV     r13, lr

        BL      __platform_dmc_init

        MOV     lr, r13

        @
        @ Have we already remapped, if so do not do it again
        @
        LDR     r8, =SC_BASE
        LDR     r6, [r8, #SC_CTRL]
        LDR     r0, =SC_REMAP_STAT
        TST     r6, r0
        BEQ     end

        @
        @ If we are running from the first 64M of memory this
        @ is going to disable when we remap, therfore we need
        @ to jump to the real copy of this code in flash.
        @
        ADR     r1, update             @ Get relative address
        CMP     r1, #(64 << 20)     @ Is this in the first 64M
        BCS     wait                 @ No, skip

        @
        @ Check boot select switches
        @
        LDR     r5, =BRD_BASE
        LDR     r6, [r5, #BRD_BOOTCS]
        AND     r6, r6, #0x1

        @
        @ Move execution to the remapped area.
        @ This will either be NOR flash or Expansion Flash.
        @
        MOV     r0, #0
        CMP     r6, #0x0
        MOVEQ   r0, #NOR_FLASH     @ NOR Flash
        CMP     r6, #0x1
        MOVEQ   r0, #EXP_FLASH     @ Expansion Flash
        ADD     pc, r0, r1

update:
	ADD     lr, r0, lr          @ Must update our return address

        @ Wait for slave cores to go into wfi
wait:
        LDR     r5, =BRD_BASE
        LDR     r6, [r5, #BRD_PLD_CTRL2]
        AND     r6, r6, #0xe000
        CMP     r6, #0xe000
        BNE     wait
        
        @ Set the REMAP bit
        LDR     r6, [r8, #SC_CTRL]
        ORR     r6, r6, #SC_REMAP
        STR     r6, [r8, #SC_CTRL]

end:
	mov pc,lr	

@ ------------------------------------------------------------
@
@ Config static memory on the SSMC with basic settings
@
@ ------------------------------------------------------------

__platform_static_memory_init:
	LDR     r0, =SSMC_BASE        

        @  Set delays for CS3
        LDR     r1, =0x0003E377
        STR     r1, [r0, #SMC_SET_CYCLES]
        
        @ set opmode bit
        LDR     r1, =0x00002002
        STR     r1, [r0, #SMC_SET_OPMODE]
        
        @ select CS3 for Ethernet
        LDR     r1, =0x01C00000
        STR     r1, [r0, #SMC_DIRECT_CMD]                

        @  Set delays for CS2 SRAM
        LDR     r1, =0x00031944
        STR     r1, [r0, #SMC_SET_CYCLES]
        
        @ set opmode bit
        LDR     r1, =0x00000aaa
        STR     r1, [r0, #SMC_SET_OPMODE]
        
        @ select CS2 for SRAM
        LDR     r1, =0x01400000
        STR     r1, [r0, #SMC_DIRECT_CMD]                

        @  Set delays for CS1 & CS0 NOR FLASH
        LDR     r1, =0x00036155
        STR     r1, [r0, #SMC_SET_CYCLES]
        
        @ set opmode bit
        LDR     r1, =0x00000aaa
        STR     r1, [r0, #SMC_SET_OPMODE]
        
        @ select CS1 
        LDR     r1, =0x00C00000
        STR     r1, [r0, #SMC_DIRECT_CMD]                

        @ select CS0 
        LDR     r1, =0x00400000
        STR     r1, [r0, #SMC_DIRECT_CMD]     

	MOV PC,LR	

@ ------------------------------------------------------------
@
@ Initializes dynamic memory controller
@
@ ------------------------------------------------------------


__platform_dmc_init:

      	LDR     r0, =DMC_BASE
    
@/* set DLL THRU mode*/
        ADD     r2,r0,#DMC_PLL
        MOV     r1, #0x1
        STR     r1, [r2, #0]
        
@/* set config mode*/
        MOV     r1, #0x4
        STR     r1, [r0, #DMC_COMMAND]
    
@/*        MOV     r1, #0x3*/
@/*        STR     r1, [r0, #DMC_ID_0_CFG]*/

@/* initialise memory controlller*/

@/* refresh period*/
        LDR     r1, =0x185
        STR     r1, [r0, #DMC_REFRESH_PRD]

@/* cas latency*/
        MOV     r1, #0x5
        STR     r1, [r0, #DMC_CAS_LATENCY]

@/* t_dqss*/
        MOV     r1, #0x1
        STR     r1, [r0, #DMC_T_DQSS]

@/* t_mrd*/
        MOV     r1, #0x3
        STR     r1, [r0, #DMC_T_MRD]

@/* t_ras*/
        MOV     r1, #0x5
        STR     r1, [r0, #DMC_T_RAS]

@/* t_rc*/
        MOV     r1, #0x8
        STR     r1, [r0, #DMC_T_RC]

@/* t_rcd*/
        MOV     r1, #0x3
        STR     r1, [r0,#DMC_T_RCD]

@/* t_rfc*/
        MOV     r1, #0xA8
        STR     r1, [r0, #DMC_T_RFC]
    
@/* t_rp*/
        MOV     r1, #0x3
        STR     r1, [r0, #DMC_T_RP]

@/* t_rrd*/
        MOV     r1, #0x2
        STR     r1, [r0, #DMC_T_RRD]

@/* t_wr*/
        MOV     r1, #0x2
        STR     r1, [r0, #DMC_T_WR]

@/* t_wtr*/
        MOV     r1, #0x1
        STR     r1, [r0, #DMC_T_WTR]

@/* t_xp*/
        MOV     r1, #0x4
        STR     r1, [r0, #DMC_T_XP]

@/* t_xsr*/
        MOV     r1, #0xC8
        STR     r1, [r0, #DMC_T_XSR]

@/* t_esr*/
        MOV     r1, #0x18
        STR     r1, [r0, #DMC_T_ESR]

@/* set memory config*/
        LDR     r1, =0x00219F93
        STR     r1, [r0, #DMC_MEMORY_CONFIG]

@/* initialise external memory chip 0 & 1*/

@/* set chip select for chip 0*/
        LDR     r1, =0x000070F0
        STR     r1, [r0, #DMC_CHIP_0_CFG]
@/* set chip select for chip 1*/
        LDR     r1, =0x000080F0
        STR     r1, [r0, #DMC_CHIP_1_CFG]

@/* delay*/
        MOV     r1, #0
1:       LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #100
        BLT      1b      
        
@/* send nop [6]*/
        LDR     r1, =0x000C0000
        STR     r1, [r0, #DMC_DIRECT_CMD]
        STR     r1, [r0, #DMC_DIRECT_CMD]        
        LDR     r1, =0x001C0000
        STR     r1, [r0, #DMC_DIRECT_CMD]
        STR     r1, [r0, #DMC_DIRECT_CMD]  

@/* pre-charge all [7]*/
        MOV     r1, #0x0
        STR     r1, [r0, #DMC_DIRECT_CMD]
        MOV     r1, #0x00100000
        STR     r1, [r0, #DMC_DIRECT_CMD]        

@/* wait tRP & nop [8]*/
        MOV     r1, #0x0c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   
        MOV     r1, #0x1c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]     
        
@/* delay*/
        MOV     r1, #0             
2:       LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #10
        BLT     2b               

@/* set extended mode register [9]*/
        LDR     r1, =0x00090000
        STR     r1, [r0, #DMC_DIRECT_CMD]
        LDR     r1, =0x00190000
        STR     r1, [r0, #DMC_DIRECT_CMD]

@/* wait tMRD & nop [10]*/
        MOV     r1, #0x0c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   
        MOV     r1, #0x1c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]  

@/* delay*/
        MOV     r1, #0               
3:       LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #10
        BLT      3b         

@/* set mode register [11]*/
        LDR     r1, =0x00080163
        STR     r1, [r0, #DMC_DIRECT_CMD]
        LDR     r1, =0x00180163
        STR     r1, [r0, #DMC_DIRECT_CMD]    
        
@/* delay*/
        MOV     r1, #0
4:       LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #100
        BLT     4b
        
@/* wait tMRD & nop [12]*/
        MOV     r1, #0x0c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   
        MOV     r1, #0x1c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]         

@/* delay*/
        MOV     r1, #0        
5:       LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #10
        BLT     5b          

@/* pre-charge all [13]*/
        MOV     r1, #0x0
        STR     r1, [r0, #DMC_DIRECT_CMD]
        MOV     r1, #0x100000
        STR     r1, [r0, #DMC_DIRECT_CMD]        
        
@/* wait tRP & nop [14]*/
        MOV     r1, #0x0c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   
        MOV     r1, #0x1c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]           
        
@/* delay*/
        MOV     r1, #0        
6:       LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #10
        BLT     6b        

@/* auto-refresh [15]*/
        MOV     r1, #0x00040000
        STR     r1, [r0, #DMC_DIRECT_CMD]
        MOV     r1, #0x00140000
        STR     r1, [r0, #DMC_DIRECT_CMD]
        
@/* wait tRFC & nop [16]*/
        MOV     r1, #0x0c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   
        MOV     r1, #0x1c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   

@/* delay*/
        MOV     r1, #0                
7:       LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #10
        BLT     7b       

@/* auto-refresh [17]*/
        MOV     r1, #0x040000
        STR     r1, [r0, #DMC_DIRECT_CMD]
        MOV     r1, #0x140000
        STR     r1, [r0, #DMC_DIRECT_CMD]        
        
@/* wait tRFC & nop [18]*/
        MOV     r1, #0x0c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   
        MOV     r1, #0x1c0000
        STR     r1, [r0, #DMC_DIRECT_CMD] 

@/* delay*/
        MOV     r1, #0                
8:   
        ldr  r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #10
        BLT     8b         

@/* set mode register [19]*/
        LDR     r1, =0x00080063
        STR     r1, [r0, #DMC_DIRECT_CMD]
        LDR     r1, =0x00180063
        STR     r1, [r0, #DMC_DIRECT_CMD]        
        
@/* wait tMRD & nop [20]*/
        MOV     r1, #0x0c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]   
        MOV     r1, #0x1c0000
        STR     r1, [r0, #DMC_DIRECT_CMD]    
        
@/* delay*/

        MOV     r1, #0             
9: 
      LDR     r3, [r0, #DMC_STATUS]    @/* read status register*/
        ADD     r1, r1, #1
        CMP     r1, #10
        BLT     9b 
    

@/*----------------------------------------    */
@/* go command*/
        MOV     r1, #0x0
        STR     r1, [r0, #DMC_COMMAND]

@/* wait for ready*/
10:      LDR     r1, [r0,#DMC_STATUS]
        TST     r1,#1
        BEQ     10b 
        mov  pc,     lr

@***************************************************************
@                       堆栈初始化
@***************************************************************

InitStacks:
	mov r2,lr
	mrs	r0,cpsr
	bic	r0,r0,#MODE_MASK
	orr	r1,r0,#MODE_UNDEF|NOINT
	msr	cpsr_cxsf,r1		@UndefMode
	ldr	sp,=UDF_stack		@ UndefStack=0x33FF_5C00

	orr	r1,r0,#MODE_ABORT|NOINT
	msr	cpsr_cxsf,r1		@AbortMode
	ldr	sp,=ABT_stack		@ AbortStack=0x33FF_6000

	orr	r1,r0,#MODE_IRQ|NOINT
	msr	cpsr_cxsf,r1		@IRQMode
	ldr	sp,=IRQ_stack		@ IRQStack=0x33FF_7000

	orr	r1,r0,#MODE_FIQ|NOINT
	msr	cpsr_cxsf,r1		@FIQMode
	ldr	sp,=FIQ_stack		@ FIQStack=0x33FF_8000

	bic	r0,r0,#MODE_MASK|NOINT
	orr	r1,r0,#MODE_SYSTEM
	msr	cpsr_cxsf,r1		@SVCMode
	ldr	sp,=SVC_stack		@ SVCStack=0x33FF_5800
	mov	pc,r2

@***************************************************************
@ clear memory
@ r0: start address
@ r1: length
@***************************************************************

mem_clear:
	MOV R2,#0
1:	STR R2,[R0],#4
	CMP R0,R1
	BNE 1b
	MOV PC,LR

__secondary_mpcore:
	LDR     r8, =SC_BASE
        LDR     r6, [r8, #SC_CTRL]
        LDR     r0, =SC_REMAP_STAT
        TST     r6, r0
        BEQ     3f

        @
        @ If we are running from the first 64M of memory this
        @ is going to disable when we remap, therfore we need
        @ to jump to the real copy of this code in flash.
        @
        ADR     r1, 1f             @ Get relative address
        CMP     r1, #(64 << 20)     @ Is this in the first 64M
        BCS     3f                 @ No, skip

        @
        @ Check boot select switches
        @
        LDR     r5, =BRD_BASE
        LDR     r6, [r5, #BRD_BOOTCS]
        AND     r6, r6, #0x1

        @
        @ Move execution to the remapped area.
        @ This will either be NOR flash or Expansion Flash.
        @
        MOV     r0, #0
        CMP     r6, #0x0
        MOVEQ   r0, #NOR_FLASH     @ NOR Flash
        CMP     r6, #0x1
        MOVEQ   r0, #EXP_FLASH     @ Expansion Flash
        ADD     pc, r0, r1

1:      ADD     lr, r0, lr          @ Must update our return address

        @ Enable software interrupt
3:
        LDR     r5, =MPCORE_CPU_INTERFACE
        MOV     r6, #0x1
        STR     r6, [r5, #CONTROL_REGISTER]
        MOV     r6, #0xF0
        STR     r6, [r5, #PRIORITY_MASK_REGISTER]

        @ Read core number into r0, required by application program
        @ on exit from wait for interrupt loop
        MRC     p15, 0, r0, c0, c0, 5
        AND     r0, r0, #0x0f

4:
        @Set WFI
        MCR     p15, 0, r2, c7, c0, 4 
        
        @ Read flag register to see if address to jump too
        LDR     r5, =BRD_BASE
        LDR     r6, [r5, #BRD_FLAGS]
        CMP     r6, #0
        BXNE    r6
        B       4b
@这个不会回去
