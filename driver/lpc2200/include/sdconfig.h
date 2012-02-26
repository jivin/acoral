#ifndef __SDCONFIG_H__
#define __SDCONFIG_H__
#include <type.h>



#define SD_CRC_EN		   		1			/* 设置数据传输时是否使用CRC */

#define SPI_CLOCK				5529600		/* 正常通信时,SPI时钟频率 frequency (Hz) */

#define SD_BLOCKSIZE 			512			/* SD卡块的长度 */

#define SD_BLOCKSIZE_NBITS		9  

/* 下面函数不常用,如果用户不需要,可置为 0 裁剪指定函数 */

#define SD_ProgramCSD_EN   		0			/* 是否使能写CSD寄存器函数 */
	
#define SD_ReadCID_EN	   		0			/* 是否使能读CID寄存器函数 */
	
#define	SD_ReadSD_Status_EN		0			/* 是否使能读SD_Status寄存器函数 */

#define	SD_ReadSCR_EN			0			/* 是否使能读SCR寄存器函数 */

/* LPC22xx_EN 硬件配置 */
 
/* SCK引脚 */
#define  SPI_SCK				(0x01 << 4)						
#define  SPI_SCK_GPIO()			PINSEL0 &= ~(0x03 << 8)			/* 设置 SCK 口为GPIO口 */
#define  SPI_SCK_OUT()			IO0DIR |= SPI_SCK				/* 设置 SCK 口为输出口 */
#define	 SPI_SCK_CLR()			IO0CLR = SPI_SCK				/* 置 SCK 为低电平 */	

/* MISO 引脚 */	
#define  SPI_MISO				(0x01 << 5)						
#define  SPI_MISO_GPIO()		PINSEL0 &= ~(0x03 << 10)		/* 设置 MISO 口为GPIO口 */
#define  SPI_MISO_OUT()			IO0DIR |= SPI_MISO				/* 设置 MISO 口为输出口 */
#define	 SPI_MISO_CLR()			IO0CLR = SPI_MISO				/* 置 MISO 为低电平 */

/* MOSI 引脚 */	
#define  SPI_MOSI				(0x01 << 6)
#define  SPI_MOSI_GPIO()		PINSEL0 &= ~(0x03 << 12)		/* 设置 MOSI 口为GPIO口 */
#define  SPI_MOSI_OUT()			IO0DIR |= SPI_MOSI				/* 设置 MOSI 口为输出口 */
#define	 SPI_MOSI_CLR()			IO0CLR = SPI_MOSI				/* 置 MISO 为低电平 */

/* CS 引脚 */		
#define  SPI_CS      			(0x01 << 18)              	 	
#define  SPI_CS_GPIO()			PINSEL1 &= ~(0x03 << 4)		 	/* 设置 CS 口为GPIO口 */
#define  SPI_CS_OUT()			IO0DIR |= SPI_CS;			 	/* 设置 CS 口为输出口 */
#define	 SPI_CS_SET()			IO0SET |= SPI_CS;				/* 置 CS 为高电平 */
#define	 SPI_CS_CLR()			IO0CLR |= SPI_CS;				/* 置 CS 为低电平 */

/* 初始化 IO 口为SPI接口 */
#define  SPI_INIT()				PINSEL0 &= ~((0x03 << 8) + (0x03 << 10) + (0x03 << 12) + (0x03 << 14)); \
								PINSEL0 |= (0x01 << 8) + (0x01 << 10) + (0x01 << 12) + (0x01 << 14);
																
/* 电源控制引脚 */
#define  SD_POWER				(0x01 << 17)	
#define  SD_POWER_GPIO()		PINSEL1 &= ~(0x03 << 2)			/* 设置 POWER 口为GPIO口 */
#define  SD_POWER_OUT()			IO0DIR |= SD_POWER				/* 设置 POWER 口为输出口 */
#define  SD_POWER_OFF()			IO0SET = SD_POWER				/* 置 POWER 为高电平 */
#define  SD_POWER_ON()			IO0CLR = SD_POWER				/* 置 POWER 为低电平 */

/* 卡完全插入卡座检测引脚 */
#define  SD_INSERT				(0x01 << 19)		
#define  SD_INSERT_GPIO()		PINSEL1 &= ~(0x03 << 6)			/* 设置 INSERT 口为GPIO口 */	
#define  SD_INSERT_IN()			IO0DIR &= ~SD_INSERT			/* 设置 INSERT 口为输出口 */	
#define  SD_INSERT_STATUS()  	(IO0PIN & SD_INSERT)			/* 读取 INSERT 口的状态 */

/* 卡写保护检测引脚 */
#define  SD_WP					(0x01 << 29)		
#define  SD_WP_GPIO()			PINSEL1 &= ~(0x03 << 26)		/* 设置 WP 口为GPIO口 */	
#define  SD_WP_IN()				IO0DIR &= ~SD_WP				/* 设置 WP 口为输出口 */	
#define  SD_WP_STATUS()  		(IO0PIN & SD_WP)				/* 读取 WP 口的状态 */

#endif
