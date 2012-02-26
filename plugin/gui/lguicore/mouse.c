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

#include "../include/common.h"
#include "../include/mouse_ps2.h"
#include "../include/mouse.h"
#include "../include/gal.h"
#include "../include/blockheap.h"
#include "../include/rect.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/gmessage.h"
#include "../include/hdc.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/shmem.h"
#include "../include/mouse_ial.h"

#define TRANSCOLOR	RGB(255,0,0)


extern PWindowsTree _lGUI_pServerWin;
static thread_t thread_mouse;
static MOUSEINFO MouseInfo={false,0,0,true,0,0,NULL,NULL};

void /*inline*/ 
LockMouse()
{
	LockShMem();
}

void /*inline*/ 
UnLockMouse()
{
	UnLockShMem();
}

BOOL 
WithinCursorBox(
	int x,
	int y
)
{
	if((x>=MouseInfo.x && x<(MouseInfo.x + MouseInfo.nWidth))&&
	   (y>=MouseInfo.y && y<(MouseInfo.y + MouseInfo.nHeight)))
	   	return true;
	else
		return false;
}

BOOL 
IntersectCursorBox(
	PRECT lpRect
)
{
	RECT rc;
	SetRect(&rc, MouseInfo.x, MouseInfo.y, 
		MouseInfo.x+MouseInfo.nWidth, MouseInfo.y+MouseInfo.nHeight);
	if(IsIntersect (lpRect,&rc))
		return true;
	else
		return false;
}

void 
ShowMouse()
{
	//save the dot color value for the current position of mouse
	Gal_GetBoxRaw(MouseInfo.x,MouseInfo.y,
		MouseInfo.nWidth,MouseInfo.nHeight,MouseInfo.pSaveBox);
	Gal_PutBoxWithTrCorRaw(MouseInfo.x,MouseInfo.y,
		MouseInfo.nWidth,MouseInfo.nHeight,MouseInfo.pCursorBox,TRANSCOLOR);
	MouseInfo.fHide=false;
}

void
HideMouse()
{
	int i,j;
	//restore the points of current position of the mouse
	for(i=0;(i<MouseInfo.nHeight)&&(MouseInfo.y+i<SCREEN_HEIGHT);i++){
		for(j=0;(j<MouseInfo.nWidth)&&(MouseInfo.x+j<SCREEN_WIDTH);j++){
			Gal_PutPixelRaw(MouseInfo.x+j,MouseInfo.y+i,
				*(MouseInfo.pSaveBox+i*MouseInfo.nWidth+j));
		}
	}
	MouseInfo.fHide=true;
}

BOOL /*inline*/ 
MouseHided()
{
	return MouseInfo.fHide;
}

void 
InitMouseServer()
{
	int i,j;
	UINT16 iType=0;
	UINT16 iBitCount=0;
	UINT08 r,g,b;
	FILE_P fd;
	char pString[100];
	InstallMouseDevice();

#ifdef CFG_SHOW_MOUSE

	MouseInfo.pCursorBox = (PCOLORREF)(GetShmAddr() + SHMEM_CURSORBOX_OFFSET);
	MouseInfo.pSaveBox = (PCOLORREF)(GetShmAddr() + SHMEM_SAVEBOX_OFFSET);

	strcpy(pString,pEnvStr);
	strcat(pString,MOUSEFILE);
	fd=OPEN(pString,RDONLY);
	SEEK(fd,0,0);
	READ(fd,&iType,sizeof(iType));
	SEEK(fd,0x12,0);
	READ(fd,&MouseInfo.nWidth,sizeof(MouseInfo.nWidth));
	READ(fd,&MouseInfo.nHeight,sizeof(MouseInfo.nHeight));
	SEEK(fd,0x1c,0);
	READ(fd,&iBitCount,1);
	SEEK(fd,0x36,0);

	for(i=0;i<MouseInfo.nHeight;i++){
		SEEK(fd,0x36+(MouseInfo.nHeight-i-1)*MouseInfo.nWidth*3,0);
		for(j=0;j<MouseInfo.nWidth;j++){
			READ(fd,&b,1);
			READ(fd,&g,1);
			READ(fd,&r,1);
			*(MouseInfo.pCursorBox+i*MouseInfo.nWidth+j)=RGB(r,g,b);
		}
	}

	SetMouseDimension();
	SetMousePos();
#endif
	OpenMouse();
#ifdef CFG_SHOW_MOUSE
	ShowMouse();//show cursor here
#endif
	//create mouse thread
	pthread_create(&thread_mouse,NULL,
				MouseMainLoop,NULL);
}

