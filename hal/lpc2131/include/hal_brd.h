#ifndef HAL_BRD_H
#define HAL_BRD_H
#include<type.h>
#include"hal_cfg.h"
/* INTERRUPT*/
#define rVICIRQSTATUS		(*(volatile acoral_u32*)0xfffff000)
#define rVICFIQSTATUS		(*(volatile acoral_u32*)0xfffff004)
#define rVICRAWINTR		(*(volatile acoral_u32*)0xfffff008)
#define rVICINTSELECT		(*(volatile acoral_u32*)0xfffff00c)
#define rVICINTENABLE		(*(volatile acoral_u32*)0xfffff010)
#define rVICINTENCLR		(*(volatile acoral_u32*)0xfffff014)
#define rVICSOFTINT		(*(volatile acoral_u32*)0xfffff018)
#define rVICSOTINTCLEAR		(*(volatile acoral_u32*)0xfffff01c)
#define rVICPROTECTION		(*(volatile acoral_u32*)0xfffff020)
#define rVICVECTADDR		(*(volatile acoral_u32*)0xfffff030)
#define rVICDEFVECTADDR		(*(volatile acoral_u32*)0xfffff034)
#define rVICVECTADDR0		(*(volatile acoral_u32*)0xfffff100)
#define rVICVECTADDR1		(*(volatile acoral_u32*)0xfffff104)
#define rVICVECTADDR2		(*(volatile acoral_u32*)0xfffff108)
#define rVICVECTADDR3           (*(volatile acoral_u32*)0xfffff10c)
#define rVICVECTADDR4		(*(volatile acoral_u32*)0xfffff110)
#define rVICVECTADDR5		(*(volatile acoral_u32*)0xfffff114)
#define rVICVECTADDR6		(*(volatile acoral_u32*)0xfffff118)
#define rVICVECTADDR7 		(*(volatile acoral_u32*)0xfffff11c)
#define rVICVECTADDR8		(*(volatile acoral_u32*)0xfffff120)
#define rVICVECTADDR9		(*(volatile acoral_u32*)0xfffff124)
#define rVICVECTADDR10		(*(volatile acoral_u32*)0xfffff128)
#define rVICVECTADDR11		(*(volatile acoral_u32*)0xfffff12c)
#define rVICVECTADDR12		(*(volatile acoral_u32*)0xfffff130)
#define rVICVECTADDR13		(*(volatile acoral_u32*)0xfffff134)
#define rVICVECTADDR14		(*(volatile acoral_u32*)0xfffff138)
#define rVICVECTADDR15		(*(volatile acoral_u32*)0xfffff13c)
#define rVICVECTCNTL0		(*(volatile acoral_u32*)0xfffff200)
#define rVICVECTCNTL1		(*(volatile acoral_u32*)0xfffff204)
#define rVICVECTCNTL2		(*(volatile acoral_u32*)0xfffff208)
#define rVICVECTCNTL3		(*(volatile acoral_u32*)0xfffff20c)
#define rVICVECTCNTL4		(*(volatile acoral_u32*)0xfffff210)
#define rVICVECTCNTL5		(*(volatile acoral_u32*)0xfffff214)
#define rVICVECTCNTL6		(*(volatile acoral_u32*)0xfffff218)
#define rVICVECTCNTL7		(*(volatile acoral_u32*)0xfffff21c)
#define rVICVECTCNTL8		(*(volatile acoral_u32*)0xfffff220)
#define rVICVECTCNTL9		(*(volatile acoral_u32*)0xfffff224)
#define rVICVECTCNTL10		(*(volatile acoral_u32*)0xfffff228)
#define rVICVECTCNTL11		(*(volatile acoral_u32*)0xfffff22c)
#define rVICVECTCNTL12		(*(volatile acoral_u32*)0xfffff230)
#define rVICVECTCNTL13		(*(volatile acoral_u32*)0xfffff234)
#define rVICVECTCNTL14		(*(volatile acoral_u32*)0xfffff238)
#define rVICVECTCNTL15		(*(volatile acoral_u32*)0xfffff23c)
   






