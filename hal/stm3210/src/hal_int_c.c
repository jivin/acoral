#include<type.h>
#include<hal_port.h>
#include"../include/hal_brd.h"
extern void HAL_INTR_DISABLE();
extern acoral_u32 HAL_INTR_TABLE[];
struct int_reg     /*at 0xe000e100*/
{
     acoral_u32 setena[32];/*使能寄存器组*/
     acoral_u32 clrena[32];/*除能寄存器组*/
     acoral_u32 setpend[32];/*挂起寄存器组*/
     acoral_u32 clrpend[32];/*解挂寄存器组*/
     acoral_u32 active[32]; /*活动状态寄存器组   */
     acoral_u32 RESERVED5[32];/*保留*/
     acoral_u8  pri[240];/*优先级寄存器组*/
};

void hal_intr_ack(acoral_vector vector);
struct int_reg volatile *const hal_int_reg = (struct int_reg *)0xe000e100;

void hal_intr_unmask(acoral_vector vector){
       if(vector>HAL_INTR_NUM-1)
          return ;
       hal_int_reg->setena[((vector-16)>>0x05)]|=(acoral_u32)0x01<<((vector-16) & (acoral_u32)0x1F);/*使能中断*/
}

void hal_intr_mask(acoral_vector vector){
	if(vector>HAL_INTR_NUM-1)
             return;			/* 屏蔽中断*/
       hal_int_reg->clrena[(vector-16)>>0x05]|=(acoral_u32)0x01<<((vector-16) & (acoral_u32)0x1F);
}

void hal_intr_ack(acoral_vector vector){
	if(vector>HAL_INTR_NUM-1)
		return ;
         hal_int_reg->clrpend[(vector-16)>>0x05]=(acoral_u32)0x01<<((vector-16) & (acoral_u32)0x1F); 
}

void hal_intr_init(){
  	acoral_u32 i,reg_num;
	acoral_vector index;
        /*关中断*/
        HAL_INTR_DISABLE();

        /*设置优先级寄存器组*/
        *((acoral_u32 *)0xe000ed0c)=0x05fa0500;

         /*重定向向量表*/
        *((acoral_u32 *)0xe000ed08)=(acoral_u32)HAL_INTR_TABLE;
        *((acoral_u8 *)0xe000ed22)=0xff;

        /*设置各中断优先级寄存器*/
        for(i=HAL_INTR_MIN+1;i<HAL_INTR_MAX;i++){
               hal_int_reg->pri[i-16]=0x00;
        }

        /*全部禁止中断并清除挂起状态*/
        reg_num=(HAL_INTR_NUM-2)>>5+1; /**/
        for(i=1;i<reg_num;i++){
               hal_int_reg->clrena[i-1]=0xffffffff;
               hal_int_reg->clrpend[i-1]=0xffffffff;
        }
        
        /*操作函数*/
        for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		acoral_set_intr_enter(i,hal_intr_ack);
		acoral_set_intr_exit(i,NULL);
		acoral_set_intr_mask(i,hal_intr_mask);
		acoral_set_intr_unmask(i,hal_intr_unmask);
	}     
}


