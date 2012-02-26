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
#include "../include/taskbar.h"
#include "../include/regclass.h"
#include "../include/hdc.h"
#include "../include/gdi.h"
#include "../include/rect.h"
#include "../include/gmessage.h"
#include "../include/blockheap.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/mledit.h"
#include "../include/scrollbar.h"
#include "../include/keyboard_pro.h"
#include "../include/skbwin.h"
#include "../include/text.h"
#include "../include/imewin.h"
#include "../include/startmenu.h"

#define ID_TIMER	1000

PWindowsTree _lGUI_pTaskBarWindow;

extern int Gal_iBytesDataType;

//process Pen down message of taskbar
static void 
ProcessTaskbarPenDown(
	HWND hWnd, 
	int x, 
	int y
);


static void 
TaskBar_StartButtonPressed(
	HWND hWnd,
	PTaskBarItem pItem
);


static void 
TaskBar_SkbPressed(
	HWND hWnd,
	PTaskBarItem pItem
);


static void 
TaskBar_DbcPressed(
	HWND hWnd,
	PTaskBarItem pItem
);


static void 
TaskBar_ImePressed(
	HWND hWnd,
	PTaskBarItem pItem
);


static void 
TaskBar_ClockPressed(
	HWND hWnd,
	PTaskBarItem pItem
);


static void 
ClearTaskBarData(
	HWND hWnd
);


static BOOL 
InitFixTaskBarData(
	HWND hWnd
);


static void* 
GetBmpBufAddress(
	int iCounter,
	void* pOrgPointer, 
	int iNumber
);


static void* 
GetStartButtonBmpBufAddress(
	void* pOrgPointer,
	int iNumber
);


static void* 
GetSkbBmpBufAddress(
	void* pOrgPointer,
	int iNumber
);


static void* 
GetDbcBmpBufAddress(
	void* pOrgPointer,
	int iNumber
);


static void* 
GetImeBmpBufAddress(
	void* pOrgPointer,
	int iNumber
);


static void 
GetCurrentTime(
	char* pString
);

static BOOL 
OutputTime(
	HWND hWnd,
	HDC hDC
); 


BOOL 
RegisterTaskBar()
{ 
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)TaskBarControlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "taskbar";
	wcex.hIconSm		= 0;
	
	RegisterClass(&wcex);			
	return true;
}


LRESULT 
TaskBarControlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	POINT point;
	PWindowsTree pWin;
	RECT rc;
	HDC hDC;
	
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	PTaskBar pTaskBar;

	pWin=(PWindowsTree)hWnd;

	
	switch(iMsg)
	{
		case LMSG_CREATE:
			_lGUI_pTaskBarWindow = (PWindowsTree)hWnd;
			InitFixTaskBarData(hWnd);
		//	SetTimer(hWnd,ID_TIMER,6000); 修改了
			break;
		case LMSG_TIMER:
		{
			PTaskBar pTaskBar;
			PTaskBarItem pItem;
			pTaskBar = (PTaskBar)(pWin->dwAddData);
			pItem = pTaskBar->pItemHead;
			while(1){
				if(pItem->id == TASKBAR_OBJID_CLOCK)
					break;
				pItem = pItem->pNext;
			}
			if(!pItem)
				return false;
			InvalidateRect(hWnd,&(pItem->rect),true);
			break;
		}
	
		case LMSG_COMMAND:
			break;
		case LMSG_PENDOWN:
		{
			int x, y;
			CaptureMouse(hWnd,BYCLIENT);	
			x = (int)wParam;
			y = (int)lParam;
			ProcessTaskbarPenDown(hWnd, x, y);
			break;
		}		
		case LMSG_PENMOVE:
			break;

		case LMSG_PENUP:
			DisCaptureMouse();
			break;
		case LMSG_ERASEBKGND:			
		
			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;

			((BRUSH*)hBrush)->crBrushColor=RGB_SYSTEMHIBRIGHT;

			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;
			
		case LMSG_PAINT:
		{
			int iHeight,iWidth;
			HPEN hPenWhite;
			PBITMAP pBitmap;
			PTaskBarItem pItem;
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			
			GetClientRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;
			

			hPenWhite 	=CreatePen(PS_SOLID,1,RGB_SYSTEMWHITE);
			
			SelectObject(hDC,hPenWhite);
			MoveToEx(hDC,rc.left,rc.top,&point);
			LineTo(hDC,rc.right,rc.top);


			DeleteObject(hPenWhite);
			//output those bitmaps
			pBitmap = malloc(sizeof(BITMAP));
			if(!pBitmap)
				return;
			memset(pBitmap,0,sizeof(BITMAP));
			pTaskBar = (PTaskBar)(pWin->dwAddData);
			pItem = pTaskBar->pItemHead;
			iWidth = pWin->rect.right - pWin->rect.left + 1;
			iHeight = pWin->rect.bottom - pWin->rect.top + 1;
			
			while(pItem){
				if(pItem->bShow){
					pBitmap->bmHeight = pItem->rect.bottom - pItem->rect.top + 1;
					pBitmap->bmWidth = pItem->rect.right - pItem->rect.left + 1;
					pBitmap->bmBits = GetBmpBufAddress(pItem->id,pItem->pData,pItem->iCurValue);
					OutputBmpObj(hDC,pItem->rect.left,pItem->rect.top,
						iWidth,iHeight,(HBITMAP)pBitmap);
				}
				pItem = pItem->pNext;
			}
			OutputTime(hWnd,hDC);
			free(pBitmap);

			EndPaint(hWnd, &ps);
			break;
		}
		case LMSG_DESTROY:
			ClearTaskBarData(hWnd);
			KillTimer(hWnd,ID_TIMER);
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}

