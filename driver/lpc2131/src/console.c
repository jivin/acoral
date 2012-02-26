#include"acoral.h"
#include"console.h"
#define UART_BPS 	115200
#define UART_DIV 	((PCLK/16)/UART_BPS)
#define vU0DLM 		(UART_DIV/256)
#define vU0DLL 		(UART_DIV%256)
#define 	WrUTXH0(ch) 	(rU0THR=ch)

#define put_char(ch) uart_write(ch)
acoral_dev_ops_t console_ops;
void uart_write(acoral_u8 ch){
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

acoral_32 console_config(acoral_u32 cmd,void *data,acoral_time time_out){
	switch(cmd){
		case CONSOLE_ECHO:
			*(acoral_u8 *)data=1;
	}		
}


void uart_init(){
}

void console_init(){
	console_ops.write=console_write;
	console_ops.read=NULL;
	console_ops.open=NULL;
	console_ops.config=console_config;
  	acoral_drv_register(&console_ops,"console");
	uart_init();
}
