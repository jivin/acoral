#include<autocfg.h>
void brd_driver_init(){
#ifdef CFG_DRV_CONSOLE
	console_init();
#endif
#ifdef CFG_DRV_EMU_DISK
	disk_init();
#endif
}
