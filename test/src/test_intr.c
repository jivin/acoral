#include "acoral.h"
#include"../include/test.h"
acoral_u32 intr_delays[10];
acoral_u32 offset;
acoral_u32 intr_delay;
void acoral_ticks_hook(){
	acoral_u32 i;
	acoral_u32 tmp=0;
	intr_delays[offset]=HAL_TICKS_DELAY();
        for(i=0;i<10;i++)
		tmp+=intr_delays[i];
	intr_delay=tmp/10;
	offset++;
	if(offset==10)
		offset=0;
}

ACORAL_COMM_THREAD test(){
  while(1){
  	acoral_delay_self(2000);
	acoral_print("intr delay:%dus\n",intr_delay);
  }
}

void intr_init()
{
	acoral_create_thread(test, ACORAL_PRINT_STACK_SIZE,NULL,NULL,22, -1);
}

TEST_CALL(intr_init)