/* UART*/
#define rU0RBR 		(*(volatile acoral_u8 *)0xE000C000)
#define rU1RBR 		(*(volatile acoral_u8 *)0xE0010000)
#define rU0THR 		(*(volatile acoral_u8 *)0xE000C000)
#define rU1THR 		(*(volatile acoral_u8 *)0xE0010000)
#define rU0DLL 		(*(volatile acoral_u8 *)0xE000C000)
#define rU1DLL 		(*(volatile acoral_u8 *)0xE0010000)
#define rU0IER 		(*(volatile acoral_u8 *)0xE000C004)
#define rU1IER 		(*(volatile acoral_u8 *)0xE0010004)
#define rU0IIR 		(*(volatile acoral_u8 *)0xE000C008)
#define rU1IIR 		(*(volatile acoral_u8 *)0xE0010008)
#define rU0FCR 		(*(volatile acoral_u8 *)0xE000C008)
#define rU1FCR 		(*(volatile acoral_u8 *)0xE0010008)
#define rU0LCR 		(*(volatile acoral_u8 *)0xE000C00C)
#define rU1LCR 		(*(volatile acoral_u8 *)0xE001000C)
#define rU0LSR 		(*(volatile acoral_u8 *)0xE000C014)
#define rU1LSR 		(*(volatile acoral_u8 *)0xE0010014)
#define rU0SCR 		(*(volatile acoral_u8 *)0xE000C01C)
#define rU1SCR 		(*(volatile acoral_u8 *)0xE001001C)
#define rU0TER 		(*(volatile acoral_u8 *)0xE000C030)
#define rU1TER 		(*(volatile acoral_u8 *)0xE0010030)






/* PWM TIMER*/
#define rT0IR  	(*(volatile acoral_u32 *)0xE0004000)	
#define rT1IR  	(*(volatile acoral_u32 *)0xE0008000)	
#define rT0TCR  (*(volatile acoral_u32 *)0xE0004004)	
#define rT1TCR 	(*(volatile acoral_u32 *)0xE0008004)	
#define rT0TC 	(*(volatile acoral_u32 *)0xE0004008)	
#define rT1TC 	(*(volatile acoral_u32 *)0xE0008008)	
#define rT0PR 	(*(volatile acoral_u32 *)0xE000400C)	
#define rT1PR 	(*(volatile acoral_u32 *)0xE000800C)	
#define rT0PC 	(*(volatile acoral_u32 *)0xE0004010)	
#define rT1PC 	(*(volatile acoral_u32 *)0xE0008010)	
#define rT0MCR 	(*(volatile acoral_u32 *)0xE0004014)	
#define rT1MCR 	(*(volatile acoral_u32 *)0xE0008014)	
#define rT0MR0 	(*(volatile acoral_u32 *)0xE0004018)	
#define rT1MR0 	(*(volatile acoral_u32 *)0xE0008018)	
#define rT0MR1 	(*(volatile acoral_u32 *)0xE000401c)	
#define rT1MR1 	(*(volatile acoral_u32 *)0xE000801c)	
#define rT0MR2 	(*(volatile acoral_u32 *)0xE0004020)	
#define rT1MR2 	(*(volatile acoral_u32 *)0xE0008020)	
#define rT0MR3 	(*(volatile acoral_u32 *)0xE0004024)	
#define rT1MR3 	(*(volatile acoral_u32 *)0xE0008024)	
#define rT0CCR 	(*(volatile acoral_u32 *)0xE0004028)	
#define rT1CCR 	(*(volatile acoral_u32 *)0xE0008028)	
#define rT0CR0  (*(volatile acoral_u32 *)0xE000402C)	
#define rT1CR0  (*(volatile acoral_u32 *)0xE000802C)	
#define rT0CR1  (*(volatile acoral_u32 *)0xE0004030)	
#define rT1CR1  (*(volatile acoral_u32 *)0xE0008030)
#define rT0CR2  (*(volatile acoral_u32 *)0xE0004034)	
#define rT1CR2  (*(volatile acoral_u32 *)0xE0008034)
#define rT0CR3  (*(volatile acoral_u32 *)0xE0004038)	
#define rT1CR3  (*(volatile acoral_u32 *)0xE0008038)
#define rT0EMR  (*(volatile acoral_u32 *)0xE000403C)
#define rT1EMR  (*(volatile acoral_u32 *)0xE000803C)
#define rT0CTCR (*(volatile acoral_u32 *)0xE0004070)
#define rT1CTCR (*(volatile acoral_u32 *)0xE0008070)

