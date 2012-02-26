#include <type.h>
#include <hal.h>
#include <spinlock.h>
#include <print.h>
acoral_u32 adr_start;
acoral_u32 adr_end;
acoral_u32 mem_size;
acoral_spinlock_t slate_lock;
acoral_32 slate_init(acoral_u32 start, acoral_u32 end)
{
	start=(start+3)&~3;
	end=end&~3;
	adr_start = start;
	adr_end = end;
        mem_size=end-start;	
	acoral_spin_init(&slate_lock);
}

acoral_u32 slate_malloc_size(acoral_u32 size){
	acoral_size tmp=(size+3)&~3;
	return tmp<mem_size?tmp:mem_size;
}

void* slate_malloc(acoral_u32  size)
{
  	acoral_u32 tmp;
  	acoral_sr cpu_sr;
	acoral_u32 count=(size+3)&~3;
	HAL_ENTER_CRITICAL();
	acoral_spin_lock(&slate_lock);
	tmp=adr_start;
	if(tmp+count>=adr_end)
		return NULL;
	adr_start+=count;
	acoral_spin_unlock(&slate_lock);
	HAL_ENTER_CRITICAL();
	return (void *)tmp;
}

void slate_free(void *p)
{
}

void slate_scan(void *p)
{
	acoral_print("Free Mem Byte:%d\n",adr_end-adr_start); 	
}
