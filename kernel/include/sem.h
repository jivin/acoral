#ifndef _ACORAL_SEM_H
#define _ACORAL_SEM_H
#include<type.h>
#include<event.h>

#define SEM_SUCCED              0
#define SEM_THREAD_SUSPEND      1
#define SEM_ERR_NULL            2
#define SEM_ERR_RES_NUM         3
#define SEM_ERR_TYPE            4
#define SEM_ERR_TASK_EXIST      5
#define SEM_ERR_INTR            6
#define SEM_ERR_UNDEF           7
#define SEM_ERR_TIMEOUT         8

#define SEM_RES_AVAI            0
#define SEM_RES_NOAVAI          1

acoral_u32   *acoral_sem_init(acoral_evt_t*,acoral_u32);
acoral_evt_t *acoral_sem_create(acoral_u32);
acoral_u32   acoral_sem_del(acoral_evt_t *, acoral_u32);
acoral_u32   acoral_sem_pend(acoral_evt_t*, acoral_u32);
acoral_u32   acoral_sem_post(acoral_evt_t*);
acoral_32    acoral_sem_getnum(acoral_evt_t*);
#endif
