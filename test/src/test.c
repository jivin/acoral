#include"../include/test.h"
APP_CALL_START(app_call_start)
APP_CALL_END(app_call_end)
void acoral_app_call(void){
	initcall_t *call;
	for (call =&app_call_start,call++;call <&app_call_end; call++)
		(*call)();
}

void test_init(){
	acoral_app_call();
}
