/*============================
 * the realtime kernel  acoral
 *   name:  sem.c
 *   desp:  内核信号量相关函数
 *   auth:  pegasus 
 *   note:
 *
 *============================
 */
#include <type.h>
#include <event.h>
#include <thread.h>
#include <lsched.h>
#include <hal.h>
#include <int.h>
#include <timer.h>
#include <print.h>
#include <sem.h>
acoral_thread_t *acoral_evt_high_thread(acoral_evt_t *evt);
acoral_u32 *acoral_sem_init(acoral_evt_t *evt,acoral_u32 semNum)
{
	if (NULL == evt)
	{
		return NULL;
	}
	semNum = 1 - semNum;  /* 拥有多个资源，0,一个  -1 两个， -2 三个 ....*/
	evt->count = semNum;
	evt->type  = ACORAL_EVENT_SEM;
	evt->data  = NULL;
	acoral_evt_init(evt);
	return SEM_SUCCED;
}
/*============================
 *   the creation of signal
 *   计算信号量的创建
 *============================*/
acoral_evt_t *acoral_sem_create(acoral_u32 semNum)
{
	acoral_evt_t *evt;
	evt = acoral_alloc_evt();
	if (NULL == evt)
	{
		return NULL;
	}
	semNum = 1 - semNum;  /* 拥有多个资源，0,一个  -1 两个， -2 三个 ....*/
	evt->count = semNum;
	evt->type  = ACORAL_EVENT_SEM;
	evt->data  = NULL;
	acoral_evt_init(evt);
	return evt;
}

/*============================
 *  the deletion of singal 
 *   计算信号量的删除
 *============================*/
acoral_u32 acoral_sem_del(acoral_evt_t *evt, acoral_u32 opt)
{
	acoral_sr            cpu_sr;
	acoral_thread_t     *thread;
	if (acoral_intr_nesting)
	{
		return SEM_ERR_INTR;
	}
	/* 参数检测*/
	if (NULL == evt)
		return SEM_ERR_NULL; /* error*/
	if ( evt->type != ACORAL_EVENT_SEM )
		return SEM_ERR_TYPE; /* error*/

	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&evt->spin_lock);
	thread =acoral_evt_high_thread(evt);
	if (thread==NULL)
	{
		/*队列上无等待任务*/
		acoral_spin_unlock(&evt->spin_lock);
		HAL_EXIT_CRITICAL();
		evt = NULL;
		return SEM_ERR_UNDEF;
	}
	else
	{
		/*有等待任务*/
		acoral_spin_unlock(&evt->spin_lock);
		HAL_EXIT_CRITICAL();
		return SEM_ERR_TASK_EXIST; /*error*/
	}
}

/*============================
 * the applition for singal 
 *  计算信号量的申请
 *  desp: count <= SEM_RES_AVAI  信号量有效 a++
 *        count >  SEM_RES_AVAI  信号量无效 a++ && thread suspend
 *============================*/
acoral_u32 acoral_sem_trypend(acoral_evt_t *evt)
{
	acoral_sr cpu_sr;

	if (acoral_intr_nesting)
	{
		return SEM_ERR_INTR;
	}

	/* 参数检测 */
	if (NULL == evt)
	{
		return SEM_ERR_NULL; /*error*/
	}
	if (ACORAL_EVENT_SEM != evt->type)
	{
		return SEM_ERR_TYPE; /*error*/
	}

	/* 计算信号量处理*/
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&evt->spin_lock);
	if ((acoral_8)evt->count <= SEM_RES_AVAI)
	{   /* available*/
		evt->count++;
		acoral_spin_unlock(&evt->spin_lock);
		HAL_EXIT_CRITICAL();
		return SEM_SUCCED;
	}

	acoral_spin_unlock(&evt->spin_lock);
	HAL_EXIT_CRITICAL();
	return SEM_ERR_TIMEOUT;
}


/*============================
 * the applition for singal 
 *  计算信号量的申请
 *  desp: count <= SEM_RES_AVAI  信号量有效 a++
 *        count >  SEM_RES_AVAI  信号量无效 a++ && thread suspend
 *============================*/
