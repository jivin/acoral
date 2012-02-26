#include<type.h>
#define HEAP_SIZE 10*1024*1024
acoral_u32 heap_start;
acoral_u32 heap_end;
void hal_mem_init(){
	heap_start=(acoral_u32)malloc(HEAP_SIZE);
	heap_end=heap_start+HEAP_SIZE;
}
