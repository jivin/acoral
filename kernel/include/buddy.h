#ifndef BUDDY_H
#define BUDDY_H
#include"type.h"
acoral_err buddy_init(acoral_u32 start, acoral_u32 end);
void* buddy_malloc(acoral_u32  size);
void buddy_free(void *p);
#endif
