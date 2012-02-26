#include"acoral.h"
struct gic_chip_data  gic_data[2];
void intr_sic_entry();
void hal_all_entry(acoral_vector vector){
	if(vector==42)
		intr_sic_entry();
	else
		acoral_intr_entry(vector);
}

/*===========================                                                                                                                 
*The TC11MP_PB_IRQ1(32+10) intrrupt entry point     
*42号中断入口函数，读取外部确认寄存器的真正中断号
*===========================*/
void intr_sic_entry(){
     acoral_u32 gic_irq;
     acoral_u32 gic_nr;

     /*应答该中断*/
     HAL_INTR_ACK(IRQ_TC11MP_PB_IRQ1);

     /*从外部的中断确认寄存器取值，并取低十位，得到中断号*/
     gic_irq=HAL_REG(REALVIEW_PB11MP_GIC_CPU_BASE,GIC_CPU_INTACK);
     gic_irq =(gic_irq & 0x3ff);

     /*中断号加上中断偏移，得到真正的代码中的中断号*/
     gic_nr=gic_irq+gic_data[1].irq_offset;
     if(gic_irq<32||gic_irq>1020||gic_nr>=NR_IRQS)
        return;

     /*重新进入公共入口函数*/
	 acoral_intr_entry(gic_nr);
}


/*===========================                                                                                                                 
*Get the base address of the interrupt distributor       
*获取分发器基地址
*===========================*/
acoral_u32 hal_gic_dist_base(acoral_vector irq){
    
     /*分为两部分，分别获取不同的基地址*/
     if(irq<64)
	return gic_data[0].dist_base;
     return gic_data[1].dist_base;        
}


/*===========================                                                                                                                 
*Get the base address of the cpu interface      
*获取CPU接口基地址
*===========================*/
acoral_u32 hal_gic_cpu_base(acoral_vector irq){
    
     /*不同的部分返回不同的基地址*/
     if(irq<64)
	return gic_data[0].cpu_base;
     return gic_data[1].cpu_base;        
}


/*===========================                                                                                                                 
*Change the irq to operate the registers      
*转化中断号，使他能够对应寄存器
*===========================*/
acoral_u32 gic_irq(acoral_vector irq){
       
        /*两部分中断号有不同的处理  */
        if(irq<64)
		return irq; 
        return (irq-gic_data[1].irq_offset);
}



/*===========================                                                                                                                 
*Get the priority of intrrupt    
*获取中断的优先级
*===========================*/
acoral_u32 hal_get_intr_prio(acoral_vector irq){
     acoral_u32 mask;
     acoral_u32 prio;

     mask=HAL_REG(hal_gic_dist_base(irq),GIC_DIST_PRI+gic_irq(irq)/4);
     prio=mask&(15<<(8*irq%4+4));
     return prio;
}


/*===========================                                                                                                                 
*Set the priority of intrrupt    
*设置中断的优先级
*===========================*/
void hal_set_intr_prio(acoral_vector irq,acoral_u32 prio){
     acoral_u32 mask;
     if(prio<0||prio>15)
       return ;
     mask =(prio<<(8*irq%4+4));
     HAL_REG(hal_gic_dist_base(irq),GIC_DIST_PRI+gic_irq(irq)/4)=mask;
}

/*===========================                                                                                                                 
*Set the boundary      
*设置实时中断与非实时中断的界限值
*===========================*/
void hal_set_intr_prio_bound(acoral_u32 prio){
     if(prio<0||prio>15)
          return ;

       PRIMASK=prio;
 
/*     prio=prio<<3;*/
/*    HAL_REG(REALVIEW_TC11MP_GIC_CPU_BASE,GIC_CPU_PRIMASK)=prio;*/
/*   HAL_REG(REALVIEW_PB11MP_GIC_CPU_BASE,GIC_CPU_PRIMASK)=prio;*/
}


