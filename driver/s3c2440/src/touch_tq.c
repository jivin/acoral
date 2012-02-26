#include<driver.h>
#include<int.h>
#include<hal.h>
#include<ts.h>
#include<sem.h>
#include"../include/reg.h"
#define ADCPRS 9
void ts_isr(acoral_vector vector);
acoral_u32 tc_pos_x,tc_pos_y;
static acoral_u8 flag=ACORAL_TS_DOWN;
static acoral_u8 event;
acoral_dev_ops_t ts_ops;
acoral_evt_t *sem;
void touchscreen_init(void)
{
	rADCDLY = (50000);    // ADC Start or Interval Delay
	rADCCON = (1<<14)|(ADCPRS<<6)|(0<<3)|(0<<2)|(0<<1)|(0); 
      // Enable Prescaler,Prescaler,AIN5/7 fix,Normal,Disable read start,No operation
	rADCTSC = (0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);//tark
      // Down,YM:GND,YP:AIN5,XM:Hi-z,XP:AIN7,XP pullup En,Normal,Waiting for interrupt mode
	acoral_intr_set_type(IRQ_ADC,ACORAL_EXPERT_INTR);
	acoral_intr_attach(IRQ_ADC,ts_isr);
	rINTMSK  &=~(BIT_ADC);
	rINTSUBMSK  &=~(BIT_SUB_TC);
}

void ts_off(void)
{
	rINTMSK    |= (BIT_ADC);
     	rINTSUBMSK |= (BIT_SUB_TC);
}

void  ts_isr(acoral_vector vector)
{
	acoral_32 num;
      	rINTSUBMSK |= (BIT_SUB_ADC | BIT_SUB_TC);     //Mask sub interrupt (ADC and TC) 
     
     //等待中断模式是在触笔落下时产生一个中断，在这种模式下，A/D触摸屏控制寄存器ADCTSC的值应为0xD3，在系统响应中断后，XY坐标的测量模式必须为无操作模式，即寄存器ADCTSC的低两位必须清零 
     //寄存器ADCTSC的第3位可以选择上拉电阻的使能，在等待中断模式下，上拉电阻要有效，在触发中断后，上拉电阻要无效
     //Auto X-Position and Y-Position Read
	rADCTSC=(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);
          //[7] YM=GND, [6] YP is connected with AIN[5], [5] XM=Hi-Z, [4] XP is connected with AIN[7]
          //[3] XP pull-up disable, [2] Auto(sequential) X/Y position conversion mode, [1:0] No operation mode            
	rADCCON |= 0x1;             //Start Auto conversion
	while(rADCCON & 0x1);//      //Check if Enable_start is low
	while(!(0x8000&rADCCON));//   //Check ECFLG
    
	tc_pos_x = (0x3ff&rADCDAT1);
	tc_pos_y = (0x3ff&rADCDAT0);
	event=flag;
	num=acoral_sem_getnum(sem);
	if(num<=0)
		acoral_sem_post(sem);
   
    //放手不算一个中断，只有在已经进入中断的情况下，才判断是什么状态
       
    //当测量完了后，我们接着配置成等待中断模式 
    //下面这两句是什么意思呢？就是说，如果这次是相应按下中断，那么下次就是响应弹起中断
	if(flag==ACORAL_TS_UP)
	{
		rADCTSC = (0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);  
	//[7] YM=GND, [6] YP is connected with AIN[5], [5] XM=Hi-Z, [4] XP is connected with AIN[7]
      //[3] XP pull-up enable, [2] Normal ADC conversion, [1:0] Waiting for interrupt mode   
		flag =ACORAL_TS_DOWN;
	}
	else
	{
		rADCTSC = (1<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3); 
		flag =ACORAL_TS_UP;
	}
      
    //相应的位为1表示已经请求，清0表示没有请求，我们就是要它没有请求，这样就能接受新的请求了  
    //怎么感觉这里错了啊，应该是清源啊，这里是或的话，就讲相应的位置1了啊，那不是接着又有请求了吗。
    //有大大的问题，调试看看.           
	rSUBSRCPND |= BIT_SUB_TC;
	// rSUBSRCPND &=~(BIT_SUB_TC);
	//子屏蔽寄存器，置1表示屏蔽，清0表示接受中断，下面的语句表示打开触摸屏中断
	rINTSUBMSK  &=~(BIT_SUB_TC);   //Unmask sub interrupt (TC)   
	//感觉这个函数也不是清除中断源，感觉刚好相反  
	rSRCPND = BIT_ADC;
	rINTPND = BIT_ADC;
}

acoral_32 ts_open(){
	sem=acoral_sem_create(0);
	touchscreen_init();
	return 0;
}

acoral_32 ts_read(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	acoral_ts_info *ts_info;
	ts_info=(acoral_ts_info *)data;
	acoral_sem_pend(sem,0);
	ts_info->x=tc_pos_x;
	ts_info->y=tc_pos_y;	
	ts_info->event=event;
	return 0;
}
	
acoral_32 ts_close(){
	ts_off();
	return 0;
}

void ts_init(){
	ts_ops.write=NULL;
	ts_ops.open=ts_open;
	ts_ops.read=ts_read;
	ts_ops.write=NULL;
	ts_ops.close=ts_close;
	ts_ops.config=NULL;
  	acoral_drv_register(&ts_ops,"ts");
}