static BOOL 
OutputTime(
	HWND hWnd,
	HDC hDC
)
{
	char pString[20];
	PWindowsTree pWin;
	PTaskBar pTaskBar;
	PTaskBarItem pItem;
	HFONT hFont;
	pWin = (PWindowsTree)hWnd;
	pTaskBar = (PTaskBar)(pWin->dwAddData);
	pItem = pTaskBar->pItemHead;
	while(pItem){
		if(pItem->id == TASKBAR_OBJID_CLOCK)
			break;
		pItem = pItem->pNext;
	}
	if(!pItem)
		return false;
	hFont = GetStockObject(FONT_12_12);
	SelectObject(hDC,hFont);
	GetCurrentTime(pString);
	DrawText(hDC,pString,strlen(pString),&(pItem->rect),DT_CENTER|DT_VCENTER);
	DeleteObject(hFont);
	return true;
}

static void 
GetCurrentTime(
	char* pString
)
{
	sprintf(pString,20,"%02d:%02d",10,20);	
}			


static void 
ProcessTaskbarPenDown(
	HWND hWnd, 
	int x, 
	int y
)
{
	PWindowsTree pWin;
	PTaskBarItem pItem;
	PTaskBar pTaskBar;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;
	pTaskBar = (PTaskBar)(pWin->dwAddData);
	ScreenToClient(hWnd,&x,&y);
	pItem = pTaskBar->pItemHead;
	if(!pItem)
		return;
	
	while(pItem){
		if(PtInRect(&(pItem->rect),x,y)){
			switch(pItem->id){
			case TASKBAR_OBJID_STARTBUTTON:
				TaskBar_StartButtonPressed(hWnd, pItem);
				break;
			case TASKBAR_OBJID_SKB:
				TaskBar_SkbPressed(hWnd, pItem);
				break;
			case TASKBAR_OBJID_DBC:
				TaskBar_DbcPressed(hWnd,pItem);
				break;
			case TASKBAR_OBJID_IME:
				TaskBar_ImePressed(hWnd,pItem);
				break;
			case TASKBAR_OBJID_CLOCK:
				TaskBar_ClockPressed(hWnd,pItem);
				break;
			}
		}
		pItem = pItem->pNext;
	}

}


