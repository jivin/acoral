#include<type.h>
#include<hal.h>
#include<queue.h>
#include<mem.h>
#include<resource.h>
#include<print.h>
#include<core.h>
acoral_pool_t acoral_pools[ACORAL_MAX_POOLS];
acoral_pool_t *acoral_free_res_pool;
/*================================
 *    create a kind of resource pool
 *       创建某一资源内存池
 *       pool_ctrl---资源内存池管理块
 *================================*/
acoral_err acoral_create_pool(acoral_pool_ctrl_t *pool_ctrl){
  	acoral_pool_t *pool;
	if(pool_ctrl->num>=pool_ctrl->max_pools)
	    return ACORAL_RES_MAX_POOL;
  	pool=acoral_get_free_pool();
	if(pool==NULL)
		return ACORAL_RES_NO_POOL;
	pool->id=pool_ctrl->type<<ACORAL_RES_TYPE_BIT|pool->id;
	pool->size=pool_ctrl->size;
	pool->num=pool_ctrl->num_per_pool;
	pool->base_adr=(void *)acoral_malloc(pool->size*pool->num);
	if(pool->base_adr==NULL)
		return ACORAL_RES_NO_MEM;
	pool->res_free=pool->base_adr;
	pool->free_num=pool->num;
	pool->ctrl=pool_ctrl;
	acoral_pool_res_init(pool);
        acoral_list_add2_tail(&pool->ctrl_list,pool_ctrl->pools);
	acoral_list_add2_tail(&pool->free_list,pool_ctrl->free_pools);
	pool_ctrl->num++;
	return 0;
}

/*================================
 *      release a kind of resource pool
 *       释放某一资源内存池所有资源
 *       pool_ctrl--资源内存池管理块
 *================================*/
void acoral_release_pool(acoral_pool_ctrl_t *pool_ctrl){
	acoral_pool_t *pool;
	acoral_list_t *list,*head;
	head=pool_ctrl->pools;
	if(acoral_list_empty(head))
	  	return;
   for(list=head->next;list!=head;list=list->next){
			pool=list_entry(list,acoral_pool_t,free_list);
		    acoral_list_del(&pool->ctrl_list);
			acoral_list_del(&pool->free_list);
			acoral_free(pool->base_adr);
			pool->base_adr=(void *)acoral_free_res_pool;
			pool->id=pool->id&ACORAL_POOL_INDEX_MASK;
			acoral_free_res_pool=pool;
	}
}

/*================================
 *    collect a kind of resource pool
 *       回收某一资源内存池空闲的内存池
 *       pool_ctrl--资源内存池管理块
 *================================*/
void acoral_collect_pool(acoral_pool_ctrl_t *pool_ctrl){
	acoral_pool_t *pool;
	acoral_list_t *list,*head;
	head=pool_ctrl->free_pools;
	if(acoral_list_empty(head))
	  	return;
        for(list=head->next;list!=head;list=list->next){
			pool=list_entry(list,acoral_pool_t,free_list);
	       	if(pool->free_num==pool->num){
		      	acoral_list_del(&pool->ctrl_list);
				acoral_list_del(&pool->free_list);
				acoral_free(pool->base_adr);
				pool->base_adr=(void *)acoral_free_res_pool;
				pool->id=pool->id&ACORAL_POOL_INDEX_MASK;
				acoral_free_res_pool=pool;
			}
	}
}

/*================================
 *    get a kind of resource 
 *       获取某一资源
 *       pool_ctrl--资源池管理块
 *================================*/
