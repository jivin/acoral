/*--------------------*/
/*   增加 acoral_delayqueue_add定义 */
/*   pegasus    0719*/
/*--------------------*/


#ifndef ACORAL_TIMER_H
#define ACORAL_TIMER_H
#include <autocfg.h>
#include <type.h>
#include <core.h>
#include <queue.h>
#include <thread.h>
#ifndef CFG_CMP
#define CFG_TICKS_PRIVATE
#endif
#define acoral_ticks acoral_get_ticks()
/*计算time对应的ticks数量*/
#define TIME_TO_TICKS(time) (time)*ACORAL_TICKS_PER_SEC/1000
extern acoral_queue_t time_delay_queue;
extern acoral_queue_t timeout_queue;

void acoral_time_init(void);
void acoral_ticks_init(void);
void acoral_ticks_entry(acoral_vector vector);
acoral_time acoral_get_ticks(void);
void acoral_set_ticks(acoral_time time);
void time_delay_deal(void);
void acoral_delayqueue_add(acoral_queue_t*, acoral_thread_t*);
acoral_evt_t *acoral_alloc_evt(void);
void timeout_delay_deal();
void timeout_queue_add(acoral_thread_t*);
void timeout_queue_del(acoral_thread_t*);

#endif

