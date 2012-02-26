#include <type.h>
#include <event.h>
#include <hal.h>
#include <thread.h>
#include <lsched.h>
#include <int.h>
#include <timer.h>
#include <message.h>

acoral_u32   acoral_doMsgTtl_checked = DO_CHECKED_FALSE;

acoral_pool_ctrl_t acoral_msgctr_pool_ctrl;
acoral_pool_ctrl_t acoral_msg_pool_ctrl;
acoral_queue_t     g_msgctr_header;
/*=============================
 *
 *  mssage 机制缓冲池初始化
 *=============================*/
void acoral_msg_sys_init()
{
	/*初始化全局事件列表头*/
	acoral_init_list (&(g_msgctr_header.head));
	g_msgctr_header.data = NULL;
	acoral_msgctr_pool_ctrl.type=ACORAL_RES_MST;
	acoral_msgctr_pool_ctrl.size=sizeof(acoral_msgctr_t);
	acoral_msgctr_pool_ctrl.num_per_pool=10;
	acoral_msgctr_pool_ctrl.max_pools=4;

	acoral_msg_pool_ctrl.type=ACORAL_RES_MSG;
	acoral_msg_pool_ctrl.size=sizeof(acoral_msg_t);
	acoral_msg_pool_ctrl.num_per_pool=10;
	acoral_msg_pool_ctrl.max_pools=4;

	acoral_pool_ctrl_init(&acoral_msgctr_pool_ctrl);
	acoral_pool_ctrl_init(&acoral_msg_pool_ctrl);
}

/*==============================
 *
 *  静态创建的acoral_msgctr_t调用
 *  message container 挂全局队列
 *  在中断中处理 ttl 时使用
 *==============================*/
void acoral_msgctr_list_queueadd(acoral_msgctr_t* msgctr)
{
  	acoral_list_add2_tail (&msgctr->msgctr_list, &(g_msgctr_header.head));
}

/*==============================
 *
 *  message 分配
 *==============================*/
acoral_msgctr_t* acoral_alloc_msgctr()
{
	return (acoral_msgctr_t*)acoral_get_res (&acoral_msgctr_pool_ctrl);
}

acoral_msg_t* acoral_alloc_msg()
{
	return (acoral_msg_t*)acoral_get_res (&acoral_msg_pool_ctrl);
}

/*==============================
 *
 *  message 等待队列增加
 *==============================*/
void acoral_msgctr_queue_add (acoral_msgctr_t* msgctr, 
		                            acoral_thread_t* thread)
{   /*需按优先级排序*/
	/*acoral_list_add2_tail (&thread->waiting, &msgctr->waiting);*/
	acoral_list_t *p, *q;
	acoral_thread_t*  ptd;
	
	p = &msgctr->waiting;
	q = p->next;
	for( ; p != q; q=q->next)
	{
		ptd = list_entry( q, acoral_thread_t, waiting);
		if (ptd->prio > thread->prio)
			break;
	}
	acoral_list_add( &thread->waiting, q->prev);
}

void acoral_msg_queue_add (acoral_msgctr_t* msgctr, 
		                          acoral_msg_t* msg)
{
	acoral_list_add2_tail (&msg->msglist, &msgctr->msglist);
}


/*----------------------------------------------------------*/
/*==============================
 * The initialize of the message
 *       消息容器初始化
 *==============================*/
acoral_u32  acoral_msgctr_init(acoral_msgctr_t *msgctr)
{
	if (msgctr == NULL)
		return MST_ERR_NULL;

	msgctr->name  = NULL;
	msgctr->type  = ACORAL_MSGCTR;
	msgctr->count = 0;
	msgctr->wait_thread_num = 0;

	acoral_init_list(&msgctr->msglist);
	acoral_init_list(&msgctr->waiting);
	acoral_init_list(&msgctr->msgctr_list);
	acoral_spin_init(&msgctr->spin_lock);
	return MSGCTR_SUCCED;
}

acoral_u32 acoral_msg_init(
		acoral_msg_t *msg, acoral_u32 id /*= 0*/, acoral_u32 n /*= 0*/, 
		acoral_u32  nTtl /*= 0*/, void* dat /*= NULL*/)
{
	if (msg == NULL)
		return MSG_ERR_NULL;

	msg->id   =  id;                /*消息标识*/
	msg->n    =  n;                 /*消息被接收次数*/
	msg->ttl  =  nTtl;              /*消息生存周期*/
	msg->data =  dat;               /*消息指针*/
	acoral_init_list(&msg->msglist);
	acoral_spin_init(&(msg->msglist.lock));
	return 0;
}

