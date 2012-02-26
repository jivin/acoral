#include<kernel.h>
#include"../include/hal_brd.h"
static void hal_intr_unmask(acoral_vector vector){
        rVICINTENABLE |= (1<<vector);			/*使能vector号中断线*/
}

static void hal_intr_mask(acoral_vector vector){
	rVICINTENCLR |=(1<<vector);			/*屏蔽vector号中断线*/
}

static void hal_intr_ack(acoral_u32 vector){ 		/*对应VicIrqStatus位清零，表示处理中断*/
	int temp=0,i;
	for(i=0;i<vector;i++)
		temp |=1<<i;
	rVICIRQSTATUS &= ~(1<<vector);
	rVICINTENCLR  =~temp; 		
	/*因为中断优先级由内核判断，所以中断响应阶段mask掉低优先中断，确保低优先中断不会抢占高优先级中断*/
}

static void hal_intr_exit(acoral_u32 vector)
{
	rVICINTENABLE =0x7FFFFF; 	/*使能所有中断线*/
}


void hal_intr_init(){
	acoral_u32 i;
	rVICINTSELECT=0x00000000;		/* 所有中断均为IRQ中断*/
	rVICVECTCNTL0=0;
	rVICVECTCNTL1=0;
	rVICVECTCNTL2=0;
	rVICVECTCNTL3=0;
	rVICVECTCNTL4=0;
	rVICVECTCNTL5=0;
	rVICVECTCNTL6=0;
	rVICVECTCNTL7=0;
	rVICVECTCNTL8=0;
	rVICVECTCNTL9=0;
	rVICVECTCNTL10=0;
	rVICVECTCNTL11=0;
	rVICVECTCNTL12=0;
	rVICVECTCNTL13=0;
	rVICVECTCNTL14=0;
	rVICVECTCNTL15=0;


	/*所有的32个中断为非向量IRQ*/
	/*rVICDEFVECTADDR =HAL_INTR_ENTRY; 	/*所有非向量IRQ的公共中断处理程序*/
	rVICINTENABLE=0X7FFFFF; 		/*使能所有（21个）中断线*/
	for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		acoral_set_intr_enter(i,hal_intr_ack);
		acoral_set_intr_exit(i,hal_intr_exit);
		acoral_set_intr_mask(i,hal_intr_mask);
		acoral_set_intr_unmask(i,hal_intr_unmask);
	}
}


