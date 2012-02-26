#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <os_file.h>
/*
;*****************************************************************************************************
;* 函数名称 : httpd_thread
;* 描    述 : HTTPD任务
;* 输　	 入 : pdata
;*        
;* 输　	 出 : 无
;*****************************************************************************************************
;*/
static acoral_char* WwwRoot = "/www";
#define HTTP_GET_STRING_LEN   1024
acoral_u8 g_http_get_string[HTTP_GET_STRING_LEN];

static acoral_u8 httpd_buf[HTTP_GET_STRING_LEN];	
static acoral_char str[HTTP_GET_STRING_LEN];
void httpd_thread(void *arg)
{
	acoral_char *cmd[2]={"get", "post"};
	acoral_32  s, ei, temp;
	acoral_32  len;
	struct sockaddr_in serv, cli;
	acoral_u32 fd;
	acoral_u32 rootlen, pos1, pos2;
	
	rootlen = acoral_str_len(WwwRoot);
	memset(&serv, 0, sizeof(struct sockaddr_in));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(80);	
	serv.sin_addr.s_addr = htons(INADDR_ANY);
	s = socket(AF_INET, SOCK_STREAM, 0);
	ei = bind(s, (struct sockaddr*)&serv, sizeof(struct sockaddr_in));
	ei = listen(s, 5);
	while (1)
	{
		//发送响应报文
		temp = accept(s, (struct sockaddr*)&cli, &len);
		if (temp != -1)
		{
			recv(temp, g_http_get_string, HTTP_GET_STRING_LEN-1, 0);
			g_http_get_string[HTTP_GET_STRING_LEN-1] = '\0';
			
			//分析http报头
			//非get命令
			if (acoral_str_nicmp(g_http_get_string, cmd[0], 3) != 0)
				continue;
			
			//没有空格分格,未知格式
			pos1 = acoral_str_chr(g_http_get_string, ' ');
			if (pos1 == 0)
				continue;
			
			pos2 = acoral_str_chr(g_http_get_string+pos1, ' ');
			if (pos2 == 0)
				continue;
				
			//pos1 与 pos2 之间为申请文件
			acoral_str_cpy(str, WwwRoot);
			//acoral_debugs(g_http_get_string);
			acoral_str_ncpy(&str[rootlen], &g_http_get_string[pos1], pos2-1);
			str[rootlen+pos2-1] = '\0';
			//acoral_debugs(str);
			if (str[rootlen+pos2-2] == '/')
				acoral_str_cat(str, "index.htm");
			if(str[rootlen]=='\0')
				acoral_str_cpy(&str[rootlen], "index.htm");
			fd=acoral_open(str,O_RDONLY); 	
			while((len=acoral_read(fd, httpd_buf,HTTP_GET_STRING_LEN))>0)
				send(temp,httpd_buf, len, 0);
			acoral_close(fd);
		}
		close(temp); 
	}
 }

void user_httpd()
{
	acoral_posix_create(NULL,NULL,httpd_thread,0);
}
