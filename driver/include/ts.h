#ifndef ACORAL_TS_H
#define ACORAL_TS_H
#include<type.h>
#define ACORAL_TS_DOWN 1
#define ACORAL_TS_UP 2
typedef struct{
	acoral_u32 x;
	acoral_u32 y;	
	acoral_u8 event;	
}acoral_ts_info;
#endif
