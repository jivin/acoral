#ifndef ACORAL_HAL_CORE_H
#define ACORAL_HAL_CORE_H
#define HAL_STACK_INIT(stack,route,exit,args) hal_stack_init(stack,route,exit,args)
typedef struct {
  	acoral_u32 cpsr;
	acoral_u32 r0;
	acoral_u32 r1;
	acoral_u32 r2;
	acoral_u32 r3;
	acoral_u32 r4;
	acoral_u32 r5;
	acoral_u32 r6;
	acoral_u32 r7;
	acoral_u32 r8;
	acoral_u32 r9;
	acoral_u32 r10;
	acoral_u32 r11;
	acoral_u32 r12;
	acoral_u32 lr;
	acoral_u32 pc;
}hal_ctx_t;
#endif
