/*
 * queue.h
 *
 *  Created on: 2010-3-8
 *      Author: jivin
 */

#ifndef QUEUE_H_
#define QUEUE_H_
#include<spinlock.h>
#include<list.h>
#include<core.h>
#define ACORAL_PRIO_QUEUE_EMPTY 255
#define PRIO_BITMAP_SIZE ((((ACORAL_MAX_PRIO_NUM+1+7)/8)+sizeof(acoral_u32)-1)/sizeof(acoral_u32))
typedef struct {
	acoral_list_t head;
	void *data;
}acoral_queue_t;

struct acoral_prio_array {
	acoral_u32 num;
	acoral_u32 bitmap[PRIO_BITMAP_SIZE];
	acoral_queue_t queue[ACORAL_MAX_PRIO_NUM];
};
typedef struct acoral_prio_array acoral_prio_array_t;

#endif /* QUEUE_H_ */
