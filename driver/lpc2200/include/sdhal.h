#ifndef __SDHAL_H__
#define __SDHAL_H__
#include<type.h>
/* 检测卡是否完全插入  check weather card is insert entirely */
extern acoral_u8 SD_ChkCard(void);

/* 检测卡是否写保护  check weather card is write protect */
extern acoral_u8 SD_ChkCardWP(void);

/* 初始化访问SD卡的硬件初始化 initialize the hardware that access sd card */
extern void SD_HardWareInit(void);

/* 设置SPI的时钟小于400kHZ set the clock of SPI less than 400kHZ */
extern void SPI_Clk400k(void);

/* 设置SPI的clock到最大值 set the clock of SPI to maximum */
extern void SPI_ClkToMax(void);

/* 通过SPI接口发送一个字节 send a byte by SPI interface */
extern void SPI_SendByte(acoral_u8 byte);

/* 从SPI接口接收一个字节 receive a byte from SPI interface */
extern acoral_u8 SPI_RecByte(void);

/* 片选SPI从机 select the SPI slave */
extern void SPI_CS_Assert(void);

/* 不片选SPI从机 not select the SPI slave */
extern void SPI_CS_Deassert(void);

/* SPI总线接口的特殊寄存器 */
/* only for lpc210x*/
#define SPI_SPCR        (*((volatile unsigned char *) 0xE0020000))
#define SPI_SPSR        (*((volatile unsigned char *) 0xE0020004))
#define SPI_SPDR        (*((volatile unsigned char *) 0xE0020008))
#define SPI_SPCCR       (*((volatile unsigned char *) 0xE002000C))
#define SPI_SPINT       (*((volatile unsigned char *) 0xE002001C))

/* Pin Connect Block */
/* 管脚连接模块控制寄存器 */
#define PINSEL0         (*((volatile unsigned long *) 0xE002C000))
#define PINSEL1         (*((volatile unsigned long *) 0xE002C004))
#define PINSEL2         (*((volatile unsigned long *) 0xE002C014))      /* Not used for lpc210x*/

#define IO0PIN          (*((volatile unsigned long *) 0xE0028000))      /* Not used for lpc210x*/
#define IO0SET          (*((volatile unsigned long *) 0xE0028004))      /* Not used for lpc210x*/
#define IO0DIR          (*((volatile unsigned long *) 0xE0028008))      /* Not used for lpc210x*/
#define IO0CLR          (*((volatile unsigned long *) 0xE002800C))      /* Not used for lpc210x*/


#endif
