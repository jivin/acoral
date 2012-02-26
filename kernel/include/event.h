#ifndef ACORAL_EVENT_H
#define ACORAL_EVENT_H
#include "resource.h"
#include "spinlock.h"
#include "queue.h"

#define ACORAL_EVENT_MIN_NUM 0
#define ACORAL_EVENT_MIN_NUM 0
#define ACORAL_EVENT_SEM ACORAL_EVENT_MIN_NUM+0
#define ACORAL_EVENT_MUTEX ACORAL_EVENT_MIN_NUM+1
#define ACORAL_EVENT_WAIT ACORAL_EVENT_MIN_NUM+2
#define ACORAL_EVENT_MBOX ACORAL_EVENT_MIN_NUM+3
typedef struct {
	acoral_res_t         res;
  	acoral_u8            type;
	acoral_spinlock_t    spin_lock;
	acoral_32           count;
	acoral_queue_t      wait_queue;
	acoral_char            *name;
	void                *data;
}acoral_evt_t;

void acoral_evt_sys_init();
void acoral_evt_pool_init();
acoral_evt_t *acoral_alloc_evt();
void acoral_evt_init(acoral_evt_t *evt);
bool acoral_evt_queue_empty(acoral_evt_t *evt);
#endif
