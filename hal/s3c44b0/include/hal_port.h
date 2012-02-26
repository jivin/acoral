#ifndef HAL_PORT_H
#define HAL_PORT_H

#define HAL_STACK_INIT(stack,route,exit,args) hal_stack_init(stack,route,exit,args)

#define HAL_INTR_MIN            0
#define HAL_INTR_NUM			26
#define HAL_INTR_MAX HAL_INTR_MIN+HAL_INTR_NUM-1
#define HAL_INTR_INIT()	hal_intr_init()

#define IRQ_TIMER0	 	HAL_INTR_MIN+13
#define HAL_TICKS_INTR IRQ_TIMER0
#define HAL_TICKS_INIT() hal_ticks_init()
#define HAL_TICKS_DELAY() hal_ticks_delay()

extern acoral_u32 heap_start[];
extern acoral_u32 heap_end[];
#define HAL_HEAP_START heap_start
#define HAL_HEAP_END heap_end
#define HAL_MEM_INIT()

#define HAL_BOARD_INIT()
#endif
