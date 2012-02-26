#include<type.h>
#include"../include/hal_brd.h"
#include<core.h>
/*****************************************/
/*****这个函数的作用是初始化滴答时钟数据**/
/**********相关数据*********************/
/*****************************************/
void hal_ticks_init(){
  	rTCON = rTCON & (~0xf) ;			/* clear manual update bit, stop Timer0*/
   	rTCFG0 &= 0xFFFF00;
 	rTCFG0 |= 0xF9;        /* prescaler等于249*/
 	rTCFG1 &= ~0x0000F;   
 	rTCFG1 |= 0x2;     /*divider等于8，则设置定时器4的时钟频率为25kHz*/

   	rTCNTB0 = PCLK /(8*(249))/ACORAL_TICKS_PER_SEC; 	/*定时器输入时钟频率=MCLK/{预分频值}/{分割值}*/
								/*{预分频值}=1~255;*/
								/*{分割值}=2,4,8,16,32*/
   	rTCON = rTCON & (~0xf) |0x02;              	/* updata*/
	rTCON = rTCON & (~0xf) |0x09; 			/* start*/
}

acoral_u32 hal_ticks_delay(){
	return 0;
}
