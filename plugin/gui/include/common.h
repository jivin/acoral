/*
	Copyright (C) 2004-2005 Li Yudong
*/
/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "gui_port.h"

#define GUIAPI
#define LGUIAPI

//#define BUFFTYPE_8BPP		//256ɫ		8 bit per point
#define BUFFTYPE_16BPP		//64kɫ		16bit per point
//#define BUFFTYPE_24BPP		//16Mɫ		24bit per point
//#define BUFFTYPE_32BPP		//4096Mɫ	24bit per point

#define SHMEM_SIZE	1024*1024


#define SHMEM_MOUSE_OFFSET_X			0
#define SHMEM_MOUSE_OFFSET_Y			4

#define SHMEM_MOUSE_WIDTH_OFFSET	8
#define SHMEM_MOUSE_HEIGHT_OFFSET	12

#define SHMEM_CURSORBOX_OFFSET		16	
#define SHMEM_CURSORBOX_SIZE			1008	

#define SHMEM_SAVEBOX_OFFSET			SHMEM_CURSORBOX_OFFSET + SHMEM_CURSORBOX_SIZE	
#define SHMEM_SAVEBOX_SIZE			1008	

#define SHMEM_FONT_16_16_OFFSET		SHMEM_SAVEBOX_OFFSET + SHMEM_SAVEBOX_SIZE
#define SHMEM_FONT_16_16_SIZE			245736 

#define SHMEM_FONT_14_14_OFFSET		SHMEM_FONT_16_16_OFFSET + SHMEM_FONT_16_16_SIZE
#define SHMEM_FONT_14_14_SIZE			215024

#define SHMEM_FONT_12_12_OFFSET		SHMEM_FONT_14_14_OFFSET + SHMEM_FONT_14_14_SIZE
#define SHMEM_FONT_12_12_SIZE			184312

#define SHMEM_GDI_INDEX_OFFSET		SHMEM_FONT_12_12_OFFSET + SHMEM_FONT_12_12_SIZE
#define SHMEM_GDI_OFFSET				SHMEM_FONT_12_12_OFFSET + SHMEM_FONT_12_12_SIZE + 0x00000040


//CONST DEFINE

extern acoral_u32 SCREEN_HEIGHT;
extern acoral_u32 SCREEN_WIDTH;

#define DESKICON_HEIGHT			48
#define DESKICON_WIDTH				48

#define DESKICON_BMP_HEIGHT		32
#define DESKICON_BMP_WIDTH		32


#define DESK_TAB_HEIGHT			20
#define DESK_TASKBAR_HEIGHT		20
#define DESK_CLIENT_HEIGHT			280

//TAB
#define DESK_TAB_LEFT				0
#define DESK_TAB_RIGHT				SCREEN_WIDTH - 1
#define DESK_TAB_TOP				0
#define DESK_TAB_BOTTOM				DESK_TAB_HEIGHT - 1
//client area
#define DESK_CLIENT_LEFT			0
#define DESK_CLIENT_RIGHT			SCREEN_WIDTH - 1
#define DESK_CLIENT_TOP				DESK_TAB_HEIGHT
#define DESK_CLIENT_BOTTOM			SCREEN_HEIGHT - DESK_TASKBAR_HEIGHT - 1
//taskbar
#define DESK_TASKBAR_LEFT			0
#define DESK_TASKBAR_RIGHT			SCREEN_WIDTH-1
#define DESK_TASKBAR_TOP			SCREEN_HEIGHT - DESK_TASKBAR_HEIGHT 
#define DESK_TASKBAR_BOTTOM			SCREEN_HEIGHT-1

#define WIN_CLOSEBOX_WIDTH			14
#define WIN_CLOSEBOX_HEIGHT			14
#define WIN_CLOSEBOX_FILESPEC_0		"close.bmp"
#define WIN_CLOSEBOX_FILESPEC_1		"close_p.bmp"

