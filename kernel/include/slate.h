#ifndef SLATE_H
#define SLATE_H
#include"type.h"
acoral_err slate_init(acoral_u32 start, acoral_u32 end);
void* slate_malloc(acoral_u32  size);
void slate_free(void *p);
#endif
