#ifndef HAL_REG_H
#define HAL_REG_H
/* EXTERNAL MEMORY CONTROLLER (EMC) */
/* 外部总线控制器 */
#define BCFG0           (*((volatile unsigned int *) 0xFFE00000))       /* lpc22xx only */
#define BCFG1           (*((volatile unsigned int *) 0xFFE00004))       /* lpc22xx only */
#define BCFG2           (*((volatile unsigned int *) 0xFFE00008))       /* lpc22xx only */
#define BCFG3           (*((volatile unsigned int *) 0xFFE0000C))       /* lpc22xx only */

/* External Interrupts */
/* 外部中断控制寄存器 */
#define EXTINT          (*((volatile unsigned char *) 0xE01FC140))
#define EXTWAKE         (*((volatile unsigned char *) 0xE01FC144))
#define EXTMODE         (*((volatile unsigned char *) 0xE01FC148))      /* Not used for lpc210x*/
#define EXTPOLAR        (*((volatile unsigned char *) 0xE01FC14C))      /* Not used for lpc210x*/

/* SMemory mapping control. */
/* 内存remap控制寄存器 */
#define MEMMAP          (*((volatile unsigned char *) 0xE01FC040))

/* Phase Locked Loop (PLL) */
/* PLL控制寄存器 */
#define PLLCON          (*((volatile unsigned char *) 0xE01FC080))
#define PLLCFG          (*((volatile unsigned char *) 0xE01FC084))
#define PLLSTAT         (*((volatile unsigned short*) 0xE01FC088))
#define PLLFEED         (*((volatile unsigned char *) 0xE01FC08C))

/* Power Control */
/* 功率控制寄存器 */
#define PCON            (*((volatile unsigned char *) 0xE01FC0C0))
#define PCONP           (*((volatile unsigned long *) 0xE01FC0C4))

/* VPB Divider */
/* VLSI外设总线（VPB）分频寄存器 */
#define VPBDIV          (*((volatile unsigned char *) 0xE01FC100))

/* Memory Accelerator Module (MAM) */
/* 存储器加速模块 */
#define MAMCR           (*((volatile unsigned char *) 0xE01FC000))
#define MAMTIM          (*((volatile unsigned char *) 0xE01FC004))

/* Vectored Interrupt Controller (VIC) */
/* 向量中断控制器(VIC)的特殊寄存器 */
#define VICIRQStatus    (*((volatile unsigned long *) 0xFFFFF000))
#define VICFIQStatus    (*((volatile unsigned long *) 0xFFFFF004))
#define VICRawIntr      (*((volatile unsigned long *) 0xFFFFF008))
#define VICIntSelect    (*((volatile unsigned long *) 0xFFFFF00C))
#define VICIntEnable    (*((volatile unsigned long *) 0xFFFFF010))
#define VICIntEnClr     (*((volatile unsigned long *) 0xFFFFF014))
#define VICSoftInt      (*((volatile unsigned long *) 0xFFFFF018))
#define VICSoftIntClear (*((volatile unsigned long *) 0xFFFFF01C))
#define VICProtection   (*((volatile unsigned long *) 0xFFFFF020))
#define VICVectAddr     (*((volatile unsigned long *) 0xFFFFF030))
#define VICDefVectAddr  (*((volatile unsigned long *) 0xFFFFF034))
#define VICVectAddr0    (*((volatile unsigned long *) 0xFFFFF100))
#define VICVectAddr1    (*((volatile unsigned long *) 0xFFFFF104))
#define VICVectAddr2    (*((volatile unsigned long *) 0xFFFFF108))
#define VICVectAddr3    (*((volatile unsigned long *) 0xFFFFF10C))
#define VICVectAddr4    (*((volatile unsigned long *) 0xFFFFF110))
#define VICVectAddr5    (*((volatile unsigned long *) 0xFFFFF114))
#define VICVectAddr6    (*((volatile unsigned long *) 0xFFFFF118))
#define VICVectAddr7    (*((volatile unsigned long *) 0xFFFFF11C))
#define VICVectAddr8    (*((volatile unsigned long *) 0xFFFFF120))
#define VICVectAddr9    (*((volatile unsigned long *) 0xFFFFF124))
#define VICVectAddr10   (*((volatile unsigned long *) 0xFFFFF128))
#define VICVectAddr11   (*((volatile unsigned long *) 0xFFFFF12C))
#define VICVectAddr12   (*((volatile unsigned long *) 0xFFFFF130))
#define VICVectAddr13   (*((volatile unsigned long *) 0xFFFFF134))
#define VICVectAddr14   (*((volatile unsigned long *) 0xFFFFF138))
#define VICVectAddr15   (*((volatile unsigned long *) 0xFFFFF13C))
#define VICVectCntl0    (*((volatile unsigned long *) 0xFFFFF200))
#define VICVectCntl1    (*((volatile unsigned long *) 0xFFFFF204))
#define VICVectCntl2    (*((volatile unsigned long *) 0xFFFFF208))
#define VICVectCntl3    (*((volatile unsigned long *) 0xFFFFF20C))
#define VICVectCntl4    (*((volatile unsigned long *) 0xFFFFF210))
#define VICVectCntl5    (*((volatile unsigned long *) 0xFFFFF214))
#define VICVectCntl6    (*((volatile unsigned long *) 0xFFFFF218))
#define VICVectCntl7    (*((volatile unsigned long *) 0xFFFFF21C))
#define VICVectCntl8    (*((volatile unsigned long *) 0xFFFFF220))
#define VICVectCntl9    (*((volatile unsigned long *) 0xFFFFF224))
#define VICVectCntl10   (*((volatile unsigned long *) 0xFFFFF228))
#define VICVectCntl11   (*((volatile unsigned long *) 0xFFFFF22C))
#define VICVectCntl12   (*((volatile unsigned long *) 0xFFFFF230))
#define VICVectCntl13   (*((volatile unsigned long *) 0xFFFFF234))
#define VICVectCntl14   (*((volatile unsigned long *) 0xFFFFF238))
#define VICVectCntl15   (*((volatile unsigned long *) 0xFFFFF23C))