acoral_res_t *acoral_get_res(acoral_pool_ctrl_t *pool_ctrl){
	acoral_sr cpu_sr;
  	acoral_list_t *first;
	acoral_res_t *res;
	acoral_pool_t *pool;
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&pool_ctrl->lock);
	first=pool_ctrl->free_pools->next;
	if(acoral_list_empty(first)){
	  	if(acoral_create_pool(pool_ctrl)){
			acoral_spin_unlock(&pool_ctrl->lock);
			HAL_EXIT_CRITICAL();
			return NULL;
		}
		else{
			first=pool_ctrl->free_pools->next;
		}
	}
	pool=list_entry(first,acoral_pool_t,free_list);
	res=(acoral_res_t *)pool->res_free;
	pool->res_free=(void *)((acoral_u8 *)pool->base_adr+res->next_id*pool->size);
	res->id=(res->id>>(ACORAL_RES_INDEX_INIT_BIT-ACORAL_RES_INDEX_BIT))&ACORAL_RES_INDEX_MASK|pool->id;
        pool->free_num--;
#ifdef CFG_TEST
	acoral_print("Alloc res 0x%x\n",res);
#endif
	if(!pool->free_num){
	  	acoral_list_del(&pool->free_list);
	}
	acoral_spin_unlock(&pool_ctrl->lock);
	HAL_EXIT_CRITICAL();
	return res;
}

/*================================
 *    release a kind of resource 
 *       释放某一资源
 *      res--资源数据块
 *================================*/
void acoral_release_res(acoral_res_t *res){
  	acoral_pool_t *pool;
	acoral_u32 index;
  	void *tmp;
       	acoral_pool_ctrl_t *pool_ctrl;
	if(!ACORAL_ASSERT(res,"Res Release\n"))
		return;
	pool=acoral_get_pool_by_id(res->id);
	if(pool==NULL){
		acoral_printerr("Res release Err\n");
		return;
	}
 	pool_ctrl=pool->ctrl;
	acoral_spin_lock(&pool_ctrl->lock);
	if((void *)res<pool->base_adr){
		acoral_printerr("Err Res\n");
		return;
	}
	index=(((acoral_u32)res-(acoral_u32)pool->base_adr)/pool->size);
	if(index>=pool->num){
		acoral_printerr("Err Res\n");
		return;
	}
#ifdef CFG_TEST
	acoral_print("Free res 0x%x\n",res);
#endif
	tmp=pool->res_free;
	pool->res_free=(void *)res;
	res->id=index<<ACORAL_RES_INDEX_INIT_BIT;
	res->next_id=((acoral_res_t *)tmp)->id>>ACORAL_RES_INDEX_INIT_BIT;
	pool->free_num++;
	if(acoral_list_empty(&pool->free_list))
	  	acoral_list_add(&pool->free_list,pool_ctrl->free_pools);
	acoral_spin_unlock(&pool_ctrl->lock);
	return;
}

/*================================
 *    get a kind of resource by res_id
 *  根据资源ID获取某一资源对应的资源池
 *      res_id--资源ID
 *================================*/
acoral_pool_t *acoral_get_pool_by_id(acoral_id res_id){
  	acoral_u32 index;
	index=(res_id&ACORAL_POOL_INDEX_MASK)>>ACORAL_POOL_INDEX_BIT;
	if(index<ACORAL_MAX_POOLS)
	  	return acoral_pools+index;
	return NULL;
}


/*================================
 *    get a free pool
 *     获取空闲资源池
 *================================*/
acoral_pool_t *acoral_get_free_pool(){
	acoral_sr cpu_sr;
  	acoral_pool_t *tmp;
	HAL_ENTER_CRITICAL();
	tmp=acoral_free_res_pool;
	if(NULL!=tmp){
		acoral_spin_lock(&tmp->lock);
		acoral_free_res_pool=*(void **)tmp->base_adr;
		acoral_spin_unlock(&tmp->lock);
	}
	HAL_EXIT_CRITICAL();
	return tmp;
}

acoral_res_t * acoral_get_res_by_id(acoral_id id){
  	acoral_pool_t *pool;
	acoral_u32 index;
	pool=acoral_get_pool_by_id(id);
	if(pool==NULL)
		return NULL;
	index=(id&ACORAL_RES_INDEX_MASK)>>ACORAL_RES_INDEX_BIT;
     	return (acoral_res_t *)((acoral_u8 *)pool->base_adr+index*pool->size);
}