/*===========================                                                                                                                 
*Set the intrrupt to be real      
*设置中断为实时中断,默认优先级0
*===========================*/
void hal_set_intr_real(acoral_vector irq){
     acoral_u32  mask;
     /*先读取中断对应的优先级寄存器的值，然后将相应为置1，再写回寄存器*/
     mask =0<<(irq%4*8);
     HAL_REG(hal_gic_dist_base(irq),GIC_DIST_PRI+gic_irq(irq)/4)=mask;
}


/*===========================                                                                                                                 
*Set the intrrupt to be common      
*设置中断为普通中断，默认优先级为界限值
*===========================*/
void hal_set_intr_comm(acoral_vector irq){
     acoral_u32  mask;

/*     prio=HAL_REG(hal_gic_dist_base(irq),GIC_CPU_PRIMASK);*/
   
     /*先读取中断对应的优先级寄存器的值，然后将相应为置1，再写回寄存器*/
     mask =PRIMASK<<(irq%4*8);
     HAL_REG(hal_gic_dist_base(irq),GIC_DIST_PRI+gic_irq(irq)/4)|=mask;
}


/*===========================                                                                                                                 
*Change the priority of intrrupt by offset      
*通过偏移量改变中断的优先级
*===========================*/
void hal_set_intr_offset(acoral_vector irq,acoral_u32 offset){
     acoral_u32  prio;

     prio=hal_get_intr_prio(irq);
     if(prio+offset>15)
         return ;

     /*普通中断，改变优先级*/
     if(PRIMASK<=prio){
         hal_set_intr_prio(irq,PRIMASK+offset);
      }else{
     /*实时中断改变优先级，如果当前优先级加上偏移后大于限值，则返回*/
	if(PRIMASK>offset)
		hal_set_intr_prio(irq,offset);
	else
		hal_set_intr_prio(irq,PRIMASK-1);/*超过了实时中断的最大值，设置为实时中断的*/
      }
      return ;
}


/*===========================                                                                                                                 
*Enable the interrupt      
*使能中断，参数为要操作的中断
*===========================*/
void hal_intr_unmask(acoral_vector irq){
        acoral_u32  mask;
        mask =1<<(IRQ_TC11MP_PB_IRQ1%32);
        HAL_REG(hal_gic_dist_base(IRQ_TC11MP_PB_IRQ1),GIC_DIST_ENABLE_SET+gic_irq(IRQ_TC11MP_PB_IRQ1)/32*4)=mask;
        mask =1<<(irq%32);
        HAL_REG(hal_gic_dist_base(irq),GIC_DIST_ENABLE_SET+gic_irq(irq)/32*4)=mask;
}

/*===========================                                                                                                                 
*Disable the interrupt
*除能中断，参数为要操作的中断
*===========================*/
void hal_intr_mask(acoral_vector irq){
	acoral_u32 mask;
        mask=1<<(irq%32);
        HAL_REG(hal_gic_dist_base(irq),GIC_DIST_ENABLE_CLEAR+gic_irq(irq)/32*4)=mask;
}


/*===========================                                                                                                                 
*Ack the interrupt      
*应答中断
*===========================*/
void hal_intr_ack(acoral_u32 irq){
	acoral_u32  mask;

        /*将挂起清除寄存器的相应位置1*/
       /* mask=1<<(irq%32);
      	HAL_REG(hal_gic_dist_base(irq),GIC_DIST_PENDING_CLEAR+gic_irq(irq)/32*4)=mask;
*/
        /*向中断结束寄存器写入中断号*/
        HAL_REG(hal_gic_cpu_base(irq),GIC_CPU_EOI)=gic_irq(irq);
}

