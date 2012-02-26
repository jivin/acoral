#include<acoral.h>
#include<sig.h>
extern void (*HAL_INTR_ENTRY)(acoral_vector vector);
acoral_u32 IRQ_stack[HAL_MAX_CPU];
acoral_bool mask_flag[HAL_MAX_CPU];
sigset_t initmask;
void hal_intr_mask(acoral_vector vector){
	acoral_u32 cpu=acoral_current_cpu;
	mask_flag[cpu]=1;
}

void hal_intr_unmask(acoral_vector vector){
	acoral_u32 cpu=acoral_current_cpu;
	mask_flag[cpu]=0;
}

void hal_intr_enable(){
	acoral_u32 cpu=acoral_current_cpu;
	if(!mask_flag[cpu])
		sigprocmask(SIG_UNBLOCK,&initmask,NULL);
}

void hal_intr_disable(){
	sigprocmask(SIG_BLOCK,&initmask,NULL);
}

void intr_special(acoral_u32 sp){
	acoral_u32 cpu;
	cpu=acoral_current_cpu;
	IRQ_stack[cpu]=sp;
}

acoral_sr hal_intr_disable_save(){
	acoral_intr_disable();
	return 0;
}

void hal_intr_restore(acoral_sr sr){
    acoral_intr_enable();
}

void hal_intr_init(){
	acoral_u32 i;
	sigemptyset(&initmask);
	sigaddset(&initmask,HAL_TICKS_INTR);
#ifdef CFG_CMP
	sigaddset(&initmask,HAL_CMD_IPI);
#endif
	for(i=0;i<HAL_MAX_CPU;i++){
		mask_flag[i]=0;
	}
	for(i=HAL_INTR_MIN;i<=HAL_INTR_MAX;i++){
		acoral_set_intr_enter(i,NULL);
		acoral_set_intr_exit(i,NULL);
		acoral_set_intr_mask(i,hal_intr_mask);
		acoral_set_intr_unmask(i,hal_intr_unmask);
		if(signal(i,&HAL_INTR_ENTRY)==SIG_ERR)
			acoral_print("hal_entry_err");
	}
}
