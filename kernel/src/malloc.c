#include<type.h>
#include<mutex.h>
#include<print.h>
#include<mem.h>
#define MEM_SIZE CFG_MEM2_SIZE
#define MAGIC 0xcc
#define MAGIC_MASK 0xfe
#define USED 1
#define FREE 0 
#define USETAG_MASK 0x1
#define SIZE_MASK 0xffffff00
#define SIZE_BIT 8
#define BLOCK_CHECK(value) (((value&MAGIC_MASK)==MAGIC))
#define BLOCK_SIZE(value) ((value&SIZE_MASK)>>SIZE_BIT)
#define BLOCK_TAG(value) (value&USETAG_MASK)
#define BLOCK_FREE(value) (BLOCK_TAG(value)==FREE)
#define BLOCK_USED(value) (BLOCK_TAG(value)==USED)
#define BLOCK_SET_USED(ptr,size) (*ptr=(size<<SIZE_BIT)|0x1|MAGIC)
#define BLOCK_SET_FREE(ptr,size) (*ptr=(size<<SIZE_BIT)|MAGIC)
#define BLOCK_CLEAR(ptr) (*ptr=0)
struct mem2_ctrl_t{
	acoral_evt_t mutex;	
	acoral_8 *top_p;
	acoral_8 *down_p;
	acoral_u32 *freep_p;
#ifdef CFG_TEST
	acoral_u32 alloc_num;
	acoral_u32 free_num;
	acoral_u32 alloc_size;
#endif
	acoral_u8 mem_state;
}mem_ctrl;

static void *r_malloc(acoral_32 size){
	acoral_u32 * tp;
	acoral_8 * ctp;
	acoral_u32 b_size;
	size=size+4;
	while(acoral_mutex_pend(&mem_ctrl.mutex, 0)!=MUTEX_SUCCED)
		acoral_suspend_self();
#ifdef CFG_TEST_MEM2
	acoral_print("...................\n");
	v_mem_scan();
#endif
#ifdef CFG_TEST
	mem_ctrl.alloc_size+=size;
	mem_ctrl.alloc_num++;
	acoral_print("In malloc:%d\n",size);
#endif
	tp=mem_ctrl.freep_p;
	ctp=(acoral_8 *)tp;
	while(ctp<mem_ctrl.top_p){
             b_size=BLOCK_SIZE(*tp);
	     if(b_size==0){
			acoral_printerr("Err address is 0x%x,size should not be 0",tp);
			acoral_mutex_post(&mem_ctrl.mutex);
			return NULL;
	     }
	     if(BLOCK_USED(*tp)||b_size<size){
        	ctp=ctp+b_size;
		tp=(acoral_u32 *)ctp;
	     }
	     else
	     {
		BLOCK_SET_USED(tp,size);
		ctp=ctp+size;
		tp=(acoral_u32 *)ctp;
		if(b_size-size>0)
			BLOCK_SET_FREE(tp,b_size-size);
		mem_ctrl.freep_p=tp;
#ifdef CFG_TEST_MEM2
		acoral_print("After malloc\n");
		v_mem_scan();
		acoral_print("...................\n");
#endif
		acoral_mutex_post(&mem_ctrl.mutex);
	        return (void *)(ctp-size+4);	
	     }
	}
    	ctp=mem_ctrl.down_p;
	tp=(acoral_u32 *)ctp;
	while(tp<mem_ctrl.freep_p){
	     b_size=BLOCK_SIZE(*tp);
	     if(b_size==0){
			acoral_printerr("Err address is 0x%x,size should not be 0",tp);
			acoral_mutex_post(&mem_ctrl.mutex);
			return NULL;
	     }
	     if(BLOCK_USED(*tp)||b_size<size){
        	ctp=ctp+b_size;
		tp=(acoral_u32 *)ctp;
	     }
	     else
	     {
		BLOCK_SET_USED(tp,size);
		ctp=ctp+size;
		tp=(acoral_u32 *)ctp;
		if(b_size-size>0)
			BLOCK_SET_FREE(tp,b_size-size);
		mem_ctrl.freep_p=tp;
#ifdef CFG_TEST_MEM2
		acoral_print("After malloc\n");
		v_mem_scan();
		acoral_print("...................\n");
#endif
		acoral_mutex_post(&mem_ctrl.mutex);
	        return (void *)(ctp-size+4);	
	     }
    	 }
#ifdef CFG_TEST_MEM2
	acoral_print("After malloc\n");
	v_mem_scan();
	acoral_print("...................\n");
#endif
#ifdef CFG_TEST
	mem_ctrl.alloc_size-=size;
#endif
	acoral_mutex_post(&mem_ctrl.mutex);
	return NULL;
}

void * v_malloc(acoral_32 size){
	if(mem_ctrl.mem_state==0)
		return NULL;
	size=(size+3)&~3;
    return r_malloc(size);
}

