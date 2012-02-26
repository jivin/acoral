#ifndef TEST_H
#define TEST_H
#include<type.h>
#include<autocfg.h>

#ifdef CFG_TEST
#define SECTION(name,var,value,type) \
static type var __attribute__((used,__section__(name)))=value;
typedef acoral_u32 (*initcall_t)(void);
#define __define_call(level,fn) SECTION(".acoral" level ".call",initcall##fn,fn,initcall_t)
#define __define_adr(level,var) SECTION(".acoral" level ".call",var,0,initcall_t)

#define SYS_CALL_START(var) 	__define_adr("1",var)
#define DRV_INIT(fn)   		__define_call("2",fn)
#define SYS_CALL_END(var)   	__define_adr("3",var)
#define APP_CALL_START(var) 	__define_adr("4",var)
#define APP_ENTER(fn) 	 	__define_call("5",fn)
#define APP_CALL(fn)  		__define_call("6",fn)
#define USER_CALL(fn)  		__define_call("7",fn)
#define TEST_CALL(fn)  		__define_call("8",fn)
#define APP_EXIT(fn)  		__define_call("9",fn)
#define APP_CALL_END(var)   	__define_adr("10",var)
#endif

#endif



