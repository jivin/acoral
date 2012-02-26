#include<bsp.h>
#include<driver.h>
#include<console.h>
void put_char(p){
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, (u8)p);
}

void get_char(acoral_char *ch){
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	*ch=USART_ReceiveData(USART1);
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
			break;
		default:
			break;
	}		
}

acoral_dev_ops_t console_ops;
void console_init(){
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	/* USARTx configured as follow:
		- BaudRate = 38400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
      	USART_InitStructure.USART_BaudRate = CFG_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* Configure USARTx_Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USARTx_Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_Init(USART1,&USART_InitStructure); 
	USART_Cmd(USART1, ENABLE);   

	console_ops.write=console_write;
	console_ops.read=console_read;
	console_ops.config=console_config;
	console_ops.open=NULL;
  	acoral_drv_register(&console_ops,"console");
}
