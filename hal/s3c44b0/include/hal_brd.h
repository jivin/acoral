#ifndef HAL_BOARD_H
#define HAL_BOARD_H
/*芯片配置变量*/
#define FCLK_60M
#ifdef FCLK_40M
#define FCLK 40000000
#define HCLK 40000000
#define PCLK 40000000
#define M_MDIV    0x48 
#define M_PDIV    0x3
#define M_SDIV    0x2
#endif
#ifdef FCLK_60M
#define FCLK 60000000
#define HCLK 60000000
#define PCLK 60000000
#define M_MDIV    0x70
#define M_PDIV    0x8
#define M_SDIV    0x1
#endif
#define vPLLCON		((M_MDIV << 12) | (M_PDIV << 4) | (M_SDIV)) 

/*bwscon寄存器定义*/
#define bo_bus_ublb_bank7   31          /*1=启用UB/LB*/
#define bo_bus_wait_bank7   30          /*1=wait enable*/
#define bm_bus_width_bank7  0x30000000  /*总线宽度，0=8，1=16，2=32，3=保留*/
#define bo_bus_width_bank7  28
#define bo_bus_ublb_bank6   27          /*1=启用UB/LB*/
#define bo_bus_wait_bank6   26          /*1=wait enable*/
#define bm_bus_width_bank6  0x03000000  /*总线宽度，0=8，1=16，2=32，3=保留*/
#define bo_bus_width_bank6  24
#define bo_bus_ublb_bank5   23          /*1=启用UB/LB*/
#define bo_bus_wait_bank5   22          /*1=wait enable*/
#define bm_bus_width_bank5  0x00300000  /*总线宽度，0=8，1=16，2=32，3=保留*/
#define bo_bus_width_bank5  20
#define bo_bus_ublb_bank4   19          /*1=启用UB/LB*/
#define bo_bus_wait_bank4   18          /*1=wait enable*/
#define bm_bus_width_bank4  0x00030000  /*总线宽度，0=8，1=16，2=32，3=保留*/
#define bo_bus_width_bank4  16
#define bo_bus_ublb_bank3   15          /*1=启用UB/LB*/
#define bo_bus_wait_bank3   14          /*1=wait enable*/
#define bm_bus_width_bank3  0x00003000  /*总线宽度，0=8，1=16，2=32，3=保留*/
#define bo_bus_width_bank3  12
#define bo_bus_ublb_bank2   11          /*1=启用UB/LB*/
#define bo_bus_wait_bank2   10          /*1=wait enable*/
#define bm_bus_width_bank2  0x00000300  /*总线宽度，0=8，1=16，2=32，3=保留*/
#define bo_bus_width_bank2  8
#define bo_bus_ublb_bank1   7           /*1=启用UB/LB*/
#define bo_bus_wait_bank1   6           /*1=wait enable*/
#define bm_bus_width_bank1  0x00000030  /*总线宽度，0=8，1=16，2=32，3=保留*/
#define bo_bus_width_bank1  4
#define bm_bus_width_bank0  0x00000006  /*总线宽度，0=8，1=16，2=32，3=保留*/

/*bankconx寄存器定义*/
#define bm_bankcon_tacs     0x6000      /*地址建立周期，0=0，1=1，2=2，3=4*/
#define bm_bankcon_tcos     0x1800      /*片选建立周期，0=0，1=1，2=2，3=4*/
#define bm_bankcon_tacc     0x0700      /*访问周期，0=1，1=2，2=3，3=4，4=6*/
                                        /*5=8，6=10，7=14，若wait允许，tacc>=4*/
#define bm_bankcon_tcoh     0x00c0      /*片选保持周期，0=0，1=1，2=2，3=4*/
#define bm_bankcon_tcah     0x0030      /*地址保持时间，0=0，1=1，2=2，3=4*/
#define bm_bankcon_tacp     0x000c      /*页模式访问周期，0=2，1=3，2=4，3=6*/
#define bm_bankcon_pmc      0x0003      /*页模式配置，0=normal，1=4bytes*/
                                        /*            2=8bytes，3=16bytes*/
#define bo_bankcon_tacs     13
#define bo_bankcon_tcos     11
#define bo_bankcon_tacc     8
#define bo_bankcon_tcoh     6
#define bo_bankcon_tcah     4
#define bo_bankcon_tacp     2
#define bo_bankcon_pmc      0

/*bank6/7位域定义*/
#define bm_b67_mt   0x18000     /*存储器类型，0=sram,1,2=保留,3=SDRAM*/
#define bm_b67_trcd 0x0000c     /*RAS to CAS clock,0=2,2=3,3=4*/
#define bm_b67_scan 0x00003     /*列地址线数,0=8,2=9,3=10*/
#define bo_b67_mt   15
#define bo_b67_trcd 2
#define bo_b67_scan 0

/*SDRAM 刷新控制*/
#define SDRAM_refresh_time  1051        /*2049-15.6(uS)*cn_mclk*/

#define bo_refresh_refen    23          /*1=使能刷新*/
#define bo_refresh_trefmd   22          /*刷新模式，0=CBR(CAS before RAS)/Auto*/
                                        /*refresh，1=self refresh*/
