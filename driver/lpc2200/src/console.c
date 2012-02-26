#include"acoral.h"
#include<console.h>
#define PINSEL0         (*((volatile unsigned long *) 0xE002C000))
/* 通用异步串行口0(UART0)的特殊寄存器 */
#define U0RBR           (*((volatile unsigned char *) 0xE000C000))
#define U0THR           (*((volatile unsigned char *) 0xE000C000))
#define U0IER           (*((volatile unsigned char *) 0xE000C004))
#define U0IIR           (*((volatile unsigned char *) 0xE000C008))
#define U0FCR           (*((volatile unsigned char *) 0xE000C008))
#define U0LCR           (*((volatile unsigned char *) 0xE000C00C))
#define U0LSR           (*((volatile unsigned char *) 0xE000C014))
#define U0SCR           (*((volatile unsigned char *) 0xE000C01C))
#define U0DLL           (*((volatile unsigned char *) 0xE000C000))
#define U0DLM           (*((volatile unsigned char *) 0xE000C004))

#define Fosc            11059200    /*Crystal frequence,10MHz~25MHz，should be the same as actual status. 
						    应当与实际一至晶振频率,10MHz~25MHz，应当与实际一至*/
#define Fcclk           (Fosc * 4)  /*System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz. 系统频率，必须为Fosc的整数倍(1~32)，且<=60MHZ*/
#define Fcco            (Fcclk * 4) /*CCO frequence,should be 2、4、8、16 multiples of Fcclk, ranged from 156MHz to 320MHz.   CCO频率，必须为Fcclk的2、4、8、16倍，范围为156MHz~320MHz*/
#define Fpclk           (Fcclk / 4) * 4   /*VPB clock frequence , must be 1、2、4 multiples of (Fcclk / 4).
						    PB时钟频率，只能为(Fcclk / 4)的1、2、4倍*/

#define put_char(ch) uart_write(ch)
#define get_char(ch) uart_read(ch)
acoral_dev_ops_t console_ops;
void uart_write(acoral_u8 ch){
    U0THR = ch;						// 发送数据
    while( (U0LSR&0x40)==0 );			// 等待数据发送完毕
}

void uart_read(acoral_u8 *ch){
	while((U0LSR&0x01)==0);   //U0RBR为空的话, 一直等待
	*ch=U0RBR; 
}

acoral_32 console_read(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	acoral_u32 i;
  	acoral_char *p;
	for(i=0;i<size;i++){
		p=(acoral_char *)data+i;
		get_char(p);
	}
	return size;
}


acoral_32 console_write(void *data,acoral_size size,acoral_size offet,acoral_time tm_out){
	acoral_u32 i;
  	acoral_char p;
	for(i=0;i<size;i++){
		p=*((acoral_char *)data+i);
		if(p=='\n')
			put_char('\r');
		put_char(p);
	}
}

acoral_32 console_config(acoral_u32 cmd,void *data,acoral_time time_out){
	switch(cmd){
		case CONSOLE_ECHO:
			*(acoral_u8 *)data=1;
		default:
			break;
	}		
}

void uart_init(){	
    acoral_16 Fdiv;
    PINSEL0 = (PINSEL0 & (~0x0F)) | 0x05;	// 不影响其它管脚连接,设置I/O连接到UART0
    U0LCR = 0x83;							// DLAB = 1，可设置波特率
    Fdiv = (Fpclk / 16) / CFG_BAUD_RATE;				// 设置波特率
    U0DLM = Fdiv / 256;							
    U0DLL = Fdiv % 256;						
    U0LCR = 0x03;
}
void console_init(){
	console_ops.write=console_write;
	console_ops.read=console_read;
	console_ops.open=NULL;
	console_ops.config=console_config;
  	acoral_drv_register(&console_ops,"console");
	uart_init();
}
