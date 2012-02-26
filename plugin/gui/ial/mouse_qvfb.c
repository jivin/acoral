#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include "../include/common.h"
#include "../include/mouse_ial.h"
#define QT_VFB_MOUSE_PIPE "/tmp/.qtvfb_mouse-0"
#define NOBUTTON 0x0000
#define LEFTBUTTON 0x0001
#define RIGHTBUTTON 0x0002
#define MIDBUTTON 0x0004
#define MOUSEBUTTONMASK 0x00FF
#define MOUSE_SENSE 3
static int mouse_fd = -1;
static fd_set rfds;
static POINT mouse_pt;
static int mouse_buttons;
static int event_flag=false;

static int mouse_flag=MOUSE_SENSE;
int mouse_update (void)
{
    int ret1, ret2;
    POINT l_mouse_pt;
    int l_mouse_buttons;

    ret1 = read (mouse_fd, &l_mouse_pt, sizeof (POINT));
    ret2 = read (mouse_fd, &l_mouse_buttons, sizeof (int));

    if (ret1 == sizeof (POINT) && ret2 && sizeof (int)
            && l_mouse_buttons < 0x08) {
        memcpy(&mouse_pt,&l_mouse_pt,sizeof(POINT));
        mouse_buttons = l_mouse_buttons;
		event_flag=true;
    }
    else
        return 0;

    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = mouse_pt.x;
    *y = mouse_pt.y;
	event_flag=false;
}
static int DownFlag=0;
static int mouse_getbutton (void)
{
    int buttons = 0;
    
	if(mouse_buttons==0){
		if(DownFlag){
			buttons =MOUSE_EVENT_LBUTTON_UP ;
			DownFlag=0;
		}
	//	else buttons=MOUSE_EVENT_MOVE;
	}else{
		if (mouse_buttons & LEFTBUTTON){
			buttons =MOUSE_EVENT_LBUTTON_DOWN ;
			DownFlag=1;
		}
	}
    return buttons;
}


static int 
Qvfb_Open(void)
{
    /* open mouse pipe */
    //if ((mouse_fd = open (QT_VFB_MOUSE_PIPE, O_RDONLY)) < 0) {
    if ((mouse_fd = open (QT_VFB_MOUSE_PIPE, O_NONBLOCK)) < 0) {
        printerror ("QVFB IAL engine: can not open mouse pipe.\n");
        return -1;
    }

	return 1;
}


static void 
Qvfb_Close(void)
{ 
}



static int 
Qvfb_Read(
	int *px, 
	int *py, 
	int *pb
)
{   
	if(mouse_flag<0)
		acoral_delay_self(50);
	else
		mouse_flag--;
	if(mouse_update()){			
		mouse_getxy(px,py);
		*pb = mouse_getbutton();
		mouse_flag=MOUSE_SENSE;
		return 1;
	}
	return 0;
}

MOUSEDEVICE mousedev_Qvfb = {
        Qvfb_Open,
        Qvfb_Close,
        Qvfb_Read
};
