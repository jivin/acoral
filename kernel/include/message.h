/*--------------------*/
/*  增加函数申明，将定时调用改为timeout */
/*  pegasus  0719*/
/*-------------------*/

#ifndef ACORAL_MESSAGE_H
#define ACORAL_MESSAGE_H
#include<type.h>
#include<queue.h>
#include<list.h>
#include<resource.h>
#include<event.h>
#include<thread.h>

#define ACORAL_MESSAGE_MAX_COUNT  10

#define MST_DEL_FORCE     1 
#define MST_DEL_UNFORCE   0

/*判断信息检测执行过程是否正在执行*/
#define DO_CHECKED_TRUE           0x0
#define DO_CHECKED_FALSE          0x1

/*-----------------*/
/*  改为延时链g_timeout_queue来处理*/
/*  pegasus  0719*/
/*-----------------*/
/*多少ticks检测一次msg的生命周期*/
#define WAKE_UP_PER               30

/*MESSAGE类型*/
#define ACORAL_MSGCTR             1

/*return*/
#define MST_ERR_NULL              0
#define MST_ERR_INTR              1 
#define MST_ERR_TIMEOUT           2
#define MST_ERR_UNDEF             3

#define MSG_ERR_COUNT             3
#define MSGCTR_SUCCED             4
 
#define MSG_ERR_NULL              7  

/*全局-用来串系统中所有的acoral_msgctr_t块*/
/*     在中断函数中处理ttl和timeout*/
/*     create acoral_msgctr_t 时加到该链表中*/
extern acoral_queue_t     g_msgctr_header;
typedef struct
{
	acoral_res_t        res;
	acoral_8            *name;           /*名字*/
	acoral_u8           type;            /*类型*/
	acoral_list_t       msgctr_list;    /*全局消息列表*/
	
	acoral_spinlock_t   spin_lock;

	acoral_u32          count;           /*消息数量*/
	acoral_u32          wait_thread_num; /*等待线程数*/
	acoral_list_t       waiting;         /*等待线程指针链*/
	
	acoral_list_t       msglist;         /*消息链指针*/
}   acoral_msgctr_t;


typedef struct
{
	acoral_res_t        res;
	acoral_list_t       msglist;

	/*消息标识*/
	acoral_u32          id;
	/*消息被接收次数，每被接收一次减一,直到0为止*/
	acoral_u32          n;
	/*消息最大生命周期  ticks计数*/
	acoral_u32          ttl;

	void*               data;          /*消息指针*/
}   acoral_msg_t;


/*function define*/
void             acoral_msgctr_sys_init();
void             acoral_msg_sys_init();
void             acoral_msgctr_list_queueadd(acoral_msgctr_t*);
acoral_msgctr_t* acoral_alloc_msgctr();
acoral_msg_t*    acoral_alloc_msg();
acoral_u32       acoral_msgctr_init(acoral_msgctr_t*);
acoral_u32       acoral_msg_init(acoral_msg_t*, acoral_u32,
		                         acoral_u32, acoral_u32, void*);
/*acoral_u32       acoral_msg_assign(acoral_msg_t*, void*, acoral_u32, */
/*		                           acoral_u32,  acoral_u32);*/
acoral_msgctr_t* acoral_msgctr_create(acoral_u32*);
acoral_msg_t*    acoral_msg_create(acoral_u32, acoral_u32*, acoral_u32, 
		                           acoral_u32, void*);
acoral_u32       acoral_msg_send(acoral_msgctr_t*, acoral_msg_t*);
void*            acoral_msg_recv(acoral_msgctr_t*, acoral_u32, 
		                         acoral_u32, acoral_u32*);
acoral_u32       acoral_msgctr_del(acoral_msgctr_t*, acoral_u32);
acoral_u32       acoral_msg_del(acoral_msg_t*);
void             acoral_print_all_msg(acoral_msgctr_t*);
void             acoral_print_waitThreadsNum(acoral_msgctr_t*);


void acoral_msgctr_pool_init();
void acoral_msg_pool_init();
void wake_up_thread(acoral_list_t*);
#endif

