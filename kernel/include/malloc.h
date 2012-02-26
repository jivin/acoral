#ifndef ACORAL_MALLOC_H
#define ACORAL_MALLOC_H
#include<type.h>
void * v_malloc(acoral_32 size);
void v_free(void * p);
void v_mem_init();
void v_mem_scan(void);
#endif
