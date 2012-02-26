#ifndef _ACORAL_LWIPOPTS_H__
#define _ACORAL_LWIPOPTS_H__

#include "arch/cc.h"
#include "arch/sys_arch.h"


extern acoral_u32  pbuf_release;  // tese for pbuf release


#define NO_SYS  0

/* 包含相关头文件 */
#include "acoral.h"
/* 提供标准错误代号 */
#define LWIP_PROVIDE_ERRNO
/* 定义小端模式*/
#define BYTE_ORDER LITTLE_ENDIAN
/* 最大线程数目 */
#define TCPIP_THREAD_MAXNUM 5


#define OS_LOWEST_PRIO    20

//数据接收线程
#define  ETH_RECV_TASK_PPIO            (OS_LOWEST_PRIO + 0)
//TCPIP线程
#define  TCPIP_THREAD_PRIO			   (OS_LOWEST_PRIO + 1)
//HTTPD网页服务器任务
#define  APP_TASK_HTTPD_PRIO		   (OS_LOWEST_PRIO + 2)
//远程登录任务
#define  TELNET_TASK_PRIO			   (OS_LOWEST_PRIO + 3)



//Task Stk lengh
#define TCPIP_THREAD_STACKSIZE  1024*4

#define TCP_WND                         4096
#define TCP_MSS                         1024*2
#define TCP_SND_BUF                     1024*4
#define TCP_SND_QUEUELEN                (4 * (TCP_SND_BUF/TCP_MSS))

#endif /* _ACORAL_LWIPOPTS_H__ */
