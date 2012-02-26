#include"type.h"
#include"../include/hal_brd.h"
#include"kernel.h"
/*****************************************/
/*****这个函数的作用是初始化滴答时钟数据**/
/**********相关数据*********************/
/*****************************************/
extern void hal_intr_ack(acoral_vector vector);
void hal_tick_ack(acoral_vector vector){
	T0IR=0x01;
	hal_intr_ack(vector);
}
void hal_ticks_init(){	
    T0IR = 0xffffffff;
    T0TC = 0;
    T0TCR = 0x01;
    T0MCR = 0x03;
    T0MR0 = Fpclk / ACORAL_TICKS_PER_SEC;
    acoral_set_intr_enter(HAL_TICKS_INTR,hal_tick_ack);
}

acoral_u32 hal_ticks_delay(){
	return 0;
}
