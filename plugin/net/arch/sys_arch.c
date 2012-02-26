#include "acoral.h"
#include "arch/cc.h"
#include "arch/sys_arch.h"
#include "lwip/sys.h"


acoral_u32  pbuf_release = 0;

struct timeoutnode
{
	struct sys_timeouts timeouts;
	u8_t prio;
	struct timeoutnode *next;
};


struct timeoutnodes_t
{
	struct timeoutnode  p[TCPIP_THREAD_MAXNUM+1];
	int    flag;
} timeoutnodes;

struct timeoutnode nulltimeouts;
struct timeoutnode *timeoutslist;
const void * const pvNullPointer;

/*-----------------------------
   void sys_init()
 */
void sys_init(void)
{
	int i;
	
    timeoutslist = &nulltimeouts;
    
    nulltimeouts.timeouts.next = NULL; 
    
    nulltimeouts.next = NULL;
    
    /* timeoutnodes init */
    timeoutnodes.flag = 0;
    for(i=0; i<TCPIP_THREAD_MAXNUM; i++)
    {
    	timeoutnodes.p[i].next = NULL;  /*初始化为未使用*/
    	timeoutnodes.p[i].timeouts.next = NULL;
    }
    
}

/*----------------------------
	sys_sem_t sys_sem_new(u8_t count)
 */
sys_sem_t sys_sem_new(u8_t count)
{
	return acoral_sem_create(count);
}

/*------------------------------
   void sys_sem_free(sys_sem_t sem)
 */
void sys_sem_free(sys_sem_t sem)
{
	acoral_sem_del(sem, 0);
}

/*--------------------------------
   void sys_sem_signal(sys_sem_t sem)
 */
void sys_sem_signal(sys_sem_t sem)
{
	acoral_sem_post(sem);
}

/*----------------------------------
   u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
 */
u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
	if(SEM_SUCCED == acoral_sem_pend(sem, timeout))
		return 0;
	else
		return SYS_ARCH_TIMEOUT;
}


/*---------------------------------
   sys_mbox_t sys_mbox_new()
 */
sys_mbox_t sys_mbox_new(int size)
{
	u32_t  err;
	return acoral_msgctr_create(&err);
	//return acoral_mbox_create();
}

/*--------------------------------
   void sys_mbox_free(sys_mbox_t mbox)
 */
void sys_mbox_free(sys_mbox_t mbox)
{
	acoral_msgctr_del(mbox, 1);
	//acoral_mbox_del(mbox);
}

/*--------------------------------
   void sys_mbox_post(sys_mbox_t  mbox, void *msg)
 */
void sys_mbox_post(sys_mbox_t mbox, void* msg)
{
	u32_t err;
	u32_t id = 1;
	sys_msg_t pmsg = acoral_msg_create(1, &err, id, 0, msg);
	if (NULL != pmsg)
		acoral_msg_send(mbox, pmsg);
	//acoral_mbox_send(mbox,msg);
}

/*--------------------------------
   err_t sys_mbox_trypost(sys_mbox_t  mbox, void *msg)
 */
err_t sys_mbox_trypost(sys_mbox_t mbox, void* msg)
{
	u32_t err;
	u32_t id = 1;
	sys_msg_t pmsg = acoral_msg_create(1, &err, id, 0, msg);
	if (NULL != pmsg)
		acoral_msg_send(mbox, pmsg);
	//acoral_mbox_send(mbox,msg);
	return ERR_OK;
}

/*---------------------------------
   u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void** msg, u32_t timeout)
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void** msg, u32_t timeout)
{

	u32_t err;
	u32_t id = 1;
	*msg = acoral_msg_recv(mbox, id, timeout, &err);
	//*msg = acoral_mbox_recv(mbox, timeout);
	if (*msg == NULL)
		return SYS_ARCH_TIMEOUT;
	else
		return 0;
}


int getTONAvai()
{
	int ret;
    
    if(timeoutnodes.flag < TCPIP_THREAD_MAXNUM)
    {
	    ret =timeoutnodes.flag;
    
    	timeoutnodes.flag += 1;
    	return ret;
    }
   	return TCPIP_THREAD_MAXNUM;
}

/*----------------------------------
   sys_thread_t sys_thread_new(void(*thread)(void* arg), void *arg, int prio)
 */
sys_thread_t sys_thread_new(char *name, void (* thread)(void *arg), void *arg, int stacksize, int prio)
{
		sys_thread_t tmp;
		int          i;
		struct timeoutnode * pto;
		SYS_ARCH_DECL_PROTECT(x);
		
		tmp =  acoral_create_thread(thread, TCPIP_THREAD_STACKSIZE, arg, name, prio, 0 );
		i   = getTONAvai();
		if (i >= TCPIP_THREAD_MAXNUM)
			return 0;
			
		pto = &timeoutnodes.p[i];
		
		pto->prio = prio;
		    
		pto->timeouts.next = NULL;
       	SYS_ARCH_PROTECT(x)
	    pto->next = timeoutslist;
		timeoutslist = pto;
      	SYS_ARCH_UNPROTECT(x)
		
		return tmp;	

}
/********************************************
* 函数名称 : sys_arch_timeouts
* 描    述 : 获得线程的超时结构
* 输　	 入 : 无
*        
* 输　	 出 : struct sys_timeouts *: 线程的超时结构
********************************************/
struct sys_timeouts * sys_arch_timeouts(void)
{
    struct timeoutnode * pto = timeoutslist;
   	u8_t curprio;	    
	SYS_ARCH_DECL_PROTECT(cpusr);

	SYS_ARCH_PROTECT(cpusr);
	curprio = acoral_cur_thread->prio;
    SYS_ARCH_UNPROTECT(cpusr);
           	    
    while (pto != &nulltimeouts)
    {
    	if (pto->prio == curprio)
    	{
    		return &(pto->timeouts);
    	}
    	else
    	{
    		pto = pto->next;
    	}
    }

    return &(pto->timeouts);
}