#define WIN_OKBUTTON_WIDTH			28
#define WIN_OKBUTTON_HEIGHT			14	
#define WIN_OKBUTTON_FILESPEC_0		"okbut.bmp"
#define WIN_OKBUTTON_FILESPEC_1		"okbut_p.bmp"

#define WIN_CAPTION_HEIGHT			14
#define WIN_CAPTION_ACTIVE_COLOR	RGB(0,20,120)
#define WIN_CAPTION_DISACTIVE_COLOR	RGB(32,94,201)
#define WIN_BORDER_WIDTH			1
#define WIN_THINBORDER_WIDTH		2
#define WIN_THICKBORDER_WIDTH		4





#define NUM_WIN_TIMERS			16//max number of timer in a window
#define SIZE_WND_MAILBOX		1024
#define SIZE_QMSG_HEAP			1024
#define SIZE_CRGN_HEAP			1024
#define SIZE_IRGN_HEAP			1024	//invalid region private heap 
#define SIZE_IPC_MSG			1024	

#define MAXLEN_CLASSNAME		256
#define MAXLEN_MENUNAME		256
#define MAXLEN_WINCAPTION		256


#ifndef FALSE
#define FALSE       0
#endif
#ifndef TRUE
#define TRUE        1
#endif

#ifndef false
#define false       0
#endif
#ifndef true
#define true        1
#endif

#ifndef NULL
#define NULL                ((void *)0)
#endif

#ifndef null
#define null                ((void *)0)
#endif

#define ERR_INV_HWND		(void*)-1



//type define

#define LONG long
#define VOID        void

typedef int				BOOL;
typedef unsigned char		BYTE;
typedef signed char		SBYTE;
typedef unsigned short		WORD;
typedef signed short		SWORD;
typedef unsigned long		DWORD;
typedef signed long		SDWORD;
typedef unsigned int		UINT;

typedef unsigned long		UINT32;
typedef unsigned short		UINT16;
typedef unsigned char		UINT08;

typedef char*			LPCSTR;
typedef UINT  			WPARAM;
typedef DWORD			LPARAM;

//typedef int				HDC;
typedef int				HCURSOR;
typedef int				HICON;
typedef int				HINSTANCE;
typedef int				HACCEL;
typedef int				HHOOK;
typedef void				*PVOID,*LPVOID;

typedef PVOID			HWND;
typedef PVOID			HANDLE;
typedef HWND			HDLG;
typedef PVOID			HMENU;


typedef PVOID			HGDIOBJ;
typedef HGDIOBJ			HPEN;
typedef HGDIOBJ			HBRUSH;
typedef HGDIOBJ			HFONT;
typedef HGDIOBJ			HBITMAP;

typedef LONG 			LRESULT;
typedef LRESULT			(*WNDPROC)(HWND,UINT,WPARAM,LPARAM);

//typedef CONST CHAR *LPCSTR, *PCSTR;



//8bit color
#ifdef BUFFTYPE_8BPP
typedef unsigned char  BUFFTYPE;    
typedef unsigned char *PBUFFTYPE; 
#define RGB(R,G,B)  ((BUFFTYPE)(((BUFFTYPE)(B&0xC0)) | \
			                          ((BUFFTYPE)((G&0xE0)>>2)) | \
															  ((BUFFTYPE)((R&0xE0)>>5))))
#define RGBto332(rgb) (BYTE) ( ((rgb&0xE0)>>5) | ((rgb&0xE000)>>10) | ((rgb&0xC00000)>>16))

#define R(x)			((unsigned char)(x & 0xE0))
#define G(x)			((unsigned char)((x & 0x1C)<<3))
#define B(x)			((unsigned char)((x & 0x03)<<6))

#endif//BUFFTYPE_8BPP

