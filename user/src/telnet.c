#include <acoral.h>
#include <console.h>
#include "lwip/api.h"
#include "lwip/inet.h"
#include "../include/shell.h"

/*
;*****************************************************************************************************
;* 函数名称 : telnet_thread
;* 描    述 : 远程登录线程
;* 输　	 入 : arg: 线程参数
;*        
;* 输　	 出 : 无
;*****************************************************************************************************
;*/
void server_client(void *args){
	acoral_id telnet_id;
	struct netconn *newconn=(struct netconn *)args;	
	if(newconn != NULL)
	{
		telnet_id=acoral_dev_open("con_telnet");
		if(telnet_id<0){
			acoral_prints("No shell Support,will Exit");
			netconn_delete(newconn);
			return;
		}
		acoral_dev_config(telnet_id,CONSOLE_SET_PRIV_DATA,newconn,0);
		acoral_posix_create(NULL,NULL,acoral_shell_enter,(void *)telnet_id);
	}
}

void telnet_thread(void *arg)
{
	struct netconn *conn, *newconn;
	struct ip_addr l_ip;
	IP4_ADDR(&l_ip, 192,168,1,2);
  	/* Create a new connection identifier. */
	conn = netconn_new(NETCONN_TCP);

	/* Bind connection to well known port number 7. */
	netconn_bind(conn, &l_ip, 23);

  	/* Tell connection to go into listening mode. */
	netconn_listen(conn);

	while(1)
	{
		newconn = netconn_accept(conn);
		server_client(newconn);
	}
}



void user_telnetd()
{
     acoral_create_thread(telnet_thread,1024,"telnet",NULL,TELNET_TASK_PRIO,-1);
}
