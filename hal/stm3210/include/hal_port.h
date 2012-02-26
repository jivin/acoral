#ifndef HAL_PORT_H
#define HAL_PORT_H
#include "autocfg.h"
#include "type.h"
#define HAL_STACK_INIT(stack,route,exit,args) hal_stack_init(stack,route,exit,args)

#define HAL_INTR_MIN            15
#define HAL_INTR_NUM   44
#define HAL_INTR_MAX    HAL_INTR_MIN+HAL_INTR_NUM-1
#define HAL_INTR_INIT() hal_intr_init()

#define EXP_SYSTICK  HAL_INTR_MIN+0
#define HAL_TICKS_INTR EXP_SYSTICK
#define HAL_TICKS_INIT() hal_ticks_init()
#define HAL_TICKS_DELAY() hal_ticks_delay()

extern acoral_u32 heap_start[];
extern acoral_u32 heap_end[];
#define HAL_HEAP_START heap_start
#define HAL_HEAP_END heap_end
#define HAL_MEM_INIT()

#define HAL_BOARD_INIT() hal_board_init()
#endif
