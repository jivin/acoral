#include<hal.h>
#include<kernel.h>
#include<signal.h>
extern void (*HAL_INTR_ENTRY)(acoral_vector vector);
sigset_t initmask;
acoral_bool mask_flag;
void hal_intr_mask(acoral_vector vector){
		mask_flag=1;
}

void hal_intr_unmask(acoral_vector vector){
		mask_flag=0;
}

void hal_intr_enable(){
	if(!mask_flag);
		sigprocmask(SIG_UNBLOCK,&initmask,NULL);
}

void hal_intr_disable(){
     sigprocmask(SIG_BLOCK,&initmask,NULL);
}

acoral_sr hal_intr_disable_save(){
	hal_intr_disable();
	return 0;
}

void hal_intr_restore(acoral_sr sr){
   	hal_intr_enable();
}

void hal_intr_init(){
	acoral_u32 i;
	mask_flag=0;
	sigemptyset(&initmask);
	sigaddset(&initmask,HAL_TICKS_INTR);
	for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		acoral_set_intr_enter(i,NULL);
		acoral_set_intr_exit(i,NULL);
		acoral_set_intr_mask(i,hal_intr_mask);
		acoral_set_intr_unmask(i,hal_intr_unmask);
		if(signal(i,&HAL_INTR_ENTRY)==SIG_ERR)
			acoral_print("hal_entry_err");
	}
}


