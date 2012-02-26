#ifndef _ACORAL_MBOX_H
#define _ACORAL_MBOX_H
#include<type.h>
#include<event.h>

#define MBOX_SUCCED              0
#define MBOX_THREAD_SUSPEND      1
#define MBOX_ERR_NULL            2
#define MBOX_ERR_TYPE            4
#define MBOX_ERR_MES_EXIST       5
#define MBOX_ERR_INTR            6
#define MBOX_ERR_UNDEF           7
#define MBOX_ERR_TIMEOUT         8

acoral_u32    *acoral_mbox_init(acoral_evt_t*);
acoral_evt_t  *acoral_mbox_create();
acoral_u32    acoral_mbox_del(acoral_evt_t *);
acoral_u32    acoral_mbox_send(acoral_evt_t*, void*);
void*         acoral_mbox_recv(acoral_evt_t*, acoral_u32);
void*         acoral_mbox_tryrecv(acoral_evt_t*);

#endif
