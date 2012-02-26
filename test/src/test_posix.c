#include "acoral.h"
#include"../include/test.h"
ACORAL_POSIX_THREAD test(void *args){
	while(1){
		acoral_print("Posix%d\n",args);
		acoral_soft_delay(5);
	}
}
ACORAL_POSIX_THREAD test1(void *args){
	while(1){
		acoral_prints("Posix Delay\n");
		acoral_delay_self(500);
	}
}
void posix_init()
{
	acoral_u32 i;
	for(i=0;i<6;i++){
		acoral_posix_create(NULL,NULL,test,i);
	}
	acoral_posix_create(NULL,NULL,test1,0);
}

TEST_CALL(posix_init)
