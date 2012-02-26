#ifndef _ACORAL_MUTEX_H
#define _ACORAL_MUTEX_H
#include<type.h> 
#include<event.h> 


#define MUTEX_AVAI    0x00FF

#define MUTEX_L_MASK  0x00FF
#define MUTEX_U_MASK  0xFF00

#define MUTEX_SUCCED              0
#define MUTEX_THREAD_SUSPEND      1
#define MUTEX_ERR_NULL            2
#define MUTEX_ERR_TYPE            3
#define MUTEX_ERR_TASK_EXIST      4
#define MUTEX_ERR_INTR            5
#define MUTEX_ERR_UNDEF           6
#define MUTEX_ERR_TIMEOUT         7
#define MUTEX_ERR_RDY             8

acoral_u32     acoral_mutex_init(acoral_evt_t*,acoral_u8);
acoral_evt_t  *acoral_mutex_create(acoral_u8, acoral_u32*);
acoral_u32     acoral_mutex_del(acoral_evt_t*, acoral_u32);
acoral_u32     acoral_mutex_pend(acoral_evt_t*, acoral_u32);
acoral_u32     acoral_mutex_post(acoral_evt_t*);
acoral_u32     acoral_mutex_trypend(acoral_evt_t*);

#endif
