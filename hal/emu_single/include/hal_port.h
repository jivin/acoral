#ifndef HAL_PORT_H
#define HAL_PORT_H
#include<autocfg.h>
#include<type.h>

//内存相关的硬件抽象接口
extern acoral_u32 heap_start;
extern acoral_u32 heap_end;
#define HAL_HEAP_START heap_start
#define HAL_HEAP_END heap_end
#define HAL_MEM_INIT() hal_mem_init()

//线程相关的硬件抽象接口
#define HAL_STACK_INIT(stack,route,exit,args) hal_stack_init(stack,route,exit,args)
#define HAL_INTR_SWITCH_TO(next) hal_switch_to(next)
#define HAL_SWITCH_TO(next) hal_switch_to(next)
#define HAL_INTR_CTX_SWITCH(prev,next) hal_intr_ctx_switch(prev,next)

//中断相关的硬件抽象接口
#define SIGUSER1 10
#define HAL_INTR_MIN SIGUSER1
#define HAL_INTR_NUM 1
#define HAL_INTR_MAX HAL_INTR_MIN+HAL_INTR_NUM-1
#define HAL_INTR_INIT() hal_intr_init()
#define HAL_INTR_DISABLE()   hal_intr_disable()
#define HAL_INTR_ENABLE()   hal_intr_enable()
#define HAL_INTR_DISABLE_SAVE()  hal_intr_disable_save()
#define HAL_INTR_RESTORE(sr)   hal_intr_restore(sr)

//ticks时钟相关的硬件抽象接口
#define HAL_TICKS_INTR HAL_INTR_MIN
#define HAL_TICKS_INIT() hal_ticks_init()

#define HAL_BOARD_INIT() 
#endif
