#ifndef HAL_BOARD_H
#define HAL_BOARD_H
/*tick相关寄存器*/
#define rTickCnt (*(volatile acoral_u32 *)0xE000E010)	/*Timer count observation 4*/
#define rTickRld (*(volatile acoral_u32 *)0xE000E014)	/*Timer count observation 4*/
#define rTickCur (*(volatile acoral_u32 *)0xE000E018)	/*Timer count observation 4*/
#define rTickPrio (*(volatile acoral_u8 *)0xE000ED23)	/*Timer count observation 4*/

#endif