void 
TerminateMouseServer()
{
}

void* 
MouseMainLoop(
	void* para
)
{
	int x,y,event;
	int iRetVal;
	while(1){
		iRetVal = ReadMouse(&x,&y,&event);
		if(!iRetVal)
			continue;
#ifdef CFG_SHOW_MOUSE

		LockMouse();
		HideMouse();
		MouseInfo.x=x;
		MouseInfo.y=y;

		SetMousePos();

		ShowMouse();
		UnLockMouse();

#else
		MouseInfo.x = x;
		MouseInfo.y = y;

#endif
		//printf("%d,%d,%d\n",x,y,event);
		//send message
		switch(event){
		case MOUSE_EVENT_LBUTTON_DOWN:
  			PostMessage(_lGUI_pServerWin,LMSG_RAW_MOUSEDOWN,
				MouseInfo.x,MouseInfo.y);
			break;
		case MOUSE_EVENT_LBUTTON_UP:
			PostMessage(_lGUI_pServerWin,LMSG_RAW_MOUSEUP,
				MouseInfo.x,MouseInfo.y);
			break;
		case MOUSE_EVENT_MOVE:
			//printf("MOVE\n");
			PostMessage(_lGUI_pServerWin,LMSG_RAW_MOUSEMOVE,
				MouseInfo.x,MouseInfo.y);
			break;
		}
	}
}




void /*inline*/ 
RequestPaint(
	PRECT lpRect
)
{
#ifdef CFG_SHOW_MOUSE

	LockMouse();

	GetMousePos();

	if(IntersectCursorBox(lpRect))
		HideMouse();

	MouseInfo.fPaintCompleted=false;
#else
	return;
#endif
	
}

void /*inline*/ 
CompletePaint()
{
#ifdef CFG_SHOW_MOUSE
	if(MouseInfo.fHide)
		ShowMouse();
	MouseInfo.fPaintCompleted=true;
	UnLockMouse();
#else
	return;
#endif
	
}

void /*inline*/ 
SetMousePos()
{
	char* pShMemAddr;
	pShMemAddr = GetShmAddr();
	*((int*)(pShMemAddr + SHMEM_MOUSE_OFFSET_X))=MouseInfo.x;
	*((int*)(pShMemAddr + SHMEM_MOUSE_OFFSET_Y))=MouseInfo.y;
}

void /*inline*/ 
GetMousePos()
{
	char* pShMemAddr;
	pShMemAddr = GetShmAddr();
	MouseInfo.x = *((int*)(pShMemAddr + SHMEM_MOUSE_OFFSET_X));
	MouseInfo.y = *((int*)(pShMemAddr + SHMEM_MOUSE_OFFSET_Y));
}

void /*inline*/ 
GetMouseDimension()
{
	char* pShMemAddr;
	pShMemAddr = GetShmAddr();
	MouseInfo.nWidth = *((int*)(pShMemAddr + SHMEM_MOUSE_WIDTH_OFFSET));
	MouseInfo.nHeight = *((int*)(pShMemAddr + SHMEM_MOUSE_HEIGHT_OFFSET));
}

void /*inline*/ 
SetMouseDimension()
{
	char* pShMemAddr;
	pShMemAddr = GetShmAddr();
	*((int*)(pShMemAddr + SHMEM_MOUSE_WIDTH_OFFSET)) = MouseInfo.nWidth;
	*((int*)(pShMemAddr + SHMEM_MOUSE_HEIGHT_OFFSET)) = MouseInfo.nHeight;
}