/*===========================                                                                                                                 
*Initialize the interrupt      
*中断初始化函数
*===========================*/
void hal_intr_init(){
	acoral_u32 i;
        acoral_u32 pldctrl;

	/*设置非DCC模式,此模式将次GIC外部中断关联到内部42号中断*/
	HAL_REG(REALVIEW_SYS_LOCK,0)=0x0000a05f;
	pldctrl = HAL_REG(REALVIEW_SYS_BASE, REALVIEW_PB11MP_SYS_PLD_CTRL1);
	pldctrl |= 2 << 22;
	HAL_REG(REALVIEW_SYS_BASE , REALVIEW_PB11MP_SYS_PLD_CTRL1)=pldctrl;
	HAL_REG(REALVIEW_SYS_LOCK,0)=0;

	/*初始化测试芯片GIC, primary */
	gic_dist_init(0,REALVIEW_TC11MP_GIC_DIST_BASE,29);
	gic_cpu_init(0,REALVIEW_TC11MP_GIC_CPU_BASE);

	/*初始化板级GIC, secondary */
	gic_dist_init(1,REALVIEW_PB11MP_GIC_DIST_BASE,IRQ_PB11MP_GIC_START);
	gic_cpu_init(1,REALVIEW_PB11MP_GIC_CPU_BASE);

        /*设置各中断的私有函数：比如应答，屏蔽等*/
	for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		acoral_set_intr_enter(i,NULL);
		acoral_set_intr_exit(i,hal_intr_ack);
		acoral_set_intr_mask(i,hal_intr_mask);
		acoral_set_intr_unmask(i,hal_intr_unmask);

	}
}	

/*===========================                                                                                                                 
*Initialize the distributor      
*分发器初始化
*===========================*/
void gic_dist_init(acoral_u32 gic_nr,acoral_u32 base,acoral_u32 irq_start){
        
	acoral_u32 i;
	acoral_u32 max_irq;
       if(gic_nr>2)
           return;
        
        /*获取当前CPU掩码*/
        acoral_u32 cpumask = 1 <<(acoral_current_cpu);

        /*匹配目标CPU寄存器的格式*/
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

        /*设置dist基地址与中断偏移*/
        gic_data[gic_nr].dist_base=base;
        gic_data[gic_nr].irq_offset=(irq_start-1) &~31;

        /*设置中断分发控制寄存器值为0，不发送中断到CPU接口*/
        HAL_REG(base,GIC_DIST_CTRL)=0;

	max_irq = HAL_REG(base , GIC_DIST_CTR)&0x1f;
	max_irq = (max_irq + 1) * 32;

	/*修正最大值*/
	if (max_irq > 1020)
		max_irq = 1020;

        /*设置所有外部中断的触发模式*/
        for (i = 32; i < max_irq; i += 16)
		HAL_REG(base , GIC_DIST_CONFIG + i * 4 / 16)=0;

        /*设置所有外部中断的目标CPU*/
	for (i = 32; i < max_irq; i += 4)
		HAL_REG(base , GIC_DIST_TARGET + i * 4 / 4)=cpumask;

	
	 /*设置所有中断优先级*/
	for (i = 0; i < max_irq; i += 4)
		HAL_REG(base , GIC_DIST_PRI + i * 4 / 4)=0xa0a0a0a0;

	/*屏蔽所有中断*/
	for (i = 0; i < max_irq; i += 32)
		HAL_REG(base ,GIC_DIST_ENABLE_CLEAR + i * 4 / 32)=0xffffffff;

        /*设置中断分发控制寄存器值为1，允许发送中断到CPU接口*/
	HAL_REG(base , GIC_DIST_CTRL)=1;

}


/*===========================                                                                                                                 
*Initialize the cpu interface      
*初始化CPU接口
*===========================*/
void gic_cpu_init(acoral_u32 gic_nr,acoral_u32 base){
       
       if(gic_nr>2)
           return;

       /*设置cpu基地址*/
       gic_data[gic_nr].cpu_base=base;

       /*设置优先级寄存器的值*/
       HAL_REG(base,GIC_CPU_PRIMASK)=0xf0;
       
       /*设置CPU接口控制寄存器值为1，允许发送中断到目标CPU*/
       HAL_REG(base,GIC_CPU_CTRL)=1; 
}
