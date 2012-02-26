#include<acoral.h>
#include<console.h>
#include "lwip/api.h"
#include "lwip/inet.h"
acoral_dev_ops_t con_telnet_ops;
struct netconn *my_conn;
static struct netbuf *con_buf;
acoral_16 len;
acoral_u8 *net_data;

acoral_32 con_telnet_write(void *data,acoral_u32 size,acoral_time tm_out){
	netconn_write(my_conn,data,size,NETCONN_COPY);
	return size;	
}

acoral_32 con_telnet_read(void *data,acoral_size size,acoral_size offset,acoral_time tm_out){
  	acoral_u8 *p;
	acoral_u32 i;
	if(con_buf==NULL){
		con_buf=netconn_recv(my_conn);
		if(con_buf==NULL)
			return 0;
		netbuf_data(con_buf, &net_data, &len);
	}
	for(i=0;i<size;i++){
		if(len-->0){
			p=(acoral_u8 *)data+i;
			*p=*net_data++;
		}else{
			if(netbuf_next(con_buf)>= 0){
				netbuf_data(con_buf, &net_data, &len);
			}
			else{
				netbuf_delete(con_buf);
				con_buf=NULL;
				return i;
			}
		}	
	}
	return i;
}

acoral_32 con_telnet_config(acoral_u32 cmd,void *data,acoral_time time_out){
	switch(cmd){
		case CONSOLE_ECHO:
			*(acoral_u8 *)data=0;
			break;
		case CONSOLE_SET_PRIV_DATA:
			my_conn=(struct netconn *)data;
			break;
		case CONSOLE_BUF_CLEAR:
			netbuf_delete(con_buf);
			con_buf=NULL;
			break;
		case CONSOLE_EXIT:
			netconn_delete(my_conn);
			break;
		default:
			break;
	}		
}

void con_telnet_init(){
	con_telnet_ops.write=con_telnet_write;
	con_telnet_ops.read=con_telnet_read;
	con_telnet_ops.config=con_telnet_config;
	con_telnet_ops.open=NULL;
	con_buf=NULL;
  	acoral_drv_register(&con_telnet_ops,"con_telnet");
}