acoral_u32 acoral_sem_pend(acoral_evt_t *evt, acoral_time timeout)
{
	acoral_thread_t *cur = acoral_cur_thread;
	acoral_sr cpu_sr;

	if (acoral_intr_nesting)
	{
		return SEM_ERR_INTR;
	}

	/* 参数检测 */
	if (NULL == evt)
	{
		return SEM_ERR_NULL; /*error*/
	}
	if (ACORAL_EVENT_SEM != evt->type)
	{
		return SEM_ERR_TYPE; /*error*/
	}

	/* 计算信号量处理*/
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&evt->spin_lock);
	if ((acoral_8)evt->count <= SEM_RES_AVAI)
	{   /* available*/
		evt->count++;
		acoral_spin_unlock(&evt->spin_lock);
		HAL_EXIT_CRITICAL();
		return SEM_SUCCED;
	}

	evt->count++;
	acoral_unrdy_thread(cur);
	if (timeout > 0)
	{
		cur->delay = TIME_TO_TICKS(timeout);
		timeout_queue_add(cur);
	}
	acoral_evt_queue_add(evt,cur);
	acoral_spin_unlock(&evt->spin_lock);
	HAL_EXIT_CRITICAL();
	
	acoral_sched();

	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&evt->spin_lock);
	if(timeout>0 && cur->delay<=0)
	{
		//--------------
		// modify by pegasus 0804: count-- [+]
		evt->count--;
		acoral_evt_queue_del(cur);
		acoral_spin_unlock(&evt->spin_lock);
		HAL_EXIT_CRITICAL();
		return SEM_ERR_TIMEOUT;
	}

	//-------------------
	// modify by pegasus 0804: timeout_queue_del [+]
	timeout_queue_del(cur);
	acoral_spin_unlock(&evt->spin_lock);
	HAL_EXIT_CRITICAL();
	return SEM_SUCCED;
}

/*===========================
 *  Release a signal
 *  计算信号量的释放
 *  desp: count > SEM_RES_NOAVAI 有等待线程 a-- && resume waiting thread.
 *        count <= SEM_RES_NOAVAI 无等待线程 a--
 *===========================*/
acoral_u32 acoral_sem_post(acoral_evt_t *evt)
{
	acoral_thread_t     *thread;
	acoral_sr            cpu_sr;

	/* 参数检测*/
	if ( NULL == evt)
	{
		return SEM_ERR_NULL; /* error*/
	}
	if (ACORAL_EVENT_SEM != evt->type)
	{
		return SEM_ERR_TYPE;
	}

	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&evt->spin_lock);

	/* 计算信号量的释放*/
	if ((acoral_8)evt->count <= SEM_RES_NOAVAI)
	{ /* no waiting thread*/
		evt->count--;
		acoral_spin_unlock(&evt->spin_lock);
		HAL_EXIT_CRITICAL();
		return SEM_SUCCED;
	}
	/* 有等待线程*/
	evt->count--;
	thread =acoral_evt_high_thread(evt);
	if (thread==NULL)
	{
		/*应该有等待线程却没有找到*/
		acoral_printerr("Err Sem post\n");
		acoral_spin_unlock(&evt->spin_lock);
		HAL_EXIT_CRITICAL();
		return SEM_ERR_UNDEF;
	}
	timeout_queue_del(thread);
	/*释放等待任务*/
	acoral_evt_queue_del(thread);
	acoral_rdy_thread(thread);
	acoral_spin_unlock(&evt->spin_lock);
	HAL_EXIT_CRITICAL();
	acoral_sched();
	return SEM_SUCCED;
}

/*===================================
 *   get singal's number now
 *     得到当前信号量数目
 *===================================*/
acoral_32 acoral_sem_getnum(acoral_evt_t* evt)
{
	acoral_32 t;
	acoral_sr cpu_sr;

	if (NULL == evt)
		return SEM_ERR_NULL;

	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&evt->spin_lock);
	t = 1 - (acoral_32)evt->count;
	acoral_spin_unlock(&evt->spin_lock);
	HAL_EXIT_CRITICAL();
	return t;
}

