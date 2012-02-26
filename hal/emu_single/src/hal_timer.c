#include<hal.h>
#include<core.h>
#include<unistd.h>
#include<signal.h>
/*****************************************/
/*****这个函数的作用是初始化滴答时钟数据**/
/**********相关数据*********************/
/*****************************************/
void hal_ticks_init(){
	pid_t pid;
	if (fork()==0){ /*初始化滴答时钟的时间间隔*/
	  /*下面这个模拟了时钟中断*/
	    pid=getppid();
	    while(1){
			usleep(1000000/ACORAL_TICKS_PER_SEC);
	      	kill(pid,HAL_TICKS_INTR); /*激发中断*/
	    }
	}

}