#define rPWMIR 	(*(volatile acoral_u32 *)0xE0014000)
#define rPWMTCR (*(volatile acoral_u32 *)0xE0014004)
#define rPWMTC 	(*(volatile acoral_u32 *)0xE0014008)
#define rPWMPR 	(*(volatile acoral_u32 *)0xE001400C)
#define rPWMPC 	(*(volatile acoral_u32 *)0xE0014010)
#define rPWMMCR (*(volatile acoral_u32 *)0xE0014014)
#define rPWMMRO (*(volatile acoral_u32 *)0XE0014018)
#define rPWMMR1 (*(volatile acoral_u32 *)0XE001401C)
#define rPWMMR2 (*(volatile acoral_u32 *)0XE0014020)
#define rPWMMR3 (*(volatile acoral_u32 *)0XE0014024)
#define rPWMMR4 (*(volatile acoral_u32 *)0XE0014040)
#define rPWMMR5 (*(volatile acoral_u32 *)0XE0014044)
#define rPWMMR6 (*(volatile acoral_u32 *)0XE0014048)
#define rPWMPCR (*(volatile acoral_u32 *)0XE001404C)
#define rPWMLER (*(volatile acoral_u32 *)0XE0014050)





/* WATCH DOG TIMER*/
#define rWDMOD    (*(volatile acoral_u32 *)0xe0000000)	/*Watch-dog time mode*/
#define rWDTC     (*(volatile acoral_u32 *)0xe0000004)	/*Watch-dog time deadline*/
#define rWDFEED   (*(volatile acoral_u32 *)0xe0000008)	/*Watch-dog time refresh value*/
#define rWDTV     (*(volatile acoral_u32 *)0xe000000c)  /*Watch-dog time current value*/



/*lpc2131 SYSTEM CONTROL*/
#define rEXTINT         (*(volatile acoral_u32*)0xE01FC140)
#define rEXTWAKE 	(*(volatile acoral_u32*)0xE01FC144)
#define rEXTMODE 	(*(volatile acoral_u32*)0xE01FC148)
#define rEXTPOLAR 	(*(volatile acoral_u32*)0xE01FC14C)

#define rMEMMAP 	(*(volatile acoral_u32*)0xE01FC040)
#define rPLLCON 	(*(volatile acoral_u32*)0xE01FC080)
#define rPLLCFG 	(*(volatile acoral_u32*)0xE01FC084)
#define rPLLSTAT 	(*(volatile acoral_u32*)0xE01FC088)
#define rPLLFEED 	(*(volatile acoral_u32*)0xE01FC08C)

#define rPCON 		(*(volatile acoral_u32*)0xE01FC0C0)
#define rPCONP 		(*(volatile acoral_u32*)0xE01FC0C4)

#define rRSID 		(*(volatile acoral_u32*)0xE01FC180)
#define rCSPR 		(*(volatile acoral_u32*)0xE01FC184)
#define rVPBDIV (*(volatile acoral_u32 *)0xE01FC100) 	/*VPB divider*/

#endif