static void 
TaskBar_StartButtonPressed(
	HWND hWnd,
	PTaskBarItem pItem
)
{
	PWindowsTree pWin;
	int iRetMsg;
	pWin = (PWindowsTree)hWnd;
	iRetMsg=STARTMENU_CLICKED;
	iRetMsg=iRetMsg<<16;
	SendNotifyMessage((HWND)(pWin->pParent), LMSG_COMMAND,(WPARAM)iRetMsg,(LPARAM)NULL);

}


static void 
TaskBar_SkbPressed(
	HWND hWnd,
	PTaskBarItem pItem
)
{
	pItem->iCurValue = (pItem->iCurValue + 1)%pItem->iTotalPictureNumber;
	InvalidateRect(hWnd,&(pItem->rect),true);
	
	if(pItem->iCurValue)
		ShowSkbWindow();
	else
		HideSkbWindow();
}


static void 
TaskBar_DbcPressed(
	HWND hWnd,
	PTaskBarItem pItem
)
{
	pItem->iCurValue = (pItem->iCurValue + 1)%pItem->iTotalPictureNumber;
	InvalidateRect(hWnd,&(pItem->rect),true);
	SwitchDbc(pItem->iCurValue);
}


static void 
TaskBar_ImePressed(
	HWND hWnd,
	PTaskBarItem pItem
)
{
	pItem->iCurValue = (pItem->iCurValue + 1)%pItem->iTotalPictureNumber;
	InvalidateRect(hWnd,&(pItem->rect),true);
	//iCurValue is mapped to the ime status 
	switch(pItem->iCurValue){
	case IME_STATUS_ENGLISH:
		SetSkbLayout(SKB_LAYOUT_STANDARD);
		SwitchIme();
		break;
	case IME_STATUS_HANDWRITE:	
		SetSkbLayout(SKB_LAYOUT_HANDWRITE);
		SwitchIme();
		break;
	default:		//other installed chinese input method
		SetSkbLayout(SKB_LAYOUT_STANDARD);
		SwitchIme();
		break;
	}
	
}


static void 
TaskBar_ClockPressed(
	HWND hWnd,
	PTaskBarItem pItem
)
{
//	printf("!\n");
}


static void 
ClearTaskBarData(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PTaskBar pTaskBar;
	PTaskBarItem pItem,pPrev;


	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;
	pTaskBar = (PTaskBar)(pWin->dwAddData);
	if(!pTaskBar)
		return;
	pItem = pTaskBar->pItemHead;
	pPrev = pItem;
	while(pPrev){
		pItem = pItem->pNext;
		free(pPrev->pData);
		free(pPrev);
		pPrev = pItem;
	}
	free(pTaskBar);
}


