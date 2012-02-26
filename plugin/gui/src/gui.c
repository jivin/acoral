#include"../include/common.h"
extern void server(void *args);
void acoral_gui_init(){
	thread_t id;
	pthread_create(&id,NULL,
			server,NULL);
}