void v_free(void * p){
	acoral_u32 * tp,*prev_tp;
	acoral_8 * ctp;
	acoral_u32 b_size,size;
	if(mem_ctrl.mem_state==0)
		return;
	p=(acoral_8 *)p-4;
	tp=(acoral_u32 *)p;
	while(acoral_mutex_pend(&mem_ctrl.mutex, 0)!=0) //周期性任务
		acoral_suspend_self();
	if(p==NULL||(acoral_8 *)p<mem_ctrl.down_p||(acoral_8 *)p>=mem_ctrl.top_p||!BLOCK_CHECK(*tp)){
		acoral_printerr("Invalide Free address:0x%x\n",tp);
		return;
	}
	if(BLOCK_FREE(*tp)){
		acoral_printerr("Address:0x%x have been freed\n",tp);
		return;
	}
	prev_tp=tp;
	ctp=(acoral_8 *)tp;
    b_size=BLOCK_SIZE(*tp);
#ifdef CFG_TEST
	mem_ctrl.alloc_size-=b_size;
	mem_ctrl.free_num++;
	acoral_print("In free:%d\n",b_size);
#endif
#ifdef CFG_TEST_MEM2
	acoral_print("...................\n");
	acoral_print("Before free\n");
	v_mem_scan();
#endif
   	ctp=ctp+b_size;
	tp=(acoral_u32 *)ctp;
	if(BLOCK_FREE(*tp)){
		size=BLOCK_SIZE(*tp);
		if(size==0){
			acoral_printerr("Err address is 0x%x,size should not be 0",tp);
			acoral_mutex_post(&mem_ctrl.mutex);
			return;
		}
		b_size+=size;
		BLOCK_CLEAR(tp);
	}
	BLOCK_SET_FREE(prev_tp,b_size);
	mem_ctrl.freep_p=prev_tp;
	if(p==mem_ctrl.down_p){
#ifdef CFG_TEST_MEM2
		acoral_print("After free\n");
		v_mem_scan();
		acoral_print("...................\n");
#endif
		acoral_mutex_post(&mem_ctrl.mutex);
		return;
	}
	ctp=mem_ctrl.down_p;
	tp=(acoral_u32 *)ctp;
	while(ctp<(acoral_8 *)p){
	     size=BLOCK_SIZE(*tp);
	     if(size==0){
			acoral_printerr("err address is 0x%x,size should not be 0",tp);
			acoral_mutex_post(&mem_ctrl.mutex);
			return;
             }
	     ctp=ctp+size;
	     prev_tp=tp;
	     tp=(acoral_u32 *)ctp;
	}
	if(BLOCK_FREE(*prev_tp)){
		tp=(acoral_u32*)p;
		BLOCK_CLEAR(tp);
		BLOCK_SET_FREE(prev_tp,b_size+size);
		mem_ctrl.freep_p=prev_tp;
	}
#ifdef CFG_TEST_MEM2
	acoral_print("After free\n");
	v_mem_scan();
	acoral_print("...................\n");
#endif
	acoral_mutex_post(&mem_ctrl.mutex);
}

void v_mem_init(){
	acoral_size size;
	size=acoral_malloc_size(MEM_SIZE);
	mem_ctrl.down_p=(acoral_8 *)acoral_malloc(size);
	if(mem_ctrl.down_p==NULL){
		mem_ctrl.mem_state=0;
		return;
	}
	else{
		mem_ctrl.mem_state=1;
	}
	acoral_mutex_init(&mem_ctrl.mutex,0);
	mem_ctrl.top_p=mem_ctrl.down_p+size;
	mem_ctrl.freep_p=(acoral_u32 *)mem_ctrl.down_p;
	BLOCK_SET_FREE(mem_ctrl.freep_p,size);
#ifdef CFG_TEST
	mem_ctrl.alloc_size=0;
	mem_ctrl.alloc_num=0;
	mem_ctrl.free_num=0;
#endif
}

void v_mem_scan(void){
        acoral_8 * ctp;
	acoral_u32 * tp;
	acoral_u32 size;
	if(mem_ctrl.mem_state==0){
		acoral_print("Mem Init Err ,so no mem space to malloc\r\n");
		return;
	}
	ctp=mem_ctrl.down_p;
	do{
		tp=(acoral_u32 *)ctp;
		size=BLOCK_SIZE(*tp);
		if(size==0){
			acoral_print("Err address is 0x%x,size should not be 0\r\n",tp);
			break;
		}
		if(BLOCK_USED(*tp)){
			acoral_print("The address is 0x%x,the block is used and it's size is %d\r\n",tp,size);
		}
		else{
			
			acoral_print("The address is 0x%x,the block is unused and it's size is %d\r\n",tp,size);
		}
		ctp=ctp+size;
	}while(ctp<mem_ctrl.top_p);
#ifdef CFG_TEST
	acoral_print("Alloc Size:%d  Free_num:%d  alloc_num:%d\r\n",mem_ctrl.alloc_size,mem_ctrl.free_num,mem_ctrl.alloc_num);
#endif
}

