#ifndef HAL_PORT_H
#define HAL_PORT_H
#include <autocfg.h>
#include <type.h>

//内存相关的硬件抽象接口
extern acoral_u32 heap_start[];
extern acoral_u32 heap_end[];
#define HAL_HEAP_START heap_start
#define HAL_HEAP_END heap_end
#define HAL_MEM_INIT() hal_mem_init()

//线程相关的硬件抽象接口
#define HAL_STACK_INIT(stack,route,exit,args) hal_stack_init(stack,route,exit,args)


//中断相关的硬件抽象接口
#define HAL_INTR_MIN      0
#define HAL_INTR_NUM	 50
#define HAL_INTR_MAX HAL_INTR_MIN+HAL_INTR_NUM-1
#define HAL_INTR_INIT()	hal_intr_init()

//ticks时钟相关的硬件抽象接口
#define HAL_TICKS_INTR 28
#define	HAL_TICKS_INIT() hal_ticks_init()
#define HAL_BOARD_INIT()

//功耗相关的硬件抽象接口，这个在单核下不用实现
#ifdef CFG_PM
#define HAL_SUSP_PREPARE() hal_susp_prepare()
#define HAL_SUSP_ENTER() hal_susp_enter()
#define HAL_SUSP_FINISH() hal_susp_finish()
#endif
#endif
