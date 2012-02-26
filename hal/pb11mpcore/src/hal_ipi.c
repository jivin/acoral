#include "acoral.h"
void hal_ipi_send(acoral_u32 cpu,acoral_vector vector){
	*(volatile acoral_u32 *)0x1f001f00=1<<(cpu+16)|vector;
}

void hal_ipi_send_all(acoral_vector vector){
	*(volatile acoral_u32 *)0x1f001f00=0x01<<24|vector;
}
