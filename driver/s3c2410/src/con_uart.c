#include<driver.h>
#include<console.h>
#include"../include/reg.h"
#define PCLK 30000000
#define put_char(ch) uart_write(ch)
#define get_char(ch) uart_read(ch)
acoral_dev_ops_t console_ops;
void uart_init();

void uart_write(acoral_u8 ch){
	while(!(rUTRSTAT0 & 0x4));
	WrUTXH0(ch);
}

void uart_read(acoral_char *ch){
	while(!(rUTRSTAT0 & 0x1));
	*ch=RdURXH0();
}

acoral_32 console_write(void *data,acoral_u32 size,acoral_time tm_out){
	acoral_u32 i;
  	char p;
	for(i=0;i<size;i++){
		p=*((acoral_char *)data+i);
		if(p=='\n')
			put_char('\r');
		put_char(p);
	}
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

acoral_32 console_config(acoral_u32 cmd,void *data,acoral_time time_out){
	switch(cmd){
		case CONSOLE_ECHO:
			*(acoral_u8 *)data=1;
		default:
			break;
	}		
}

acoral_32 console_open(){
	uart_init();
	return 0;
}

void uart_init(){
	rUCON0=0x245;	
	rULCON0=0x3;	
	rUFCON0=0x0;	
	rUMCON0=0x0;	
	rUBRDIV0=(PCLK /(CFG_BAUD_RATE * 16))-1;
    rGPHCON = 0x0016faaa;
	rGPHUP  = 0x7ff;    /* The pull up function is disabled GPH[10:0]*/
}

void con_uart_init(){
	console_ops.open=console_open;
	console_ops.write=console_write;
	console_ops.read=console_read;
	console_ops.config=console_config;
  	acoral_drv_register(&console_ops,"console");
}
