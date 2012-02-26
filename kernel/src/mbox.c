/*====================================
 *    the real kernel acoral
 *    file: mbox.c
 *    by pegasus   06.04
 *====================================*/

#include <type.h>
#include <event.h>
#include <thread.h>
#include <lsched.h>
#include <hal.h>
#include <int.h>
#include <timer.h>
#include <mbox.h>

#define  MBOX_L_MASK  0x0000FFFF
#define  MBOX_U_MASK  0xFFFF0000
acoral_thread_t *acoral_evt_high_thread(acoral_evt_t *evt);
/*====================================
 *     create a mailbox
 *       创建一个邮箱
 *====================================*/
acoral_evt_t *acoral_mbox_create()
{
  	acoral_evt_t * event;
	
	event=acoral_alloc_evt();
	if (NULL == event)
		return NULL;

	event->type  = ACORAL_EVENT_MBOX;
	event->count = 0x00000000;
	event->data  = NULL;
	acoral_evt_init(event);
	return event;
}

/*=====================================
 *   delete a mailbox
 *     删除一个邮箱
 *=====================================*/
acoral_u32 acoral_mbox_del(acoral_evt_t* event)
{
	if (acoral_intr_nesting > 0)
		return MBOX_ERR_INTR;
	
	if (NULL == event)
		return MBOX_ERR_NULL;

	if(event->type != ACORAL_EVENT_MBOX)
		return MBOX_ERR_TYPE;

	acoral_release_thread((acoral_res_t *)event); 
	return MBOX_SUCCED;
}

/*======================================
 *     send a mail to mailbox
 *       发送邮件至邮箱
 *======================================*/
acoral_u32 acoral_mbox_send(acoral_evt_t * event, void *msg)
{
	acoral_sr            cpu_sr;
	acoral_thread_t     *thread;

	/*if (acoral_intr_nesting > 0)
		return MBOX_ERR_INTR;
	*/
	if (NULL == event)
		return MBOX_ERR_NULL;

	if(event->type != ACORAL_EVENT_MBOX)
		return MBOX_ERR_TYPE;

	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&event->spin_lock);
	if(event->data != NULL)
	{
		acoral_spin_unlock(&event->spin_lock);
		HAL_EXIT_CRITICAL();
		return MBOX_ERR_MES_EXIST;
	}
	event->data = msg;
	thread =acoral_evt_high_thread(event);
	/* 释放等待进程*/
	if (thread==NULL)
	{
		/* 没有等待队列*/
		acoral_spin_unlock(&event->spin_lock);
		HAL_EXIT_CRITICAL();
		return  MBOX_SUCCED;
	}
	/*释放等待任务*/
	timeout_queue_del(thread);
	acoral_evt_queue_del(thread);
	acoral_rdy_thread(thread);
	acoral_spin_unlock(&event->spin_lock);
	HAL_EXIT_CRITICAL();
	acoral_sched();
	return MBOX_SUCCED;
}

/*======================================
 *    receive  a mail to mailbox
 *       接收邮件从邮箱
 *======================================*/
void* acoral_mbox_recv(acoral_evt_t * event, acoral_time timeout)
{
	void            *msg;
	acoral_sr         cpu_sr;
	acoral_thread_t  *cur;
	
	if (acoral_intr_nesting > 0)
		return NULL;

	if(event->type!=ACORAL_EVENT_MBOX)
		return NULL;
	
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&event->spin_lock);
	if( event->data == NULL)
	{
		cur = acoral_cur_thread;
		if (timeout > 0)
		{
			cur->delay = TIME_TO_TICKS(timeout);
			timeout_queue_add(cur);
		}
		acoral_unrdy_thread(cur);
		acoral_evt_queue_add(event, cur);
		acoral_spin_unlock(&event->spin_lock);
		HAL_EXIT_CRITICAL();
		acoral_sched();
		HAL_ENTER_CRITICAL();
		acoral_spin_lock(&event->spin_lock);

		if (timeout > 0 && cur->delay <= 0)
		{
			acoral_evt_queue_del(cur);
			acoral_spin_unlock(&event->spin_lock);
			HAL_EXIT_CRITICAL();
			return NULL;
		}

		msg        = event->data;
		event->data = NULL;
		acoral_spin_unlock(&event->spin_lock);
		HAL_EXIT_CRITICAL();
		return msg;
	}
	
	msg         = event->data;
	event->data = NULL;
	acoral_spin_unlock(&event->spin_lock);
	HAL_EXIT_CRITICAL();

	return msg;
}

/*======================================
 *   try receive  a mail to mailbox
 *       接收邮件从邮箱
 *======================================*/
void* acoral_mbox_tryrecv(acoral_evt_t * event)
{
	void            *msg;
	acoral_sr         cpu_sr;
	
	if (acoral_intr_nesting > 0)
		return NULL;

	if(event->type!=ACORAL_EVENT_MBOX)
		return NULL;
	
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&event->spin_lock);
	if( event->data == NULL)
	{
		acoral_spin_unlock(&event->spin_lock);
		HAL_EXIT_CRITICAL();
		return NULL;
	}
	
	msg         = event->data;
	event->data = NULL;
	acoral_spin_unlock(&event->spin_lock);
	HAL_EXIT_CRITICAL();

	return msg;
}
