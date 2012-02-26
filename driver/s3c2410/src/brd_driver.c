#include<autocfg.h>
void brd_driver_init(){
#ifdef CFG_DRV_CONSOLE
	con_uart_init();
#endif
}