acoral_32 acoral_msg_assign(
		acoral_msg_t* msg, void* dat, acoral_u32 id,
		acoral_u32 nTtl, acoral_u32 n)
{
	if (msg == NULL)
		return MSG_ERR_NULL;

	msg->id   =  id;                /*消息标识*/
	msg->n    =  n;                 /*消息被接收次数*/
	msg->ttl  =  nTtl;              /*消息生存周期*/
	msg->data =  dat;               /*消息指针*/
	return 0;
}

/*==================================
 *
 *   消息创建
 *==================================*/
acoral_msgctr_t*  acoral_msgctr_create (acoral_u32 *err)
{
	acoral_msgctr_t *msgctr;

	msgctr = acoral_alloc_msgctr();

	if (msgctr == NULL)
		return NULL;

	msgctr->name  = NULL;
	msgctr->type  = ACORAL_MSGCTR;
	msgctr->count = 0;
	msgctr->wait_thread_num = 0;

	acoral_init_list(&msgctr->msgctr_list);
	acoral_init_list(&msgctr->msglist);
	acoral_init_list(&msgctr->waiting);

	acoral_spin_init(&(msgctr->msgctr_list.lock));
	acoral_spin_init(&(msgctr->msglist.lock));
	acoral_spin_init(&(msgctr->waiting.lock));

	acoral_spin_init(&msgctr->spin_lock);
	acoral_list_add2_tail (&msgctr->msgctr_list, &(g_msgctr_header.head));
	return msgctr;
}

acoral_msg_t* acoral_msg_create (
		acoral_u32 n, acoral_u32 *err, acoral_u32 id, 
		acoral_u32 nTtl/* = 0*/, void* dat /*= NULL*/)
{
	acoral_msg_t *msg;

	msg = acoral_alloc_msg();

	if (msg == NULL)
		return NULL;

	msg->id   =  id;                 /*消息标识*/
	msg->n    =  n;                  /*消息被接收次数*/
	msg->ttl  =  nTtl;               /*消息生存周期*/
	msg->data =  dat;                /*消息指针*/
	acoral_init_list(&msg->msglist);
	return msg;
}

/*===================================
 * 
 *   消息发送
 *===================================*/
acoral_u32 acoral_msg_send(acoral_msgctr_t* msgctr, acoral_msg_t* msg)
{
	acoral_sr     cpu_sr;
	
/*	if (acoral_intr_nesting > 0)
		return MST_ERR_INTR;
*/
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&msgctr->spin_lock);

	if (NULL == msgctr)
	{
		acoral_spin_unlock(&msgctr->spin_lock);
		HAL_EXIT_CRITICAL();
		return MST_ERR_NULL;
	}

	if (NULL == msg)
	{
		acoral_spin_unlock(&msgctr->spin_lock);
		HAL_EXIT_CRITICAL();
		return MSG_ERR_NULL;
	}

	/*----------------*/
	/*   消息数限制*/
	/*----------------*/
	if (ACORAL_MESSAGE_MAX_COUNT <= msgctr->count)
	{
		acoral_spin_unlock(&msgctr->spin_lock);
		HAL_EXIT_CRITICAL();
		return MSG_ERR_COUNT;
	}

	/*----------------*/
	/*   增加消息*/
	/*----------------*/
	msgctr->count++;
	msg->ttl += acoral_get_ticks();
	acoral_list_add2_tail(&msg->msglist, &msgctr->msglist);

	/*----------------*/
	/*   唤醒等待*/
	/*----------------*/
	if (msgctr->wait_thread_num > 0)
	{
		/* 此处将最高优先级唤醒*/
		wake_up_thread(&msgctr->waiting);
		msgctr->wait_thread_num--;
	}
	acoral_spin_unlock(&msgctr->spin_lock);
	HAL_EXIT_CRITICAL();
	acoral_sched();
	return MSGCTR_SUCCED;
}

/*===================================
 *
 *  消息接收
 *===================================*/