//16bit color
#ifdef BUFFTYPE_16BPP
typedef unsigned short  BUFFTYPE;
typedef unsigned short *PBUFFTYPE; 
#define RGB(R,G,B) 	((BUFFTYPE)( ((B&0xF8)>>3) | (((WORD)G&0xFC)<<3) | (((WORD)R&0xF8)<<8)))
#define RGBto565(rgb) (BUFFTYPE)(((rgb&0xf8)>>3) | ((rgb&0xfc00) >> 5) | ((rgb&0xf80000) >> 8))


#define R(x)			((unsigned char)((x & 0xf800)>>8 ))
#define G(x)			((unsigned char)((x & 0x07e0)>>3 ))
#define B(x)			((unsigned char)((x & 0x001f)<<3 ))

#endif//BUFFTYPE_16BPP

//24bit color
#ifdef BUFFTYPE_24BPP
typedef unsigned long  BUFFTYPE;
typedef unsigned long *PBUFFTYPE; 
#define RGB(R,G,B) (BUFFTYPE)((((DWORD)R)<<16) | (((DWORD)G)<<8) | (((DWORD)B)))

#define R(x)			((unsigned char)((x>>16)))
#define G(x)			((unsigned char)((x>>8)))
#define B(x)			((unsigned char)((x)))

#endif//BUFFTYPE_24BPP

//32bit color
#ifdef BUFFTYPE_32BPP
typedef unsigned long  BUFFTYPE;
typedef unsigned long *PBUFFTYPE;
#define RGB(R,G,B) (BUFFTYPE)((((DWORD)R)<<16) | (((DWORD)G)<<8) | (((DWORD)B)))

#define R(x)			((unsigned char)((x>>16)))
#define G(x)			((unsigned char)((x>>8)))
#define B(x)			((unsigned char)((x)))


#endif//BUFFTYPE_32BPP


typedef BUFFTYPE			COLORREF;
typedef PBUFFTYPE			PCOLORREF;


#define RGB_WHITE			RGB(255,255,255)
#define RGB_BLACK			RGB(0,0,0)
#define RGB_GRAY			RGB(0xC0,0xC0,0xC0)
#define RGB_DKGRAY			RGB(0x80,0x80,0x80)
#define RGB_RED				RGB(255,0,0)
#define RGB_GREEN			RGB(0,255,0)
#define RGB_BLUE			RGB(0,0,255)


//window and control style 

//color style 1
#define RGB_NAVYBLUEBRIGHT  RGB(136,192,184)
#define RGB_NAVYBLUEDARK    RGB(72,144,136)
#define RGB_NAVYLTGRAY		RGB(200,224,216)
#define RGB_NAVYDKGRAY		RGB(44,78,71)
//color style 2


#define RGB_SYSTEMBRIGHT		RGB_NAVYBLUEBRIGHT
#define RGB_SYSTEMDARK			RGB_NAVYBLUEDARK
#define RGB_SYSTEMLTGRAY		RGB_NAVYLTGRAY
#define RGB_SYSTEMDKGRAY		RGB_NAVYDKGRAY
#define RGB_SYSTEMBLACK		RGB_BLACK
#define RGB_SYSTEMWHITE		RGB_WHITE
#define RGB_SYSTEMTRANS		RGB(255,0,0)

#define RGB_SYSTEMHIBRIGHT	RGB(173,221,255)



#define VSCROLL_WIDTH			13
#define VSCROLL_ARROW_HEIGHT	13
#define HSCROLL_WIDTH			13
#define HSCROLL_ARROW_HEIGHT	13

//Width, in pixels, of a vertical scroll bar;
//and height, in pixels, of the arrow bitmap on a vertical scroll bar.
#define SM_CXVSCROLL	0x0001
#define SM_CYVSCROLL	0x0002
#define SM_CXHSCROLL	0x0003
#define SM_CYHSCROLL	0x0004

/* DrawText() Format Flags */
#define DT_VMASK		0x0000000F
#define DT_TOP			0x00000001
#define DT_VCENTER		0x00000002
#define DT_BOTTOM		0x00000004

