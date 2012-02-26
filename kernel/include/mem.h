#ifndef ACORAL_MEM_H
#define ACORAL_MEM_H
#include<autocfg.h>
#ifdef CFG_MEM_BUDDY
#include<buddy.h>
#define acoral_malloc(size) buddy_malloc(size)
#define acoral_free(ptr) buddy_free(ptr)
#define acoral_malloc_size(size) buddy_malloc_size(size)
#define acoral_mem_init(start,end) buddy_init(start,end)
#define acoral_mem_scan() buddy_scan()
#endif

#ifdef CFG_MEM_SLATE
#include<slate.h>
#define acoral_malloc(size) slate_malloc(size)
#define acoral_free(ptr) slate_free(ptr)
#define acoral_malloc_size(size) slate_malloc_size(size)
#define acoral_mem_init(start,end) slate_init(start,end)
#define acoral_mem_scan() slate_scan()
#endif

#ifdef CFG_MEM2
#include<malloc.h>
#define acoral_mem_init2() v_mem_init()
#define acoral_malloc2(size) v_malloc(size)
#define acoral_free2(p) v_free(p)
#define acoral_mem_scan2() v_mem_scan()
#endif

#endif