static BOOL 
InitFixTaskBarData(
	HWND hWnd
)
{
	FILE_P fd;
	PTaskBarItem pItem;
	char pString[256];

	PTaskBar pTaskBar;
	PWindowsTree pWin;
	pWin  = (PWindowsTree)hWnd;
	if(!pWin)
		return false;
	pTaskBar = (PTaskBar)malloc(sizeof(TaskBar));
	if(!pTaskBar)
		return false;
	memset(pTaskBar,0,sizeof(TaskBar));
	pTaskBar->iTotalItem = TASKBAR_ITEMS_NUMBER;
	//==========================================================
	//start button
	pItem = (PTaskBarItem)malloc(sizeof(TaskBarItem));
	if(!pItem)
		return false;
	memset(pItem,0,sizeof(TaskBarItem));
	pItem->bShow = true;
	pItem->iCurValue = 0;
	pItem->id = TASKBAR_OBJID_STARTBUTTON;
	pItem->iTotalPictureNumber =TASKBAR_STARTBUTTONBMP_NUMBER;
	SetRect(&(pItem->rect),TASKBAR_STARTBUTTON_LEFT,1,
		TASKBAR_STARTBUTTON_LEFT + TASKBAR_STARTBUTTON_WIDTH-1,TASKBAR_BUTTONS_HEIGHT);
	pItem->pData = malloc(pItem->iTotalPictureNumber *
		TASKBAR_BUTTONS_HEIGHT * TASKBAR_STARTBUTTON_WIDTH * Gal_iBytesDataType);
	if(!pItem->pData)
		return false;

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_STARTBUTTONBMP_NOPRESSED);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_STARTBUTTON_WIDTH,TASKBAR_BUTTONS_HEIGHT,pItem->pData);
	CLOSE(fd);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_STARTBUTTONBMP_PRESSED);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_STARTBUTTON_WIDTH,TASKBAR_BUTTONS_HEIGHT,
		GetStartButtonBmpBufAddress(pItem->pData,1));
	CLOSE(fd);
	

	pTaskBar->pItemHead = pItem;


	//=====================================================
	//skb
	pItem = (PTaskBarItem)malloc(sizeof(TaskBarItem));
	if(!pItem)
		return false;
	memset(pItem,0,sizeof(TaskBarItem));
	pItem->bShow = true;
	pItem->iCurValue = 0;
	pItem->id = TASKBAR_OBJID_SKB;
	pItem->iTotalPictureNumber = TASKBAR_SKBBMP_NUMBER;
	SetRect(&(pItem->rect),TASKBAR_SKBSWITCH_LEFT,1,
		TASKBAR_SKBSWITCH_LEFT + TASKBAR_SKBSWITCH_WIDTH - 1,TASKBAR_BUTTONS_HEIGHT);
	pItem->pData = malloc(pItem->iTotalPictureNumber *
		TASKBAR_BUTTONS_HEIGHT * TASKBAR_SKBSWITCH_WIDTH * Gal_iBytesDataType);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_SKBBMP_NOPRESSED);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_SKBSWITCH_WIDTH,TASKBAR_BUTTONS_HEIGHT,pItem->pData);
	CLOSE(fd);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_SKBBMP_PRESSED);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_SKBSWITCH_WIDTH,TASKBAR_BUTTONS_HEIGHT,
		GetSkbBmpBufAddress(pItem->pData,1));
	CLOSE(fd);
	pItem->pNext = pTaskBar->pItemHead;
	pTaskBar->pItemHead = pItem;

	//=====================================================
	//Dbc
	pItem = (PTaskBarItem)malloc(sizeof(TaskBarItem));
	if(!pItem)
		return false;
	memset(pItem,0,sizeof(TaskBarItem));
	pItem->bShow = true;
	pItem->iCurValue = 0;
	pItem->id = TASKBAR_OBJID_DBC;
	pItem->iTotalPictureNumber = TASKBAR_DBCBMP_NUMBER;
	SetRect(&(pItem->rect),TASKBAR_DBCSWITCH_LEFT,1,
		TASKBAR_DBCSWITCH_LEFT + TASKBAR_DBCSWITCH_WIDTH - 1,TASKBAR_BUTTONS_HEIGHT);
	pItem->pData = malloc(pItem->iTotalPictureNumber *
		TASKBAR_BUTTONS_HEIGHT * TASKBAR_DBCSWITCH_WIDTH * Gal_iBytesDataType);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_DBCBMP_NOPRESSED);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_DBCSWITCH_WIDTH,TASKBAR_BUTTONS_HEIGHT,pItem->pData);
	CLOSE(fd);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_DBCBMP_PRESSED);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_DBCSWITCH_WIDTH,TASKBAR_BUTTONS_HEIGHT,
		GetDbcBmpBufAddress(pItem->pData,1));
	CLOSE(fd);
	pItem->pNext = pTaskBar->pItemHead;
	pTaskBar->pItemHead = pItem;

	//=====================================================
	//ime
	pItem = (PTaskBarItem)malloc(sizeof(TaskBarItem));
	if(!pItem)
		return false;
	memset(pItem,0,sizeof(TaskBarItem));
	pItem->bShow = true;
	pItem->iCurValue = 0;
	pItem->id = TASKBAR_OBJID_IME;
	pItem->iTotalPictureNumber = TASKBAR_IMEBMP_NUMBER;
	SetRect(&(pItem->rect),TASKBAR_IMESWITCH_LEFT,1,
		TASKBAR_IMESWITCH_LEFT + TASKBAR_IMESWITCH_WIDTH - 1,TASKBAR_BUTTONS_HEIGHT);
	pItem->pData = malloc(pItem->iTotalPictureNumber *
		TASKBAR_BUTTONS_HEIGHT * TASKBAR_IMESWITCH_WIDTH * Gal_iBytesDataType);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_IMEBMP_ENG);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_IMESWITCH_WIDTH,TASKBAR_BUTTONS_HEIGHT,pItem->pData);
	CLOSE(fd);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_IMEBMP_HANDWRITE);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_IMESWITCH_WIDTH,TASKBAR_BUTTONS_HEIGHT,
		GetImeBmpBufAddress(pItem->pData,1));
	CLOSE(fd);

	pItem->pNext = pTaskBar->pItemHead;
	pTaskBar->pItemHead = pItem;
	
	pTaskBar->pItemIme = pItem;
	//=====================================================
	//clock
	pItem = (PTaskBarItem)malloc(sizeof(TaskBarItem));
	if(!pItem)
		return false;
	memset(pItem,0,sizeof(TaskBarItem));
	pItem->bShow = true;
	pItem->iCurValue = 0;
	pItem->id = TASKBAR_OBJID_CLOCK;
	pItem->iTotalPictureNumber = TASKBAR_CLOCKBMP_NUMBER;
	SetRect(&(pItem->rect),TASKBAR_CLOCKBOX_LEFT,1,
		TASKBAR_CLOCKBOX_LEFT + TASKBAR_CLOCKBOX_WIDTH - 1,TASKBAR_BUTTONS_HEIGHT);
	pItem->pData = malloc(pItem->iTotalPictureNumber *
		TASKBAR_BUTTONS_HEIGHT * TASKBAR_CLOCKBOX_WIDTH * Gal_iBytesDataType);

	strcpy(pString,pEnvStr);
	strcat(pString,TASKBAR_CLOCKBMP);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,TASKBAR_CLOCKBOX_WIDTH,TASKBAR_BUTTONS_HEIGHT,pItem->pData);
	CLOSE(fd);

	pItem->pNext = pTaskBar->pItemHead;
	pTaskBar->pItemHead = pItem;

	pWin->dwAddData = (DWORD)pTaskBar;
	return true;
}