#define DT_HMASK		0x000000F0
#define DT_LEFT			0x00000010
#define DT_CENTER		0x00000020
#define DT_RIGHT		0x00000040


//MACRO DEFINE
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define MAKEWORD(low, high) ( (WORD)( ((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)) )
#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#define LOSWORD(l)          ((SWORD)(DWORD)(l))
#define HISWORD(l)          ((SWORD)((((DWORD)(l)) >> 16) & 0xFFFF))

#define MAKELONG(low, high) ( (LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)) )

#define max(x, y)           ((x > y)?x:y)
#define min(x, y)           ((x < y)?x:y)
#define swap(x,y)			{(x)^=(y);(y)^=(x);(x)^=(y);}


#define Pi	3.1415926
#define DEG2RAD(x) Pi*x/180.000000
#define RAD2DEG(x) 180.000000*x/Pi


/****** Common struct **************************************************/

typedef struct tagPOINT
{
	unsigned int x;
	unsigned int y;
} POINT;
typedef POINT* PPOINT;
typedef PPOINT LPPOINT;
typedef struct tagRECT
{
	int left;
 	int top;
 	int right;
 	int bottom;
} RECT;
typedef RECT* PRECT;
typedef RECT* LPRECT;

typedef struct tagSIZE
{
	int cx;
	int cy;
} SIZE;
typedef SIZE* PSIZE;

/* RGB struct */
typedef struct
{
	BYTE r;
	BYTE g;
	BYTE b;
}RGB;
typedef RGB* PRGB;


/*  0917 refefine

#define __DEBUG

#ifdef __DEBUG
	#define printerror(a)		printf("%s\n",a);
#else
	#define printerror(a)		
#endif
*/


//system message define 

#define MTYPE_MASK				0xf000

//Mouse raw Message
#define MTYPE_RAW_MOUSE				0x1000

#define LMSG_RAW_MOUSEDOWN			0x1001
#define LMSG_RAW_MOUSEUP			0x1002
#define LMSG_RAW_MOUSEMOVE			0x1003

//Key RAW Message
#define MTYPE_RAW_KEY				0x2000

#define LMSG_RAW_KEYDOWN			0x2000
#define LMSG_RAW_KEYUP				0x2001
#define LMSG_RAW_CHAR				0x2002


//Mouse Message
#define MTYPE_MOUSE				0x3000

#define LMSG_MOUSEDOWN				0x3001
#define LMSG_MOUSEUP				0x3002
#define LMSG_MOUSEMOVE				0x3003
//NC Mouse message
#define LMSG_NCMOUSEDOWN			0x0004
#define LMSG_NCMOUSEUP				0x0005
#define LMSG_NCMOUSEMOVE			0x0006

//Pen Message
#define MTYPE_PEN				0x3000

#define LMSG_PENDOWN				LMSG_MOUSEDOWN
#define LMSG_PENUP				LMSG_MOUSEUP
#define LMSG_PENMOVE				LMSG_MOUSEMOVE

#define LMSG_NCPENDOWN				LMSG_NCMOUSEDOWN
#define LMSG_NCPENUP				LMSG_NCMOUSEUP
#define LMSG_NCPENMOVE				LMSG_NCMOUSEMOVE

//Key Message
#define MTYPE_KEY				0x4000

#define LMSG_KEYDOWN				0x4000
#define LMSG_KEYUP				0x4001
#define LMSG_CHAR				0x4002

//common message
#define MTYPE_COMMON				0x5000

#define LMSG_QUIT				0x5000
#define LMSG_PAINT				0x5001
#define LMSG_NCPAINT				0x5002
#define LMSG_SETFOCUS				0x5003
#define LMSG_KILLFOCUS				0x5004
#define LMSG_CREATE				0x5005
#define LMSG_DESTROY				0x5006
#define LMSG_COMMAND				0x5007
#define LMSG_SRVEXIT				0x5008
#define LMSG_ERASEBKGND				0x5009
#define LMSG_ACTIVEWINDOW			0x500A
#define LMSG_DISACTIVEWINDOW			0x500B
#define LMSG_ENABLE				0x500C
#define LMSG_TIMER				0x500D
#define LMSG_SHOWWINDOW				0x500E
#define LMSG_VSCROLL				0x500F
#define LMSG_HSCROLL				0x5010
#define LMSG_GETTEXT 				0x5011
#define LMSG_SETTEXT 				0x5012

#define LMSG_CLOSE				0x5014
#define LMSG_OK					0x5015

#define LMSG_GETTEXTLENGTH			0x5016
#define LMSG_RESETIME				0X5017

#define BM_SETCHECK				0x6003
#define BM_GETCHECK				0x6004

#define BN_CLICKED 				0x6005
#define STARTMENU_CLICKED			0x6006
#define STARTMENU_ITEM_CLICKED			0x6007
//listbox
#define LB_RESETCONTENT				0x6008
#define LB_ADDSTRING				0x6009
#define LB_INSERTSTRING				0x600A
#define LB_DELETESTRING				0x600B
#define LB_FINDSTRING				0x600C
#define LB_FINDSTRINGEXACT			0x600D
#define LB_GETCOUNT				0x600E
#define LB_SETCURSEL				0x600F
#define LB_GETCURSEL				0x6010
#define LB_GETTEXT				0x6011
#define LB_SETTEXT				0x6012
#define LBN_SELCHANGE				0x6013


//EDIT BOX
#define EN_MAXTEXT				0x6030
#define EN_CLICKED				0x6031
#define EN_SETFOCUS				0x6033
#define EN_KILLFOCUS				0x6034
#define EN_CHANGE				0x6035
#define EN_HSCROLL				0x6038
#define EN_VSCROLL				0x6039
#define EN_ENTER				0x603A



// Edit Control messages
#define EM_LIMITTEXT           			0xF0C5
#define EM_SETPASSWORDCHAR     			0xF0CC
#define EM_SETREADONLY          		0xF0CF
#define EM_GETPASSWORDCHAR      		0xF0D2

//tabcontrol
#define TM_ADDITEM				0x7000
#define TM_SETCURTAB				0x7001	
#define TM_TABCHANGED				0x7002
#define TM_GETCURTAB				0x7003

//startmenu control
#define SM_ADDITEM				0x7004

//ime
#define LMSG_IME_SWITCH				0X9000
#define LMSG_IME_LEFTKEY_PRESSED		0x9001
#define LMSG_IME_RIGHTKEY_PRESSED		0x9002
#define LMSG_IME_LIST_PRESSED			0x9003

//IPC message
#define MTYPE_IPC				0x8000

#define LMSG_IPC_CREATEAPP			0x8001
#define LMSG_IPC_DESTROYAPP			0x8002
#define LMSG_IPC_EXITSERVER			0x8003
#define LMSG_IPC_CLIPRGNCHANGE		 	0x8004	//main window's clip area changed 
#define LMSG_IPC_REDRAW				0x8005	//main window's redraw message
#define LMSG_IPC_SHOWMAINWIN			0x8006
#define LMSG_IPC_HIDEMAINWIN			0x8007
#define LMSG_IPC_ACTIVEAPP			0x8008  //active application
#define LMSG_IPC_DISACTIVEAPP			0x800A  //disactive application
#define LMSG_IPC_SHOWMAINWIN_ANS		0x800D  //from client to server
#define LMSG_IPC_CHAR				0x8010	//character message sended to client by server
#define LMSG_IPC_KEYDOWN			0x8011	//key message sended to client by server
#define LMSG_IPC_PENDOWN			0x8012	//key message sended to client by server
#define LMSG_IPC_PENMOVE			0x8013	//key message sended to client by server
#define LMSG_IPC_PENUP				0x8014	//key message sended to client by server


#define LMSG_USER				0xFF00

typedef HWND IpcHand;


#ifdef __cplusplus
}
#endif

#endif

