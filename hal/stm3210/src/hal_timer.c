#include<core.h>
#include<hal_port.h>
#include"../include/hal_brd.h"
/*****************************************/
/*****这个函数的作用是初始化滴答时钟数据**/
/**********相关数据*********************/
/*****************************************/
extern void hal_intr_ack(acoral_vector vector);
void hal_ticks_ack(acoral_vector vector){
    acoral_u32 tmp=rTickCnt; /*这个因为定义为valotile了，所以不会被优化为无*/
    hal_intr_ack(vector); 
}

void hal_ticks_mask(acoral_vector vector){
      hal_intr_mask(vector);
      rTickCnt =0x6;
}

void hal_ticks_unmask(acoral_vector vector){
      hal_intr_unmask(vector);
      rTickCnt =0x7;
}

void hal_ticks_init(){
    rTickRld =72*1000000/ACORAL_TICKS_PER_SEC;
    rTickCnt =0x7;
    rTickPrio =0x00;
    acoral_set_intr_enter(HAL_TICKS_INTR,hal_ticks_ack);
    acoral_set_intr_mask(HAL_TICKS_INTR,hal_ticks_mask);
    acoral_set_intr_unmask(HAL_TICKS_INTR,hal_ticks_unmask);
}

acoral_u32 hal_ticks_delay(){
	return (72*1000000/ACORAL_TICKS_PER_SEC -rTickCur)/72;
}
