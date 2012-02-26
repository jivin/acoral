#include<core.h>
#include"../include/hal_brd.h"
/*****************************************/
/*****这个函数的作用是初始化滴答时钟数据**/
/**********相关数据*********************/
/*****************************************/
void hal_ticks_init(){
	rT0TCR=0x0; 	/*关闭T0*/
	rT0CTCR=0x0;
	/*定时器模式，每个pclk上升沿PC递增*/
	rT0PR = (PCLK / ACORAL_TICKS_PER_SEC);
	rT0MR0=0x1;
	rT0MCR=0x2; 	/*TC复位并产生中断*/

	rT0TCR=1; 	/*T0开始计数*/
}

acoral_u32 hal_ticks_delay(){
	return 0;
}