#define bm_refresh_trp      0x300000    /*RAS预充电周期，0=2，1=3,2=4，4=保留*/
#define bm_refresh_trc      0x0c0000    /*RC最短定时，0~3=4~7周期*/
#define bm_refresh_tchr     0x030000
#define bm_refresh_refcnt   0x0007ff    /*sdram刷新计数=2049-15.6(uS)*cn_hclk*/
#define bo_refresh_trp      20
#define bo_refresh_trc      18
#define bo_refresh_tchr     16
#define bo_refresh_refcnt   0

/*BANKSIZE寄存器位域*/
#define bo_banksize_bursten 7       /*1=允许突发*/
#define bo_banksize_sckeen  5       /*1=掉电模式使能*/
#define bo_banksize_sclken  4       /*1=仅在访问时输出时钟*/
#define bm_banksize_size    0x07    /*bank6/7内存尺寸，2=128/128M,1=64/64M*/
                                    /*0=32/32M,7=16/16M,6=8/8M,5=4/4M,4=2/2M*/
#define bo_banksize_size    0

/*MRSRB6/7寄存器设置*/
#define bm_mrsrb_cl         0x70
#define bo_mrsrb_cl         4
#define vBWSCON			(0<<bo_bus_ublb_bank7)|(0<<bo_bus_wait_bank7)|(1<<bo_bus_width_bank7)|(0<<bo_bus_ublb_bank6)|(0<<bo_bus_wait_bank6)|(1<<bo_bus_width_bank6)
#define vBANKCON0		(0<<bo_bankcon_tacs)\
                                  +(0<<bo_bankcon_tcos)\
                                  +(4<<bo_bankcon_tacc)\
                                  +(0<<bo_bankcon_tcoh)\
                                  +(0<<bo_bankcon_tcah)\
                                  +(0<<bo_bankcon_tacp)\
                                  +(0<<bo_bankcon_pmc)
#define vBANKCON1		0x00007ffc
#define vBANKCON2		0x00007ffc
#define vBANKCON3		0x00007ffc
#define vBANKCON4		0x00007ffc
#define vBANKCON5		0x00007ffc
#define vBANKCON6		(3<<bo_b67_mt)\
                                  +(0<<bo_b67_trcd)\
                                  +(0<<bo_b67_scan)

#define vBANKCON7	           (3<<bo_b67_mt)\
                                  +(0<<bo_b67_trcd)\
                                  +(0<<bo_b67_scan)
#define vREFRESH		(1<<bo_refresh_refen)\
                                 +(0<<bo_refresh_trefmd)\
                                 +(0<<bo_refresh_trp)\
                                 +(1<<bo_refresh_trc)\
                                 +(0<<bo_refresh_tchr)\
                                 +(SDRAM_refresh_time<<bo_refresh_refcnt)
#define vBANKSIZE		(0<<bo_banksize_bursten)\
                                  +(1<<bo_banksize_sckeen)\
                                  +(1<<bo_banksize_sclken)\
                                  +(6<<bo_banksize_size)

#define vMRSRB6			0x20
#define vMRSRB7			0x20


/* INTERRUPT*/
#define rI_INTCON   (*(volatile acoral_u32 *)0x01E00000)	/*Interrupt control Register*/
#define rINTPND     (*(volatile acoral_u32 *)0x01E00004)	/*Interrupt request status*/
#define rINTMOD     (*(volatile acoral_u32 *)0x01E00008)	/*Interrupt mode control*/
#define rINTMSK     (*(volatile acoral_u32 *)0x01E0000C)	/*Interrupt mask control*/
#define rI_PSLV     (*(volatile acoral_u32 *)0x01E00010)	/*IRQ priority of slave register*/
#define rI_PMST     (*(volatile acoral_u32 *)0x01E00014)	/*IRQ priority of master register*/
#define rI_CSLV     (*(volatile acoral_u32 *)0x01E00018)	/*Current IRQ priority of slave register*/
#define rI_CMST     (*(volatile acoral_u32 *)0x01E0001C)	/*Current IRQ priority of master register*/
#define rI_ISPR     (*(volatile acoral_u32 *)0x01E00020)	/*IRQ interrupt service pending register*/
#define rI_ISPC     (*(volatile acoral_u32 *)0x01E00024)	/*IRQ interrupt service clear register*/



/* PWM TIMER*/
#define rTCFG0  (*(volatile acoral_u32 *)0x01D50000)	/*Timer 0 configuration*/
#define rTCFG1  (*(volatile acoral_u32 *)0x01D50004)	/*Timer 0 configuration*/
#define rTCON   (*(volatile acoral_u32 *)0x01D50008)	/*Timer control*/
#define rTCNTB0 (*(volatile acoral_u32 *)0x01D5000C)	/*Timer count buffer 0*/
#define rTCMPB0 (*(volatile acoral_u32 *)0x01D50010)	/*Timer compare buffer 0*/
#define rTCNTO0 (*(volatile acoral_u32 *)0x01D50014)	/*Timer count observation 0*/
#define HAL_BOARD_INIT()
#endif
