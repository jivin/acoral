#ifndef ACORAL_RES_H
#define ACORAL_RES_H
#include<type.h>
#include<core.h>
#include<list.h>
#include<spinlock.h>
#define ACORAL_MAX_POOLS 40

#define ACORAL_MAX_RES_TYPE 4

#define ACORAL_RES_THREAD 1
#define ACORAL_RES_EVENT 2
#define ACORAL_RES_TIMER 3
#define ACORAL_RES_DRIVER 4
#define ACORAL_RES_GUI 5
#define ACORAL_RES_USER 6
/*------------------*/
/* pegasus  0719*/
/*------------------*/
#define ACORAL_RES_MSG    5
#define ACORAL_RES_MST    6

#define ACORAL_POOL_INDEX_BIT 0
#define ACORAL_POOL_ID_BIT 0

#define ACORAL_RES_TYPE_BIT 10
#define ACORAL_RES_INDEX_BIT 14
#define ACORAL_RES_INDEX_INIT_BIT 16
#define ACORAL_RES_CPU_BIT 24
#define ACORAL_RES_ID_BIT 24

#define ACORAL_POOL_INDEX_MASK 0x000003FF
#define ACORAL_POOL_ID_MASK    0x00003FFF

#define ACORAL_RES_TYPE_MASK   0x00003c00
#define ACORAL_RES_INDEX_MASK  0x00FFC000
#define ACORAL_RES_ID_MASK     0x00FFFFFF
#define ACORAL_RES_CPU_MASK    0xFF000000

#define ACORAL_RES_ERR         0xf0   
#define ACORAL_RES_NO_RES      ACORAL_RES_ERR+0 
#define ACORAL_RES_NO_POOL     ACORAL_RES_ERR+1 
#define ACORAL_RES_NO_MEM     ACORAL_RES_ERR+2 
#define ACORAL_RES_MAX_POOL    ACORAL_RES_ERR+3 

/*================================
 *    get a kind of resource by res_id
 *      根据资源ID获取某一资源数据块
 *      res_id--资源ID
 *================================*/
#define ACORAL_RES_TYPE(id) ((id&ACORAL_RES_TYPE_MASK)>>ACORAL_RES_TYPE_BIT)
#define ACORAL_RES_CPU(id) ((id&ACORAL_RES_CPU_MASK)>>ACORAL_RES_CPU_BIT)

typedef union {
   acoral_id id;
   acoral_u16 next_id;
}acoral_res_t;


typedef struct{
      void (*release_res)(acoral_res_t *res);
}acoral_res_api_t;

typedef struct {
  acoral_u32       type;
  acoral_u32       size;
  acoral_u32 	  num_per_pool;
  acoral_u32       num;
  acoral_u32 	  max_pools;
  acoral_list_t 	 *free_pools,*pools,list[2];
  acoral_res_api_t *api;
  acoral_spinlock_t lock;
  acoral_u8 *name;
}acoral_pool_ctrl_t;

typedef struct {
   void *base_adr;
   /*这个有两个作用，在为空闲的时候,它指向下一个pool，否则为它管理的资源的基地址*/
   void *res_free;
   acoral_id id;
   acoral_u32 size;
   acoral_u32 num;
   acoral_u32 position;
   acoral_u32 free_num;
   acoral_pool_ctrl_t *ctrl;
   acoral_list_t ctrl_list;
   acoral_list_t free_list;
   acoral_spinlock_t lock;
}acoral_pool_t;

typedef struct{
    void (*create_pool)(acoral_pool_ctrl_t * res_ctrl);
    void (*release_pool)(acoral_pool_ctrl_t * res_ctrl);
    acoral_res_t *(*get_res)(acoral_pool_ctrl_t *res_ctrl);
    acoral_u32* (*release_res)(acoral_res_t *res);
}acoral_res_ops_t;

acoral_char acoral_assert_res(acoral_res_t *res,acoral_8 *assert);
acoral_pool_t *acoral_get_pool_by_id(acoral_id id);
acoral_pool_t *acoral_get_free_pool(void);
acoral_err acoral_create_pool(acoral_pool_ctrl_t *pool_ctrl);
void acoral_pool_res_init(acoral_pool_t *pool);
void acoral_release_pool(acoral_pool_ctrl_t *pool_ctrl);
void acoral_collect_pool(acoral_pool_ctrl_t *pool_ctrl);
acoral_res_t *acoral_get_res(acoral_pool_ctrl_t *pool_ctrl);
void acoral_release_res(acoral_res_t *res);
acoral_res_t * acoral_get_res_by_id(acoral_id id);
void  acoral_pools_init(void);
void acoral_pool_res_init(acoral_pool_t * pool);
acoral_u8 acoral_get_cpu_by_id(acoral_id id);
#endif
