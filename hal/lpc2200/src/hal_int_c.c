#include"../include/hal_brd.h"
#include"hal_port.h"
extern void IRQ_ISR0();
extern void IRQ_ISR1();
extern void IRQ_ISR2();
extern void IRQ_ISR3();
extern void IRQ_ISR4();
extern void IRQ_ISR5();
extern void IRQ_ISR6();
extern void IRQ_ISR7();
extern void IRQ_ISR8();
extern void IRQ_ISR9();
extern void IRQ_ISR10();
extern void IRQ_ISR11();
extern void IRQ_ISR12();
extern void IRQ_ISR13();
extern void IRQ_ISR14();
extern void IRQ_ISR15();
extern void IRQ_ISRDEF();
static void hal_intr_unmask(acoral_vector vector){
        //rINTMSK &= ~(1<<vector);			/*打开TIMER0中断允许*/
        VICIntEnable=(1<<vector);
}

static void hal_intr_mask(acoral_vector vector){
	//rINTMSK |=(1<<vector);			/* 打开TIMER0中断允许*/
	VICIntEnClr=(1<<vector);
}

void hal_intr_ack(acoral_u32 vector){
	//rI_ISPC=1<<vector;
	if((vector<=17)&&(vector>=14))
		EXTINT=(1<<(vector-14));
	VICVectAddr=0;
}

void hal_intr_init(){
	acoral_u32 i;
	//rI_INTCON=0x05;
	//rINTMOD=0x00000000;		/* 所有中断均为IRQ中断*/
	//rINTMSK=0x03FFFFFF;
	VICIntSelect=0x00000000;
	VICIntEnClr=0xFFFFFFFF;
	VICVectAddr0=(acoral_u32)IRQ_ISR0;
	VICVectCntl0=0x20|0;
	VICVectAddr1=(acoral_u32)IRQ_ISR1;
	VICVectCntl1=0x20|1;
	VICVectAddr2=(acoral_u32)IRQ_ISR2;
	VICVectCntl2=0x20|2;
	VICVectAddr3=(acoral_u32)IRQ_ISR3;
	VICVectCntl3=0x20|3;
	VICVectAddr4=(acoral_u32)IRQ_ISR4;
	VICVectCntl4=0x20|4;
	VICVectAddr5=(acoral_u32)IRQ_ISR5;
	VICVectCntl5=0x20|5;
	VICVectAddr6=(acoral_u32)IRQ_ISR6;
	VICVectCntl6=0x20|6;
	VICVectAddr7=(acoral_u32)IRQ_ISR7;
	VICVectCntl7=0x20|7;
	VICVectAddr8=(acoral_u32)IRQ_ISR8;
	VICVectCntl8=0x20|8;
	VICVectAddr9=(acoral_u32)IRQ_ISR9;
	VICVectCntl9=0x20|9;
	VICVectAddr10=(acoral_u32)IRQ_ISR10;
	VICVectCntl10=0x20|10;
	VICVectAddr11=(acoral_u32)IRQ_ISR11;
	VICVectCntl11=0x20|11;
	VICVectAddr12=(acoral_u32)IRQ_ISR12;
	VICVectCntl2=0x20|12;
	VICVectAddr13=(acoral_u32)IRQ_ISR13;
	VICVectCntl13=0x20|13;
	VICVectAddr14=(acoral_u32)IRQ_ISR14;
	VICVectCntl14=0x20|14;
	VICVectAddr15=(acoral_u32)IRQ_ISR15;
	VICVectCntl15=0x20|15;
	VICDefVectAddr=(acoral_u32)IRQ_ISRDEF;

	for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		acoral_set_intr_enter(i,hal_intr_ack);
		acoral_set_intr_exit(i,NULL);
		acoral_set_intr_mask(i,hal_intr_mask);
		acoral_set_intr_unmask(i,hal_intr_unmask);
	}
}