/* Timer 0 */
/* 定时器0的特殊寄存器 */
#define T0IR            (*((volatile unsigned long *) 0xE0004000))
#define T0TCR           (*((volatile unsigned long *) 0xE0004004))
#define T0TC            (*((volatile unsigned long *) 0xE0004008))
#define T0PR            (*((volatile unsigned long *) 0xE000400C))
#define T0PC            (*((volatile unsigned long *) 0xE0004010))
#define T0MCR           (*((volatile unsigned long *) 0xE0004014))
#define T0MR0           (*((volatile unsigned long *) 0xE0004018))
#define T0MR1           (*((volatile unsigned long *) 0xE000401C))
#define T0MR2           (*((volatile unsigned long *) 0xE0004020))
#define T0MR3           (*((volatile unsigned long *) 0xE0004024))
#define T0CCR           (*((volatile unsigned long *) 0xE0004028))
#define T0CR0           (*((volatile unsigned long *) 0xE000402C))
#define T0CR1           (*((volatile unsigned long *) 0xE0004030))
#define T0CR2           (*((volatile unsigned long *) 0xE0004034))
#define T0CR3           (*((volatile unsigned long *) 0xE0004038))
#define T0EMR           (*((volatile unsigned long *) 0xE000403C))

/* Timer 1 */
/* 定时器1的特殊寄存器 */
#define T1IR            (*((volatile unsigned long *) 0xE0008000))
#define T1TCR           (*((volatile unsigned long *) 0xE0008004))
#define T1TC            (*((volatile unsigned long *) 0xE0008008))
#define T1PR            (*((volatile unsigned long *) 0xE000800C))
#define T1PC            (*((volatile unsigned long *) 0xE0008010))
#define T1MCR           (*((volatile unsigned long *) 0xE0008014))
#define T1MR0           (*((volatile unsigned long *) 0xE0008018))
#define T1MR1           (*((volatile unsigned long *) 0xE000801C))
#define T1MR2           (*((volatile unsigned long *) 0xE0008020))
#define T1MR3           (*((volatile unsigned long *) 0xE0008024))
#define T1CCR           (*((volatile unsigned long *) 0xE0008028))
#define T1CR0           (*((volatile unsigned long *) 0xE000802C))
#define T1CR1           (*((volatile unsigned long *) 0xE0008030))
#define T1CR2           (*((volatile unsigned long *) 0xE0008034))
#define T1CR3           (*((volatile unsigned long *) 0xE0008038))
#define T1EMR           (*((volatile unsigned long *) 0xE000803C))

#define IO2SET			(*((volatile unsigned long *) 0xE0028024))
#define IO2DIR			(*((volatile unsigned long *) 0xE0028028))
#define IO2CLR			(*((volatile unsigned long *) 0xE002802C))

#define Fosc            11059200    /*Crystal frequence,10MHz~25MHz，should be the same as actual status. 
						    应当与实际一至晶振频率,10MHz~25MHz，应当与实际一至*/
#define Fcclk           (Fosc * 4)  /*System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz. 系统频率，必须为Fosc的整数倍(1~32)，且<=60MHZ*/
#define Fcco            (Fcclk * 4) /*CCO frequence,should be 2、4、8、16 multiples of Fcclk, ranged from 156MHz to 320MHz.   CCO频率，必须为Fcclk的2、4、8、16倍，范围为156MHz~320MHz*/
#define Fpclk           (Fcclk / 4) * 4   /*VPB clock frequence , must be 1、2、4 multiples of (Fcclk / 4).
						    PB时钟频率，只能为(Fcclk / 4)的1、2、4倍*/

#endif
