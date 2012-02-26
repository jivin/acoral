#include"acoral.h"
#include<console.h>
#define put_char(ch) uart_write(ch)
#define get_char(ch) uart_read(ch)
acoral_dev_ops_t console_ops;
void uart_write(acoral_u8 ch){
	while((HAL_REG(UART0_BASE,UART_PL01x_FR) & 0x20));
		HAL_REG(UART0_BASE,UART_PL01x_DR)=ch;

}

void uart_read(acoral_char *ch){
	while((HAL_REG(UART0_BASE,UART_PL01x_FR) & 0x10));
		*ch=HAL_REG(UART0_BASE,UART_PL01x_DR)&0xFF;
}

acoral_32 console_write(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
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
			break;
		default:
			break;
	}		
}


void uart_init(){
	acoral_u32 tmp,divider,remainder,fraction;	
	tmp      = 16 * CFG_BAUD_RATE;
	divider   = UART_CLOCK / tmp;
	remainder = UART_CLOCK % tmp;
	tmp      = (8 * remainder) / CFG_BAUD_RATE;
	fraction  = (tmp >> 1) + (tmp & 1);

	HAL_REG(UART0_BASE,UART_PL011_CR)=0;
	HAL_REG(UART0_BASE,UART_PL011_IBRD)=divider;
	HAL_REG(UART0_BASE,UART_PL011_FBRD)=fraction;
	HAL_REG(UART0_BASE,UART_PL011_LCRH)=0x70;
	HAL_REG(UART0_BASE,UART_PL011_CR)=0x301;
}

void console_init(){
	console_ops.write=console_write;
	console_ops.read=console_read;
	console_ops.config=console_config;
  	acoral_drv_register(&console_ops,"console");
	uart_init();
}