static void* 
GetBmpBufAddress(
	int iCounter,
	void* pOrgPointer, 
	int iNumber
)
{
	switch(iCounter){
	case TASKBAR_OBJID_STARTBUTTON:
		return GetStartButtonBmpBufAddress(pOrgPointer,iNumber);
	case TASKBAR_OBJID_SKB:
		return GetSkbBmpBufAddress(pOrgPointer,iNumber);
	case TASKBAR_OBJID_DBC:
		return GetDbcBmpBufAddress(pOrgPointer,iNumber);
	case TASKBAR_OBJID_IME:
		return GetImeBmpBufAddress(pOrgPointer,iNumber);
	default:
		return pOrgPointer;
	}
}


static void* 
GetStartButtonBmpBufAddress(
	void* pOrgPointer,
	int iNumber
)
{
	return (void*)((acoral_8*)pOrgPointer + 
		iNumber * TASKBAR_BUTTONS_HEIGHT * TASKBAR_STARTBUTTON_WIDTH * Gal_iBytesDataType);
}


static void* 
GetSkbBmpBufAddress(
	void* pOrgPointer,
	int iNumber
)
{
	return (void*)((acoral_8*)pOrgPointer + 
		iNumber * TASKBAR_BUTTONS_HEIGHT * TASKBAR_SKBSWITCH_WIDTH * Gal_iBytesDataType);
}


static void* 
GetDbcBmpBufAddress(
	void* pOrgPointer,
	int iNumber
)
{
	return (void*)((acoral_8*)pOrgPointer + 
		iNumber * TASKBAR_BUTTONS_HEIGHT * TASKBAR_DBCSWITCH_WIDTH * Gal_iBytesDataType);
}


static void* 
GetImeBmpBufAddress(
	void* pOrgPointer,
	int iNumber
)
{
	return (void*)((acoral_8*)pOrgPointer + 
		iNumber * TASKBAR_BUTTONS_HEIGHT * TASKBAR_IMESWITCH_WIDTH * Gal_iBytesDataType);
}






