#include "acoral.h"
void hal_board_init(void){
 	hal_cpu_init();
}
void hal_cpu_init(){
	EnableDCache();
	EnableICache();
	hal_scu_enable();
}
