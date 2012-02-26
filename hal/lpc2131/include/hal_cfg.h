#ifndef HAL_CFG_H
#define HAL_CFG_H

#define USR_MODE		0x10
#define FIQ_MODE		0x11
#define IRQ_MODE		0x12
#define SVC_MODE		0x13
#define ABT_MODE		0x17
#define UND_MODE		0x1b
#define SYS_MODE		0x1f
#define MODE_MASK		0x1f
#define NOINT        	0xc0

#define CCLK  60000000 /*主频*/
#define FOSC  10000000 /*晶振频率*/
#define M 6 	/*倍增器值*/
#define P 1	 	/*分频器值*/
#define FCCO 	(2*P*CCLK)	/*frequence of  CCO*/

#define vPLLCFG 	(P<<5)|M
#define vPLLCON_ENABLE 	0x1
#define vPLLCON_LINK 	vPLLCON_ENABLE|(1<<1)
#define PCLK  (CCLK/2)  /*外部设备时钟*/

#endif
