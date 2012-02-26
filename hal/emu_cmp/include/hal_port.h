#ifndef HAL_PORT_H
#define HAL_PORT_H
#include "autocfg.h"
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
#define HAL_INTR_NUM 3
#define HAL_INTR_MAX HAL_INTR_MIN+HAL_INTR_NUM-1
#define HAL_INTR_INIT() hal_intr_init()
#define HAL_INTR_DISABLE()   hal_intr_disable()
#define HAL_INTR_ENABLE()   hal_intr_enable()
#define HAL_INTR_DISABLE_SAVE()  hal_intr_disable_save()
#define HAL_INTR_RESTORE(sr)   hal_intr_restore(sr)

//ticks时钟相关的硬件抽象接口
#define HAL_TICKS_INTR HAL_INTR_MIN
#define HAL_TICKS_INIT()

#define HAL_BOARD_INIT() 

#include "hal_atomic.h"
#ifdef CFG_CMP
#define HAL_CMD_IPI HAL_INTR_MIN+2
#define HAL_GET_CURRENT_CPU() hal_get_current_cpu()
#define HAL_CPU_IS_ACTIVE(cpu) hal_cpu_is_active(cpu)
#define HAL_PREPARE_CPUS() 
#define HAL_START_CPU(cpu) hal_start_cpu(cpu)
#define HAL_CMP_ACK() hal_cmp_ack()
#define HAL_CORE_CPU_INIT() hal_core_cpu_init()
#define HAL_FOLLOW_CPU_INIT() hal_follow_cpu_init() 
#define HAL_MOVE_SWITCH_TO(a,b,c) 
#define HAL_IPI_SEND(cpu,vector) hal_ipi_send(cpu,vector)
#define HAL_IPI_SEND_CPUS(cpulist,vector) 
#define HAL_IPI_SEND_ALL(vector) 
#include "hal_spinlock.h"
#endif

#endif
