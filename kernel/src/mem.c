#include <autocfg.h>
#include<type.h>
#include<hal.h>
#include<mem.h>
/*================================
 *   memory system initialize
 *	内存管理系统初始化
 *================================*/
void acoral_mem_sys_init(){
	/*硬件相关的内存初始化，比如内存控制器等*/
  	HAL_MEM_INIT();
	/*堆初始化,这个可以选择不同管理系统，比如buddy内存管理等*/
  	acoral_mem_init((acoral_u32)HAL_HEAP_START,(acoral_u32)HAL_HEAP_END);
#ifdef CFG_MEM2
  	acoral_mem_init2();
#endif
}
