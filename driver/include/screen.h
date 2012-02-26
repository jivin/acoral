#ifndef ACORAL_SCREEN_H
#define ACORAL_SCREEN_H
#include<type.h>
#define CMD_SCREEN_INFO 1
typedef struct{
	acoral_u32 yres;
	acoral_u32 xres;	
	acoral_u32 bits_per_pixel;	
	acoral_char *screen_buf;	
}acoral_screen_info;
#endif
