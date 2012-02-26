#ifndef _ACORAL_SYS_ARCH_H__
#define _ACORAL_SYS_ARCH_H__

#include "arch/cc.h"
#include "message.h"

/* Lwip邮箱类型定义 */

/*
   sys_sem_t sys_mbox_t sys_thread_t
 */
typedef acoral_evt_t*     sys_sem_t;
typedef acoral_msgctr_t*  sys_mbox_t;
typedef acoral_id         sys_thread_t;
typedef acoral_msg_t*     sys_msg_t;

#define SYS_MBOX_NULL  (sys_mbox_t)NULL
#define SYS_SEM_NULL   (sys_sem_t)NULL

#define sys_arch_mbox_tryfetch(x,y)  sys_arch_mbox_fetch((x),(y),1)

#endif /* _ACORAL_SYS_ARCH_H__ */
