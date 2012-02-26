#include"acoral.h"
/*****************************************/
/*****这个函数的作用是初始化滴答时钟数据**/
/**********相关数据*********************/
/*****************************************/
static void hal_ticks_clear(acoral_vector vector){
	*(volatile acoral_u32 *)0x1001100c=0;
}

void hal_ticks_init(){
	acoral_u32 tmp=rSCCTRL;
	acoral_u32 control = TIMER_INT_ENABLED | TIMER_ENABLED | TIMER_32BIT_OPERATION|TIMER_MODE_PERIODIC;
	rSCCTRL=tmp|1<<15;
	rTCNT0=0;
	rTLD0=1000000/ACORAL_TICKS_PER_SEC;
	rTCNT0=control;
	acoral_set_intr_enter(HAL_TICKS_INTR,hal_ticks_clear);
}

acoral_u32 hal_ticks_delay(){
	return 1000000/ACORAL_TICKS_PER_SEC-HAL_REG(TIMER0_1_BASE,TIMER_VALUE);	
}