/*================================
 *       a kind of resource initial 
 *        资源初始化
 *================================*/
void acoral_pool_res_init(acoral_pool_t * pool){
	acoral_res_t *res;
	acoral_u32 i;
	acoral_u8 *pblk;
	acoral_u32 blks;
	blks=pool->num;
	res=(acoral_res_t *)pool->base_adr;
     	pblk=(acoral_u8 *)pool->base_adr + pool->size;
	for (i = 0; i < (blks - 1); i++) {
	    res->id=i<<ACORAL_RES_INDEX_INIT_BIT;
	    res->next_id=i+1;
	    res=(acoral_res_t *)pblk;
	    pblk+=pool->size;
	}
	res->id=blks-1<<ACORAL_RES_INDEX_INIT_BIT;
	res->next_id=0;
}

/*================================
 *        resource pool initial 
 *              资源池初始化
 *================================*/
void  acoral_pools_init(void)
{
    acoral_pool_t  *pool;
    acoral_u32 i;
    pool = &acoral_pools[0];
    for (i = 0; i < (ACORAL_MAX_POOLS - 1); i++) {
        pool->base_adr= (void *)&acoral_pools[i+1];
		pool->id=i;
        pool++;
		acoral_spin_init(&pool->lock);
    }
    pool->base_adr= (void *)0;
    acoral_free_res_pool = &acoral_pools[0];
	acoral_spin_init(&acoral_free_res_pool->lock);
}

/*================================
 *        resource pool control initial 
 *              资源池控制块初始化
 *================================*/
void acoral_pool_ctrl_init(acoral_pool_ctrl_t *pool_ctrl)
{
	acoral_u32 size;
	pool_ctrl->free_pools=&pool_ctrl->list[0];
	pool_ctrl->pools=&pool_ctrl->list[1];
	pool_ctrl->num=0;
	acoral_init_list(pool_ctrl->pools);
    acoral_init_list(pool_ctrl->free_pools);
    acoral_spin_init(&pool_ctrl->lock);
	/*调整pool的对象个数以最大化利用分配了的内存*/
	size=acoral_malloc_size(pool_ctrl->size*pool_ctrl->num_per_pool);
	if(size<pool_ctrl->size){
		pool_ctrl->num_per_pool=0;
	}
	else{
		pool_ctrl->num_per_pool=size/pool_ctrl->size;
		acoral_create_pool(pool_ctrl);
	}
}

/*================================
 *        user resource api
 *         用户资源相关接口
 *================================*/

void acoral_obj_pool_init(acoral_pool_ctrl_t *pool_ctrl)
{
	pool_ctrl->type=ACORAL_RES_USER;
	pool_ctrl->size+=sizeof(acoral_res_t);
	acoral_pool_ctrl_init(pool_ctrl);
}

void acoral_obj_pool_release(acoral_pool_ctrl_t *pool_ctrl){
	 acoral_release_pool(pool_ctrl);
}

void *acoral_get_obj(acoral_pool_ctrl_t *pool_ctrl){
	return (void *)((acoral_char *)acoral_get_res(pool_ctrl)+sizeof(acoral_res_t));
}

void acoral_free_obj(void *obj){
	acoral_release_res((acoral_char *)obj-sizeof(acoral_res_t));
}

/*================================
 *        resource  system initial 
 *          资源系统初始化
 *================================*/
void acoral_res_sys_init(){
	acoral_pools_init();
}

acoral_char acoral_assert_res(acoral_res_t *res,acoral_8 *assert){
	if(res==NULL||acoral_get_res_by_id(res->id)!=res){
		acoral_printerr("%s\n",assert);
		acoral_printerr("Err res's id:%x\n",res->id);
		return false;
	}
	return true;
}
