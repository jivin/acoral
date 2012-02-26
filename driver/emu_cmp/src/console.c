#include"acoral.h"
#include<stdio.h>
#include<console.h>
#define put_char(ch) putchar(ch)
#define get_char(ch) ((*ch)=getchar())
acoral_32 console_write(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
	acoral_u32 i;
  	acoral_char p;
	for(i=0;i<size;i++){
		p=*((acoral_char *)data+i);
		if(p=='\n')
			put_char('\r');
		put_char(p);
	}
	return size;
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
			*(acoral_u8 *)data=0;
			break;
		default:
			break;
	}		
}

acoral_dev_ops_t console_ops={
  	.write=console_write,
	.read=console_read,
	.config=console_config,
};

void console_init(){
  	acoral_drv_register(&console_ops,"console");
}
