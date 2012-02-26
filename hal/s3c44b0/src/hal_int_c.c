#include"kernel.h"
#include"../include/hal_brd.h"
static void hal_intr_unmask(acoral_vector vector){
        rINTMSK &= ~(1<<vector);			/*打开TIMER0中断允许*/
}
static void hal_intr_mask(acoral_vector vector){
	rINTMSK |=(1<<vector);			/* 打开TIMER0中断允许*/
}

static void hal_intr_ack(acoral_u32 vector){
	rI_ISPC=1<<vector;
}

void hal_intr_init(){
	acoral_u32 i;
	rI_INTCON=0x05;
	rINTMOD=0x00000000;		/* 所有中断均为IRQ中断*/
	rINTMSK=0x03FFFFFF;
	for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		acoral_set_intr_enter(i,hal_intr_ack);
		acoral_set_intr_exit(i,NULL);
		acoral_set_intr_mask(i,hal_intr_mask);
		acoral_set_intr_unmask(i,hal_intr_unmask);
	}
}


