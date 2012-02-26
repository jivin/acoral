#include "../include/common.h"
#include "../include/blockheap.h"
#include "../include/rect.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/gmessage.h"
#include "../include/hdc.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/lguiapp.h"
#include "../include/gtimer.h"

static WndCaptureMouse tmp;
PWndCaptureMouse _lGUI_wndCaptureMouse=&tmp;
extern char* _lGUI_pShm;	

void PortSetCurStat(PlGUIAppStat stat){
	acoral_cur_thread->data=malloc(sizeof(win_data));
	((win_data *)acoral_cur_thread->data)->curStat=stat;
	((win_data *)acoral_cur_thread->data)->curWin=NULL;
}

PlGUIAppStat PortGetCurStat(thread_t id){
	acoral_thread_t *thread;
	thread=(acoral_thread_t *)acoral_get_res_by_id(id);
	return ((win_data *)thread->data)->curStat;
}

void PortExit(){
	free(acoral_cur_thread->data);
}

BOOL port_shm_server_init(){
	_lGUI_pShm=bmalloc(SHMEM_SIZE);
	return true;
}

BOOL port_shm_server_exit(){
	bfree(_lGUI_pShm);
	return true;
}

BOOL port_shm_client_exit(){
	return true;
}

BOOL port_shm_client_init(){
	return true;
}

void PortServerExit(){
	
}

void PortClientExit(){
	PortExit();
}

void TimerServer(void *args){
	void (*route)();
	while(1){
		route=(void (*)())args;
		acoral_delay_self(GROUTETIME);
		route();
	}
}

void PortTimerAttach(void (*route)()){
	thread_t id;
	pthread_create(&id,NULL,
			TimerServer,(void *)route);
}