void* acoral_msg_recv (acoral_msgctr_t* msgctr,
						acoral_u32  id,
						acoral_time  timeout,
						acoral_u32  *err)
{
	void             *dat;
	acoral_sr        cpu_sr;
	acoral_list_t    *p, *q;
	acoral_msg_t     *pmsg;
	acoral_thread_t  *cur;

	if (acoral_intr_nesting > 0)
	{
		*err = MST_ERR_INTR;
		return NULL;
	}
	if (NULL == msgctr)
	{
		*err = MST_ERR_NULL;
		return NULL;
	}

	cur = acoral_cur_thread;

	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&msgctr->spin_lock);
	if(timeout>0){
		cur->delay = TIME_TO_TICKS(timeout);
		timeout_queue_add( cur);
	}
	while(1)
	{
		p = &msgctr->msglist;
		q = p->next;
		for( ;p != q; q = q->next)
		{
			pmsg = list_entry( q, acoral_msg_t, msglist);
			if ( (pmsg->id == id) && (pmsg->n > 0))
			{
				/*-----------------*/
				/* 有接收消息*/
				/*-----------------*/
				pmsg->n--;
				/*-----------------*/
				/* 延时列表删除*/
				/*-----------------*/
				timeout_queue_del(cur);
				dat = pmsg->data;
				acoral_list_del (q);
				acoral_release_res ((acoral_res_t *)pmsg);		
				msgctr->count--;
				acoral_spin_unlock(&msgctr->spin_lock);
				HAL_EXIT_CRITICAL();
				return dat;
			}
		}
	
		/*-----------------*/
		/*  没有接收消息*/
		/*-----------------*/
		msgctr->wait_thread_num++;
		acoral_msgctr_queue_add(msgctr, cur);
		acoral_unrdy_thread(cur);
		acoral_spin_unlock(&msgctr->spin_lock);
		HAL_EXIT_CRITICAL();
		acoral_sched();
		/*-----------------*/
		/*  看有没有超时*/
		/*-----------------*/
		HAL_ENTER_CRITICAL();
		acoral_spin_lock(&msgctr->spin_lock);
	
		if (timeout>0&&(acoral_32)cur->delay <=0 )
			break;

	}

	/*---------------*/
	/*  超时退出*/
	/*---------------*/
//	timeout_queue_del(cur);
	if(msgctr->wait_thread_num>0)
		msgctr->wait_thread_num--;
	acoral_list_del(&cur->waiting);
	acoral_spin_unlock(&msgctr->spin_lock);
	HAL_EXIT_CRITICAL();
	*err = MST_ERR_TIMEOUT;
	return NULL;

}

/*===================================
 *
 *  消息删除
 *===================================*/
acoral_u32 acoral_msgctr_del(acoral_msgctr_t* pmsgctr, acoral_u32 flag)
{
	acoral_list_t    *p, *q;
	acoral_thread_t  *thread;
	acoral_msg_t     *pmsg;
	
	if(NULL == pmsgctr)
		return MST_ERR_NULL;
	if (flag == MST_DEL_UNFORCE)
	{
		if ((pmsgctr->count > 0) || (pmsgctr->wait_thread_num > 0))
			return MST_ERR_UNDEF;
		else
			acoral_release_res((acoral_res_t *)pmsgctr);
	}
	else
	{
		// 释放等待进程
		if (pmsgctr->wait_thread_num > 0)
		{
			p = &pmsgctr->waiting;
			q = p->next;
			for(; q != p; q = q->next)
			{
				thread=list_entry( q, acoral_thread_t, waiting);
				//acoral_list_del  (&thread->waiting);
				acoral_rdy_thread(thread);
			}
		}
		
		// 释放消息结构
		if (pmsgctr->count > 0)
		{
			p = &pmsgctr->msglist;
			q = p->next;
			for( ;p != q; q = p->next)
			{
				pmsg = list_entry( q, acoral_msg_t, msglist);
				acoral_list_del (q);
				acoral_release_res ((acoral_res_t *)pmsg);		
			}		
		}
		
		// 释放资源
		acoral_release_res((acoral_res_t *)pmsgctr);
	}
}

acoral_u32 acoral_msg_del(acoral_msg_t* pmsg)
{
	if (NULL != pmsg)
		acoral_release_res ((acoral_res_t *)pmsg);
	return 0;
}

/*-----------------------------------------------------*/

/*==========================
 *
 *  唤醒最高优先等待线程
 *==========================*/
void wake_up_thread(acoral_list_t  *head)
{
	acoral_list_t    *p, *q;
	acoral_thread_t  *thread;

	p = head;
	q = p->next;
	thread=list_entry( q, acoral_thread_t, waiting);
	acoral_list_del  (&thread->waiting);
	acoral_rdy_thread(thread);
}

/*======================*/
/**/
/*  输出事件容器上全部消息*/
/*======================*/
void acoral_print_all_msg(acoral_msgctr_t* msgctr)
{
	acoral_list_t *p, *q;
	acoral_msg_t  *pmsg;

	acoral_doMsgTtl_checked = DO_CHECKED_TRUE;

	p = &msgctr->msglist;
	q = p->next;
	for( ;p != q; q = q->next)
	{
		pmsg = list_entry( q, acoral_msg_t, msglist);
		acoral_print ("\nid = %d", pmsg->id);
	}

	acoral_doMsgTtl_checked = DO_CHECKED_FALSE;

}

/*======================*/
/**/
/*  输出等待任务数目*/
/*======================*/
void acoral_print_waitThreadsNum(acoral_msgctr_t* msgctr)
{
	acoral_print("\n wait threads mum = %d", msgctr->wait_thread_num);
}
