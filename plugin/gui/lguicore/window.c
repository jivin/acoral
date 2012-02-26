/* Copyright (C) 2004-2005 Li Yudong
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
#include "../include/blockheap.h"
#include "../include/mouse.h"
#include "../include/keyboard.h"
#include "../include/gal.h"
#include "../include/rect.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/hdc.h"
#include "../include/lguiapp.h"
#include "../include/ipc.h"
#include "../include/gmessage.h"
#include "../include/regclass.h"
#include "../include/shmem.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/scrollbar.h"
#include "../include/shmem.h"
#include "../include/gtimer.h"


//focus and active window strategy
//all windows (exclude controls) have a pointer which pointer to its focus control
//the control will get focus automaticly when the window acitved.
PWindowsTree	_lGUI_pFocus			=NULL;
//_lGUI_pActiveWin
//server: if _lGUI_pActiveWin = _lGUI_pWindowsTree then desktop is active
//otherwise the top application is active
//_lGUI_pActiveWin=_lGUI_pWindowsTree after createwindow 
PWindowsTree	_lGUI_pActiveWin		=NULL;
PWindowsTree 	_lGUI_pServerWin 		=NULL;

extern BOOL			_lGUI_bByServer;

extern PWindowsTree _lGUI_pImeWindow;
extern PWindowsTree _lGUI_pSkbWindow;

extern PWindowsTree _lGUI_pStartMenuWindow;

PlGUIAppStat  _lGUI_ServerStat=NULL;

PlGUIAppStat	_lGUI_pAppStat			=NULL;
//WndCaptureMouse _lGUI_wndCaptureMouse;




HWND GUIAPI CreateWindow(
  char* lpClassName,	// pointer to registered class name
  char* lpWindowName,	// pointer to window name
  DWORD dwStyle,        // window style & window statue
  int x,                // horizontal position of window
  int y,                // vertical position of window
  int nWidth,           // window width
  int nHeight,          // window height
  HWND hWndParent,      // handle to parent or owner window
  HMENU hMenu,          // handle to menu or child-window identifier
  HANDLE hInstance,     // handle to application instance
  LPVOID lpParam	    // pointer to window-creation data
)
{
	PWindowsTree pWin;
	int iWinType;
	iWinType=dwStyle & WS_TYPEMASK;
	if(((iWinType==WS_DESKTOP)||(iWinType==WS_MAIN)) && _lGUI_pWindowsTree){
		printerror("Main Window or desktop already exist!");
		return ERR_INV_HWND;
	}

/*	if(((iWinType==WS_CHILD)||(iWinType==WS_CONTROL)) && !_lGUI_pWindowsTree){
		printerror("main window or desktop does not exist!");
		return ERR_INV_HWND;
	}*/

	pWin =(PWindowsTree)malloc(sizeof(WindowsTree));
	if(!pWin){
		printerror("alloc memory error!\n");
		return ERR_INV_HWND;
	}

	memset(pWin,0,sizeof(WindowsTree));
	if(!lpClassName){
		printerror("class name string should not be null!");
		free(pWin);
	}

	strcpy(pWin->lpszClassName,lpClassName);
	//lpszCaption allowed to be Null string
	if(lpWindowName)
		strcpy(pWin->lpszCaption,lpWindowName);
	pWin->dwStyle		=dwStyle;
	pWin->pParent		=hWndParent;
	pWin->hMenu			=hMenu;

	SetRect(&(pWin->rect),x,y,x+nWidth-1,y+nHeight-1);

	if(hWndParent)
		ClientToScreenRect(hWndParent,&(pWin->rect));
	//alloc memory for scrollbar struct
	if(pWin->dwStyle & WS_VSCROLL){
		pWin->pVScroll = (LPSCROLLINFO)malloc(sizeof(SCROLLINFO));
		if(!(pWin->pVScroll)){
			free(pWin);
			return ERR_INV_HWND;
		}
		else{
			memset(pWin->pVScroll,0,sizeof(SCROLLINFO));
			//default set disabled
			pWin->pVScroll->dwStatus |= SS_DISABLED;
		}
	}
	if(pWin->dwStyle & WS_HSCROLL){
		pWin->pHScroll = (LPSCROLLINFO)malloc(sizeof(SCROLLINFO));
		if(!(pWin->pHScroll)){
			free(pWin->pVScroll);
			free(pWin);
			return ERR_INV_HWND;
		}
		else{
			memset(pWin->pHScroll,0,sizeof(SCROLLINFO));
			//default set disabled
			pWin->pHScroll->dwStatus |= SS_DISABLED;
		}
	}
	if((pWin->dwStyle & WS_VSCROLL) || (pWin->dwStyle & WS_HSCROLL)){
		pWin->pHCurState = (LPSCROLLCURSTATE)malloc(sizeof(SCROLLCURSTATE));
		pWin->pVCurState = (LPSCROLLCURSTATE)malloc(sizeof(SCROLLCURSTATE));

		if(!pWin->pHCurState || !pWin->pVCurState){
			if(pWin->pVScroll)
				free(pWin->pVScroll);
			if(pWin->pHScroll)
				free(pWin->pHScroll);
			free(pWin);
			return ERR_INV_HWND;
		}
		else{
			memset(pWin->pHCurState,0,sizeof(SCROLLCURSTATE));
			memset(pWin->pVCurState,0,sizeof(SCROLLCURSTATE));
		}
	}
	//caret struct
	/*
	pWin->pCaretInfo = (PCARETINFO)malloc(sizeof(CARETINFO));
	if(!pWin->pCaretInfo){
		free(pWin->pVScroll);
		free(pWin->pHScroll);
		free(pWin);
		return ERR_INV_HWND;
	}
	memset(pWin->pCaretInfo,0,sizeof(CARETINFO));
	pWin->pCaretInfo->hOwner = (HWND)pWin;
	*/
	switch(dwStyle & WS_TYPEMASK){
	case WS_DESKTOP:
		if(CreateDesktop(pWin)==ERR_INV_HWND){
			free(pWin);
			return ERR_INV_HWND;
		}
		else{
			SendMessage((HWND)pWin,LMSG_CREATE,(WPARAM)hInstance,(LPARAM)lpParam);
		}
		break;
	case WS_MAIN:
		if(CreateMain(pWin)==ERR_INV_HWND){
			free(pWin);
			return ERR_INV_HWND;
		}
		else{
			SendMessage((HWND)pWin,LMSG_CREATE,(WPARAM)hInstance,(LPARAM)lpParam);
		}
		break;
	case WS_CHILD:
		if(CreateChild(pWin)==ERR_INV_HWND){
			free(pWin);
			return ERR_INV_HWND;
		}
		else{
			SendMessage((HWND)pWin,LMSG_CREATE,(WPARAM)hInstance,(LPARAM)lpParam);
		}
		break;
	case WS_CONTROL:
		if(CreateControl(pWin)==ERR_INV_HWND){
			free(pWin);
			return ERR_INV_HWND;
		}
		else
			SendMessage((HWND)pWin,LMSG_CREATE,(WPARAM)hInstance,(LPARAM)lpParam);
		break;

	case WS_IMEWIN:
		if(CreateImeWin(pWin)==ERR_INV_HWND){
			free(pWin);
			return ERR_INV_HWND;
		}
		else
			SendMessage((HWND)pWin,LMSG_CREATE,(WPARAM)hInstance,(LPARAM)lpParam);
		break;
	case WS_SKBWIN:
		if(CreateSkbWin(pWin)==ERR_INV_HWND){
			free(pWin);
			return ERR_INV_HWND;
		}
		else
			SendMessage((HWND)pWin,LMSG_CREATE,(WPARAM)hInstance,(LPARAM)lpParam);
		break;
	case WS_MENUWIN:
		if(CreateMenuWin(pWin)==ERR_INV_HWND){
			free(pWin);
			return ERR_INV_HWND;
		}
		else
			SendMessage((HWND)pWin,LMSG_CREATE,(WPARAM)hInstance,(LPARAM)lpParam);
		break;

	}
	return pWin;
}



HWND 
CreateDesktop(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

//	_lGUI_pWindowsTree=pWin;
	PortSetCurWin(pWin);
	pWin->threadid=pthread_self();


	if(!InitMsgQueue((HWND)pWin)){
		printerror("init message queue error!");
		return ERR_INV_HWND;
	}


	if(!InitWindowRgn(pWin)){
		printerror("init window clip region error!");
		return ERR_INV_HWND;
	}
	return pWin;
}



HWND 
CreateMain(
	const HWND hWnd
)
{
	PlGUIAppStat _lGUI_Stat=_lGUI_CurStat;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	_lGUI_Stat->hand=(IpcHand)pWin;
//	_lGUI_pWindowsTree=pWin;
	PortSetCurWin(pWin);

	pWin->threadid=pthread_self();


	if(!InitMsgQueue((HWND)pWin)){
		printerror("init message queue error!");
		return ERR_INV_HWND;
	}

	if(!InitWindowRgn((HWND)pWin)){
		printerror("init window clip region error!");
		return ERR_INV_HWND;
	}

	//init the bound rect of the cliparea
	AddRectClipRegion (_lGUI_Stat->pClipRgn, &pWin->rect);
	GetBoundClipRegion(_lGUI_Stat->pClipRgn);

	//send create application message to server
	//application name is PID
	//sprintf(_lGUI_Stat->pAppName,"%d",getpid());
	CopyRect(&_lGUI_Stat->rc,&pWin->rect);

	SendMsgByClient(LMSG_IPC_CREATEAPP,(void*)_lGUI_Stat,0);

	sem_wait(&_lGUI_Stat->sem_cs);
	//LockMutexForSynchro();

	return pWin;

}


HWND 
CreateChild(
	const HWND hWnd
)
{

	PWindowsTree pCurWin;
	PWindowsTree pWin;
	int iZOrder=0;
	pWin=(PWindowsTree)hWnd;


	if(!InitMsgQueue((HWND)pWin)){
		printerror("init message queue error!");
		return ERR_INV_HWND;
	}

	if(!InitWindowRgn((HWND)pWin)){
		printerror("init window clip region error!");
		return ERR_INV_HWND;
	}

	if(pWin->pParent->pChildHead==NULL){
		pWin->pParent->pChildHead	=pWin;
		pWin->pParent->pChildTail	=pWin;
		pWin->pNext					=NULL;
		pWin->pPrev					=NULL;

	}
	else{
		pWin->pParent->pChildHead->pPrev=pWin;
		pWin->pNext=pWin->pParent->pChildHead;
		pWin->pParent->pChildHead=pWin;
	}

	pCurWin=pWin->pParent->pChildHead;
	while(pCurWin){
		pCurWin->iZOrder=iZOrder;
		iZOrder++;
		pCurWin=pCurWin->pNext;
	}

	///////////////////////////////////////////////////////////
	pthread_create(&(pWin->threadid),NULL,
			MessageLoop,(void*)pWin);
	//detach thread
	//pthread_detach(pWin->threadid);


	return pWin;
}


void 
MessageLoop(
	void* para
)
{
	MSG msg;
	HWND hWnd;
	hWnd=(HWND)para;
	while (GetMessage(&msg,hWnd)){
		pthread_testcancel();
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


HWND 
CreateControl(
	const HWND hWnd
)
{
	PWindowsTree pCurWin;
	PWindowsTree pWin;
	int iZOrder=0;
	pWin=(PWindowsTree)hWnd;


	if(!InitWindowRgn(pWin)){
		printerror("init window clip region error!\n");
		return ERR_INV_HWND;
	}
	pWin->threadid=pthread_self();

	if(pWin->pParent->pControlHead==NULL){
		pWin->pParent->pControlHead	=pWin;
		pWin->pParent->pControlTail	=pWin;
		pWin->pNext					=NULL;
		pWin->pPrev					=NULL;

	}
	else{
		pWin->pParent->pControlHead->pPrev=pWin;
		pWin->pNext=pWin->pParent->pControlHead;
		pWin->pParent->pControlHead=pWin;
	}


	pCurWin=pWin->pParent->pControlHead;
	while(pCurWin){
		pCurWin->iZOrder=iZOrder;
		iZOrder++;
		pCurWin=pCurWin->pNext;
	}

	return pWin;
}


HWND 
CreateMenuWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	_lGUI_pStartMenuWindow=pWin;

	if(!InitWindowRgn(pWin)){
		printerror("init window clip region error!");
		return ERR_INV_HWND;
	}
	return pWin;
}

HWND
CreateImeWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	_lGUI_pImeWindow=pWin;

	if(!InitWindowRgn(pWin)){
		printerror("init window clip region error!");
		return ERR_INV_HWND;
	}
	return pWin;
}


HWND 
CreateSkbWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	_lGUI_pSkbWindow=pWin;

	if(!InitWindowRgn(pWin)){
		printerror("init window clip region error!");
		return ERR_INV_HWND;
	}
	return pWin;
}

//===========================================================================

BOOL GUIAPI 
ShowWindow(
	HWND hWnd, 
	int nCmdShow
)
{
	PWindowsTree pWin;
	int iWinType;
	pWin=(PWindowsTree)hWnd;

	iWinType=GetWinType(pWin);
	switch(iWinType){
	case WS_DESKTOP:
		ShowDesktop(pWin,nCmdShow);
		break;
	case WS_MAIN:
		ShowMain(pWin,nCmdShow);
		break;
	case WS_CHILD:
		ShowChild(pWin,nCmdShow);
		break;
	case WS_CONTROL:
		ShowControl(pWin,nCmdShow);
		break;
	case WS_IMEWIN:
		ShowImeWin(pWin,nCmdShow);
		break;
	case WS_SKBWIN:
		ShowSkbWin(pWin,nCmdShow);
		break;
	case WS_MENUWIN:
		ShowMenuWin(pWin,nCmdShow);
		break;
	}
	SendMessage(hWnd,LMSG_SHOWWINDOW,(WPARAM)nCmdShow,(LPARAM)NULL);
	return true;
}


BOOL GUIAPI 
UpdateWindow(
	HWND hWnd
)
{
	PWindowsTree pWin;
	int iWinType;
	pWin=(PWindowsTree)hWnd;
	iWinType=GetWinType(pWin);
	switch(iWinType){
	case WS_DESKTOP:
		UpdateDesktop(pWin);
		break;
	case WS_MAIN:
		UpdateMain(pWin);
		break;
	case WS_CHILD:
		UpdateChild(pWin);
		break;
	case WS_CONTROL:
		UpdateControl(pWin);
		break;
	case WS_IMEWIN:
		UpdateImeWin(pWin);
		break;
	case WS_SKBWIN:
		UpdateSkbWin(pWin);
		break;
	case WS_MENUWIN:
		UpdateMenuWin(pWin);
		break;
	}
	return true;
}


BOOL 
ShowDesktop(
	const HWND hWnd,
	int nCmdShow
)
{
	PWindowsTree pControl;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	//set desktop visible
	pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;

	ReCalClipRegion(pWin);

	pControl=pWin->pControlHead;
	while(pControl){
		ReCalClipRegion(pControl);
		pControl=pControl->pNext;
	}

	ActiveWindow(pWin);

	scrInvalidateRect((HWND)pWin, NULL, true);

	pControl=pWin->pControlHead;
	while(pControl){
		if(IsVisible((HWND)pControl)){
			scrInvalidateRect((HWND)pControl, NULL, true);

		}
		pControl=pControl->pNext;
	}
	return true;
}


void 
UpdateDesktop(
	const HWND hWnd
)
{
	PWindowsTree pControl;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	if(!IsEmptyInvalidRegion(pWin->pInvRgn))
		SendMessage((HWND)pWin,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
	pControl=pWin->pControlHead;
	while(pControl){
		if(!IsEmptyInvalidRegion(pControl->pInvRgn))
			SendMessage(pControl,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
		pControl=pControl->pNext;
	}
}


BOOL 
ShowMain(
	const HWND hWnd,
	int nCmdShow
)
{
	PWindowsTree pChild,pControl;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	if(nCmdShow){
		pChild=pWin->pChildHead;
		if(pChild){
			ActiveWindow((HWND)pChild);
		}
		else
			ActiveWindow((HWND)pWin);
	}
	else
		DisactiveWindow((HWND)pWin);

	if(nCmdShow){
		SendMsgByClient(LMSG_IPC_SHOWMAINWIN,NULL,0);
		sem_wait(&_lGUI_CurStat->sem_cs);
		//LockMutexForSynchro();

		//set window visible
		pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;
		//get clip area from server
		//for there are maybe skb or ime window.

		ReCalClipRegion(pWin);
		pChild=pWin->pChildHead;
		while(pChild){
   			ReCalClipRegion(pChild);
			pControl=pChild->pControlHead;
			while(pControl){
				ReCalClipRegion(pControl);
				pControl=pControl->pNext;
			}
			pChild=pChild->pNext;
		}
		pControl=pWin->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl=pControl->pNext;
		}
	}
	else{
		SendMsgByClient(LMSG_IPC_HIDEMAINWIN,NULL,0);
	//	pthread_mutex_lock(&_lGUI_CurStat->mutex_cs);
		pWin->dwStyle = pWin->dwStyle & ~WS_VISIBLE;
		EmptyClipRegion(pWin->pClipRgn);
		pChild=pWin->pChildHead;
		while(pChild){
			EmptyClipRegion(pChild->pClipRgn);
			pControl=pChild->pControlHead;
			while(pControl){
				EmptyClipRegion(pControl->pClipRgn);
				pControl=pControl->pNext;
			}
			pChild=pChild->pNext;
		}
		pControl=pWin->pControlHead;
		while(pControl){
			EmptyClipRegion(pControl->pClipRgn);
			pControl=pControl->pNext;
		}
	}

	if(nCmdShow){
		scrInvalidateRect((HWND)pWin, NULL,true);
		pChild=pWin->pChildHead;
		while(pChild){
			if(IsVisible((HWND)pChild)){
				scrInvalidateRect(pChild, NULL,true);
				pControl=pChild->pControlHead;
				while(pControl){
					if(IsVisible((HWND)pControl))
						scrInvalidateRect(pControl, NULL,true);
					pControl=pControl->pNext;
				}
			}
			pChild=pChild->pNext;
		}
		pControl=pWin->pControlHead;
		while(pControl){
			if(IsVisible((HWND)pControl))
				scrInvalidateRect(pControl,NULL,true);
			pControl=pControl->pNext;
		}
		//SendMsgByClient(LMSG_IPC_SHOWMAINWIN,NULL,0);
	}
	else{
		//SendMsgByClient(LMSG_IPC_HIDEMAINWIN,NULL,0);
	}

	return true;
}

void 
UpdateMain(
	const HWND hWnd
)
{
	PWindowsTree pChild,pControl;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	if((pWin->dwStyle & WS_VISIBLE) && !IsEmptyInvalidRegion(pWin->pInvRgn)){
		SendMessage((HWND)pWin,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
		pControl=pWin->pControlHead;
		while(pControl){
			SendMessage((HWND)pControl,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
			pControl=pControl->pNext;
		}
		pChild=pWin->pChildHead;
		while(pChild){
			SendMessage((HWND)pChild,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
			pControl=pChild->pControlHead;
			while(pControl){
				SendMessage((HWND)pControl,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
				pControl=pControl->pNext;
			}
			pChild=pChild->pNext;
		}
	}
}


//show child will not change the visible attribute of its control
//but clip region of its control and its parent may be changed.
BOOL 
ShowChild(
	const HWND hWnd,
	int nCmdShow
)
{
	PWindowsTree pBrother, pControl;
	PWindowsTree pWin;
	
	pWin=(PWindowsTree)hWnd;

	//Set Active Window
	if(nCmdShow){
		ActiveWindow((HWND)pWin);
	}
	else{
		if(IsActive((HWND)pWin)){
			//Set it's brother active
			if(pWin->pNext)
				ActiveWindow((HWND)(pWin->pNext));
			else
				ActiveWindow((HWND)(pWin->pParent));
		}
	}

	if(nCmdShow){
		MoveWindowTopMost(hWnd);

		pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;
		ReCalClipRegion(pWin);
		SubtractClipRegion(pWin->pParent->pClipRgn,&(pWin->rect));
		pControl = pWin->pParent->pControlHead;
		while(pControl){
			SubtractClipRegion(pControl->pClipRgn,&(pWin->rect));
			pControl = pControl->pNext;
		}

		pBrother=pWin->pNext;
		while(pBrother){
			if(IsIntersect(&(pWin->rect),&(pBrother->rect))){
				SubtractClipRegion(pBrother->pClipRgn,&(pWin->rect));
				pControl = pBrother->pControlHead;
				while(pControl){
					SubtractClipRegion(pControl->pClipRgn,&(pWin->rect));
					pControl = pControl->pNext;
				}
			}
			pBrother=pBrother->pNext;
		}
		pControl=pWin->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl=pControl->pNext;
		}
		//clipped by control
	        ReCalClipChild(pWin);
	}
	else{
		pWin->dwStyle = pWin->dwStyle & ~WS_VISIBLE;
		EmptyClipRegion(pWin->pClipRgn);
		pControl=pWin->pControlHead;
		while(pControl){
			EmptyClipRegion(pControl->pClipRgn);
			pControl=pControl->pNext;
		}
		ReCalClipRegion(pWin->pParent);
		pBrother=pWin->pNext;
		while(pBrother){
			if(IsIntersect(&(pWin->rect),&(pBrother->rect))){
				ReCalClipRegion(pBrother);
				pControl = pBrother->pControlHead;
				while(pControl){
					ReCalClipRegion(pControl);
					pControl = pControl->pNext;
				}
			}
			pBrother=pBrother->pNext;	//Set Active Window
		}
	}

	if(nCmdShow){
		ActiveWindow((HWND)pWin);
	}
	else{
		if(IsActive((HWND)pWin)){
			//Set it's brother active
			if(pWin->pNext)
				ActiveWindow((HWND)(pWin->pNext));
			else
				ActiveWindow((HWND)(pWin->pParent));
		}
	}

	if(nCmdShow){
		scrInvalidateRect((HWND)pWin,NULL,true);
		pControl=pWin->pControlHead;
		while(pControl){
			if(IsVisible((HWND)pControl))
				scrInvalidateRect(pControl,NULL,true);
			pControl=pControl->pNext;
		}
	}
	else
		RePaintCoveredWindow(pWin);

	return true;
}


void 
UpdateChild(
	const HWND hWnd
)
{
	PWindowsTree pControl;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	if((pWin->dwStyle & WS_VISIBLE) && !IsEmptyInvalidRegion(pWin->pInvRgn)){
		SendMessage((HWND)pWin,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
		pControl=pWin->pControlHead;
		while(pControl){
			SendMessage((HWND)pControl,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
			pControl=pControl->pNext;
		}
	}

}


BOOL 
ShowControl(
	const HWND hWnd,
	int nCmdShow
)
{
	PWindowsTree pBrother;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(nCmdShow&&(pWin->dwStyle&WS_VISIBLE))
		return;
	if(!nCmdShow&&!(pWin->dwStyle&WS_VISIBLE))
		return;
	if(nCmdShow){
		pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;
		//MoveWindowTopMost(hWnd);
		//do not move to top most otherwise it will lead a error of deskicon
		//when scroll up /down
		ReCalClipRegion(pWin);
		SubtractClipRegion(pWin->pParent->pClipRgn,&(pWin->rect));
		pBrother=pWin->pNext;
		while(pBrother){
			if(IsIntersect(&(pWin->rect),&(pBrother->rect)))
				SubtractClipRegion(pBrother->pClipRgn,&(pWin->rect));
			pBrother=pBrother->pNext;
		}
	}
	else{
		pWin->dwStyle = pWin->dwStyle & ~WS_VISIBLE;
		EmptyClipRegion(pWin->pClipRgn);
		ReCalClipRegion(pWin->pParent);
		pBrother=pWin->pNext;
		while(pBrother){
			if(IsIntersect(&(pWin->rect),&(pBrother->rect)))
				ReCalClipRegion(pBrother);
			pBrother=pBrother->pNext;
		}
		ReCalClipChild(pWin->pParent);
	}

	if(nCmdShow){
		scrInvalidateRect((HWND)pWin,NULL,true);
	}
	else{
		RePaintCoveredWindow(pWin);
	}
	return true;
}


void 
UpdateControl(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	if((pWin->dwStyle & WS_VISIBLE) && !IsEmptyInvalidRegion(pWin->pInvRgn))
		SendMessage((HWND)pWin,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);//显示客户区
}


BOOL 
ShowImeWin(
	const HWND hWnd,
	int nCmdShow
)
{
	PlGUIAppStat pCurStat;
	PWindowsTree  pControl;
	PWindowsTree pWin;
	RECT rc,*rc1;
	pWin=(PWindowsTree)hWnd;
	CopyRect(&rc,&(pWin->rect));

	if(nCmdShow){
		pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;
		SetInitRectClipRegion(pWin->pClipRgn,&rc);
		SubtractClipRegion(_lGUI_pWindowsTree->pClipRgn,&rc);
		pControl = _lGUI_pWindowsTree->pControlHead;
		while(pControl){
			SubtractClipRegion(pControl->pClipRgn,&rc);
			pControl = pControl->pNext;
		}
		pCurStat=_lGUI_pAppStat;
		while(pCurStat){
			if(IsIntersect(&(pCurStat->pClipRgn->rcBound),&rc)){
				SubtractClipRegion(pCurStat->pClipRgn, &rc);
				if(pCurStat->bVisible)
					SendClipRgnChangeMsg(pCurStat);
			}
			pCurStat = pCurStat->pNext;
		}
		//paint
		winInvalidateRect(hWnd,NULL,true);
	}
	else{
		pWin->dwStyle = pWin->dwStyle & ~WS_VISIBLE;
		EmptyClipRegion(pWin->pClipRgn);
		ReCalClipRegion(_lGUI_pWindowsTree);
		InvalidateRect((HWND)_lGUI_pWindowsTree,NULL,true);
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl=pControl->pNext;
		}
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			scrInvalidateRect((HWND)pControl,NULL,true);
			pControl=pControl->pNext;
		}

		pCurStat = _lGUI_pAppStat;
		RecalClipRgnUnderThis(pCurStat);
		while(pCurStat){
			SendClipRgnChangeMsg(pCurStat);
			pCurStat=pCurStat->pNext;
		}
		pCurStat = _lGUI_pAppStat;
		while(pCurStat){
			rc1=malloc(sizeof(RECT));
			memcpy(rc1,&rc,sizeof(RECT));
			SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);
			//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&rc,sizeof(RECT));
			pCurStat=pCurStat->pNext;
		}
	}
	return true;
}


void 
UpdateImeWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_VISIBLE) && !IsEmptyInvalidRegion(pWin->pInvRgn))
		SendMessage((HWND)pWin,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
	//else
	//	SyncPaintCoveredWindow(pWin);
}


BOOL 
ShowSkbWin(
	const HWND hWnd,
	int nCmdShow
)
{
	PlGUIAppStat pCurStat;
	PWindowsTree pControl;
	PWindowsTree pWin;
	RECT rc,*rc1;
	pWin=(PWindowsTree)hWnd;
	CopyRect(&rc,&(pWin->rect));

	if(nCmdShow){
		pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;
		SetInitRectClipRegion(pWin->pClipRgn,&rc);
		SubtractClipRegion(_lGUI_pWindowsTree->pClipRgn,&rc);
		pControl = _lGUI_pWindowsTree->pControlHead;
		while(pControl){
			SubtractClipRegion(pControl->pClipRgn,&rc);
			pControl = pControl->pNext;
		}
		pCurStat=_lGUI_pAppStat;
		while(pCurStat){
			if(IsIntersect(&(pCurStat->pClipRgn->rcBound),&rc)){
				SubtractClipRegion(pCurStat->pClipRgn, &rc);
				if(pCurStat->bVisible)
					SendClipRgnChangeMsg(pCurStat);
			}
			pCurStat = pCurStat->pNext;
		}
		//paint
		winInvalidateRect(hWnd,NULL,true);
	}
	else{
		pWin->dwStyle = pWin->dwStyle & ~WS_VISIBLE;
		EmptyClipRegion(pWin->pClipRgn);
		ReCalClipRegion(_lGUI_pWindowsTree);
		InvalidateRect((HWND)_lGUI_pWindowsTree,NULL,true);
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl=pControl->pNext;
		}
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			scrInvalidateRect((HWND)pControl,NULL,true);
			pControl=pControl->pNext;
		}

		pCurStat = _lGUI_pAppStat;
		RecalClipRgnUnderThis(pCurStat);
		while(pCurStat){
			SendClipRgnChangeMsg(pCurStat);
			pCurStat=pCurStat->pNext;
		}
		pCurStat = _lGUI_pAppStat;
		while(pCurStat){
			rc1=malloc(sizeof(RECT));
			memcpy(rc1,&rc,sizeof(RECT));
			SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);
			//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
			//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,(void*)&rc,sizeof(RECT));
			//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
			pCurStat=pCurStat->pNext;
		}
	}
	return true;
}


void 
UpdateSkbWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_VISIBLE) && !IsEmptyInvalidRegion(pWin->pInvRgn))
		SendMessage((HWND)pWin,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
}


BOOL 
ShowMenuWin(
	const HWND hWnd,
	int nCmdShow
)
{
	PlGUIAppStat pCurStat;
	PWindowsTree pControl;
	PWindowsTree pWin;
	RECT rc,*rc1;
	pWin=(PWindowsTree)hWnd;
	CopyRect(&rc,&(pWin->rect));

	if(nCmdShow){
		pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;
		SetInitRectClipRegion(pWin->pClipRgn,&rc);
		SubtractClipRegion(_lGUI_pWindowsTree->pClipRgn,&rc);
		pControl = _lGUI_pWindowsTree->pControlHead;
		while(pControl){
			SubtractClipRegion(pControl->pClipRgn,&rc);
			pControl = pControl->pNext;
		}
		pCurStat=_lGUI_pAppStat;
		while(pCurStat){
			if(IsIntersect(&(pCurStat->pClipRgn->rcBound),&rc)){
				SubtractClipRegion(pCurStat->pClipRgn, &rc);
				if(pCurStat->bVisible)
					SendClipRgnChangeMsg(pCurStat);
			}
			pCurStat = pCurStat->pNext;
		}
		winInvalidateRect(hWnd,NULL,true);
	}
	else{
		pWin->dwStyle = pWin->dwStyle & ~WS_VISIBLE;
		EmptyClipRegion(pWin->pClipRgn);
		ReCalClipRegion(_lGUI_pWindowsTree);
		InvalidateRect((HWND)_lGUI_pWindowsTree,NULL,true);
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl=pControl->pNext;
		}
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			scrInvalidateRect((HWND)pControl,NULL,true);

			pControl=pControl->pNext;
		}
		pCurStat = _lGUI_pAppStat;
		RecalClipRgnUnderThis(pCurStat);
		while(pCurStat){
			SendClipRgnChangeMsg(pCurStat);
			pCurStat=pCurStat->pNext;
		}
		pCurStat = _lGUI_pAppStat;
		while(pCurStat){
			rc1=malloc(sizeof(RECT));
			memcpy(rc1,&rc,sizeof(RECT));
			SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);
			//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
			//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&rc,sizeof(RECT));
			pCurStat=pCurStat->pNext;
		}
	}
	return true;
}


void 
UpdateMenuWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_VISIBLE) && !IsEmptyInvalidRegion(pWin->pInvRgn))
		SendMessage((HWND)pWin,LMSG_PAINT,(WPARAM)NULL,(LPARAM)NULL);
	//else
	//	SyncPaintCoveredWindow(pWin);
}


//Destroywindow function will destroy sub window and control of this window
void GUIAPI DestroyWindow(HWND hWnd)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin){
		printerror("null window pointer!");
		return;
	}
	
	switch(GetWinType(pWin)){
	case WS_DESKTOP:
	case WS_MAIN:
		SendMessage(hWnd,LMSG_DESTROY,(WPARAM)NULL,(LPARAM)NULL);
		break;
	case WS_CHILD:
		SendMessage(hWnd,LMSG_DESTROY,(WPARAM)NULL,(LPARAM)NULL);
		break;
	case WS_CONTROL:
		DestroyControl(pWin);
		break;
		break;
	case WS_IMEWIN:
		DestroyImeWin(pWin);
		break;
	case WS_SKBWIN:
		DestroySkbWin(pWin);
		break;
	case WS_MENUWIN:
		DestroyMenuWin(pWin);
		break;
	}
}

//call this function to destroy window data structure
//it will be called by getmessage in the module message.c
void 
ExitWindow(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	switch(GetWinType(pWin)){
	case WS_DESKTOP:
		DestroyDesktop(pWin);
		break;
	case WS_MAIN:
		DestroyMain(pWin);
		break;
	case WS_CHILD:
		DestroyChild(pWin);
		break;
	}
}

void DestroyWindowRes(PWindowsTree pWin){
	EmptyClipRegion(pWin->pClipRgn);
	EmptyInvalidRegion(pWin->pInvRgn);
	EmptyClipRegion(pWin->pBackClipRgn);
	free(pWin->pBackClipRgn);
	free(pWin->pClipRgn);
	free(pWin->pInvRgn);
	if(pWin->pMsgQueue)
		DestroyMsgQueue((HWND)pWin);

	//free the scroll struct
	if(pWin->pVScroll)
		free(pWin->pVScroll);
	if(pWin->pHScroll)
		free(pWin->pHScroll);

	if(pWin->pHCurState)
		free(pWin->pHCurState);

	if(pWin->pVCurState)
		free(pWin->pVCurState);

	free(pWin);
}

void 
DestroyDesktop(
	HWND hWnd
)
{
	PlGUIAppStat pStat;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	if(_lGUI_pAppStat){
		pStat=_lGUI_pAppStat->pNext;
		while(_lGUI_pAppStat){
			SendMsgByServer(pStat,LMSG_IPC_EXITSERVER,NULL,0);
			pStat=_lGUI_pAppStat->pNext;
		}
	}
	DestroyWindowRes(pWin);
}


void 
DestroyMain(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	//Destroy Child Window
//	ShowWindow(hWnd,false);
	SendMsgByClient(LMSG_IPC_DESTROYAPP,NULL,0);
	sem_wait(&_lGUI_CurStat->sem_cs);
	//LockMutexForSynchro();
	while(pWin->pChildHead)
		DestroyChild_Process(pWin->pChildHead,true);
	//Destroy Control
	while(pWin->pControlHead)
		DestroyControl_Process(pWin->pControlHead,true);
	DestroyWindowRes(pWin);
}

void 
DestroyChild(
	HWND hWnd
)
{
	DestroyChild_Process(hWnd,false);
}

void 
DestroyChild_Process(
	HWND hWnd,
	BOOL bByParent
)
{
	PWindowsTree pBrother;
	PWindowsTree pWin;
	PWindowsTree pParent;
	PWindowsTree pControl;
	RECT rect,rc;

	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return;

	pParent=pWin->pParent;
	pBrother=pWin->pNext;

	if(bByParent){
		if(pParent->pChildHead==pParent->pChildTail){
			pParent->pChildHead=NULL;
			pParent->pChildTail=NULL;
		}
		else if(pWin==pParent->pChildHead){
			pWin->pNext->pPrev=NULL;
			pParent->pChildHead=pWin->pNext;
		}
		else if(pWin==pParent->pChildTail){
			pWin->pPrev->pNext=NULL;
			pParent->pChildTail=pWin->pPrev;
		}
		else{
			pWin->pPrev->pNext=pWin->pNext;
			pWin->pNext->pPrev=pWin->pPrev;
		}
		pWin->pParent=NULL;
		PostQuitMessage(hWnd);//用来退出消息循环
		return;
	}

	while(pWin->pControlHead)
		DestroyControl_Process((HWND)(pWin->pControlHead),true);
	//Set Active Window
	if(pParent!=NULL){
		if(IsActive(hWnd)){
			_lGUI_pActiveWin = NULL;
			if(pBrother){
				ActiveWindow(pBrother);
			}
			else{
				ActiveWindow(pParent);
			}
		}

		if(pParent->pChildHead==pParent->pChildTail){
			pParent->pChildHead=NULL;
			pParent->pChildTail=NULL;
		}
		else if(pWin==pParent->pChildHead){
			pWin->pNext->pPrev=NULL;
			pParent->pChildHead=pWin->pNext;
		}
		else if(pWin==pParent->pChildTail){
			pWin->pPrev->pNext=NULL;
			pParent->pChildTail=pWin->pPrev;
		}
		else{
			pWin->pPrev->pNext=pWin->pNext;
			pWin->pNext->pPrev=pWin->pPrev;
		}
			ReCalClipRegion(pParent);
			pControl = pParent->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl = pControl->pNext;
		}
		while(pBrother){
			if(IsIntersect(&(pWin->rect),&(pBrother->rect)))
				ReCalClipRegion(pBrother);
			pControl = pBrother->pControlHead;
			while(pControl){
				ReCalClipRegion(pControl);
				pControl = pControl->pNext;
			}
			//clipped by control
			scrGetClientRect((HWND)pBrother,&rect);
			pControl=pBrother->pControlHead;
			while(pControl){
				if(!IsHideWindow((HWND)pControl)){
					IntersectRect(&rc,&(pControl->rect),&rect);
					SubtractClipRegion(pBrother->pClipRgn,&rc);
				}
				pControl=pControl->pNext;
			}
			pBrother=pBrother->pNext;
		}
		
		RePaintCoveredWindow(pWin);
	}
	DestroyWindowRes(pWin);
}

void 
DestroyControl(
	HWND hWnd
)
{
	DestroyControl_Process(hWnd,false);
}


void 
DestroyControl_Process(
	HWND hWnd,
	BOOL bByParent
)
{
	PWindowsTree pBrother;
	PWindowsTree pWin;
	PWindowsTree pParent;
	BOOL bFocus;
	RECT rect;

	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return;
	pParent = pWin->pParent;
	pBrother = pWin->pNext;

	CopyRect(&rect,&(pWin->rect));

	bFocus = IsFocus(hWnd);

	SendMessage(hWnd,LMSG_DESTROY,(WPARAM)NULL,(LPARAM)NULL);
	if(pParent->pControlHead==pParent->pControlTail){
		pParent->pControlHead=NULL;
		pParent->pControlTail=NULL;
	}
	else if(pWin==pParent->pControlHead){
		pWin->pNext->pPrev=NULL;
		pParent->pControlHead=pWin->pNext;
	}
	else if(pWin==pParent->pControlTail){
		pWin->pPrev->pNext=NULL;
		pParent->pControlTail=pWin->pPrev;
	}
	else{
		pWin->pPrev->pNext=pWin->pNext;
		pWin->pNext->pPrev=pWin->pPrev;
	}
	DestroyWindowRes(pWin);
	if(!bByParent){
		ReCalClipRegion(pParent);
		while(pBrother){
			if(IsIntersect(&rect,&(pBrother->rect)))
				ReCalClipRegion(pBrother);
			pBrother=pBrother->pNext;
		}
		RePaintCoveredWindow(pWin);
		if(bFocus)
			_lGUI_pFocus = NULL;
	}
}


void 
DestroyMenuWin(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PWindowsTree pControl;
	PlGUIAppStat pCurStat;
	RECT rc,*rc1;
	pWin=(PWindowsTree)hWnd;
	CopyRect(&rc,&(pWin->rect));
	

	ReCalClipRegion(_lGUI_pWindowsTree);

	InvalidateRect((HWND)_lGUI_pWindowsTree,NULL,true);

	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		ReCalClipRegion(pControl);
		pControl=pControl->pNext;
	}

	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		if(IsVisible(pControl))
			InvalidateRect((HWND)pControl,NULL,true);
		pControl=pControl->pNext;
	}
	//imewindow
	if(IsVisible((HWND)_lGUI_pImeWindow))
		if(IsIntersect(&_lGUI_pImeWindow->rect,&pWin->rect))
			scrInvalidateRect((HWND)_lGUI_pImeWindow,&pWin->rect,true);

	//skbwindow
	if(IsVisible((HWND)_lGUI_pSkbWindow))
		if(IsIntersect(&_lGUI_pSkbWindow->rect,&pWin->rect))
			scrInvalidateRect((HWND)_lGUI_pSkbWindow,&pWin->rect,true);


	pCurStat = _lGUI_pAppStat;
	RecalClipRgnUnderThis(pCurStat);

	while(pCurStat){
		SendClipRgnChangeMsg(pCurStat);
		pCurStat=pCurStat->pNext;
	}

	pCurStat = _lGUI_pAppStat;
	while(pCurStat){
		rc1=malloc(sizeof(RECT));
		memcpy(rc1,&rc,sizeof(RECT));
		SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);
		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&rc,sizeof(RECT));

		pCurStat=pCurStat->pNext;
	}
	DestroyWindowRes(pWin);
	_lGUI_pStartMenuWindow = NULL;
}

void
DestroyImeWin(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PWindowsTree pControl;
	PlGUIAppStat pCurStat;
	RECT rc,*rc1;
	pWin=(PWindowsTree)hWnd;
	CopyRect(&rc,&(pWin->rect));
	ReCalClipRegion(_lGUI_pWindowsTree);
	InvalidateRect((HWND)_lGUI_pWindowsTree,NULL,true);
	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		ReCalClipRegion(pControl);
		pControl=pControl->pNext;
	}
	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		if(IsVisible(pControl))
			InvalidateRect((HWND)pControl,NULL,true);
		pControl=pControl->pNext;
	}

	pCurStat = _lGUI_pAppStat;
	RecalClipRgnUnderThis(pCurStat);
	while(pCurStat){
		SendClipRgnChangeMsg(pCurStat);
		pCurStat=pCurStat->pNext;
	}
	pCurStat = _lGUI_pAppStat;
	while(pCurStat){
		rc1=malloc(sizeof(RECT));
		memcpy(rc1,&rc,sizeof(RECT));
		SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);

		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&rc,sizeof(RECT));
		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&rc,sizeof(RECT));
		pCurStat=pCurStat->pNext;
	}
	DestroyWindowRes(pWin);
	_lGUI_pImeWindow = NULL;
}


void 
DestroySkbWin(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PWindowsTree pControl;
	PlGUIAppStat pCurStat;
	RECT rc,*rc1;
	pWin=(PWindowsTree)hWnd;
	CopyRect(&rc,&(pWin->rect));
	ReCalClipRegion(_lGUI_pWindowsTree);
	InvalidateRect((HWND)_lGUI_pWindowsTree,NULL,true);
	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		ReCalClipRegion(pControl);
		pControl=pControl->pNext;
	}
	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		if(IsVisible(pControl))
			InvalidateRect((HWND)pControl,NULL,true);
		pControl=pControl->pNext;
	}

	pCurStat = _lGUI_pAppStat;
	RecalClipRgnUnderThis(pCurStat);
	while(pCurStat){
		SendClipRgnChangeMsg(pCurStat);
		pCurStat=pCurStat->pNext;
	}
	pCurStat = _lGUI_pAppStat;
	while(pCurStat){
		rc1=malloc(sizeof(RECT));
		memcpy(rc1,&rc,sizeof(RECT));
		SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);
		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&rc,sizeof(RECT));
		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
		//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
		pCurStat=pCurStat->pNext;
	}
	DestroyWindowRes(pWin);
	_lGUI_pSkbWindow = NULL;
}

BOOL GUIAPI 
MoveWindow(
	HWND hWnd, 
	int x, 
	int y
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(IsVisible(hWnd)){
		ShowWindow(hWnd,false);
		pWin->rect.left+=x;
		pWin->rect.right+=x;
		pWin->rect.top+=y;
		pWin->rect.bottom+=y;
		ShowWindow(hWnd,true);
	}
	else{
		pWin->rect.left+=x;
		pWin->rect.right+=x;
		pWin->rect.top+=y;
		pWin->rect.bottom+=y;
	}
	return true;
}


BOOL 
MoveWindowTo(
	HWND hWnd, 
	int x,
	int y
)
{
	PWindowsTree pWin;
	int iWidth,iHeight;
	pWin=(PWindowsTree)hWnd;
	iWidth = pWin->rect.right - pWin->rect.left + 1;
	iHeight = pWin->rect.bottom - pWin->rect.top + 1;
	if(IsVisible(hWnd)){
		ShowWindow(hWnd,false);
		pWin->rect.left =x;
		pWin->rect.right =x + iWidth - 1;
		pWin->rect.top =y;
		pWin->rect.bottom =y + iHeight -1;
		ShowWindow(hWnd,true);
	}
	else{
		pWin->rect.left =x;
		pWin->rect.right =x + iWidth - 1;
		pWin->rect.top =y;
		pWin->rect.bottom =y + iHeight -1;
	}
	return true;
}
//Move Main Window
//for we can't control the show operation order of the main windows 
//when clip region changed
//this function is designed for solving this problem.
BOOL 
MoveMainWindow(
	HWND hWnd, 
	int x, 
	int y
)
{
	PWindowsTree pWin,pControl;
	PlGUIAppStat pCurStat;
	RECT rc;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;

	pWin->rect.left+=x;
	pWin->rect.right+=x;
	pWin->rect.top+=y;
	pWin->rect.bottom+=y;

	CopyRect(&rc,&pWin->rect);
	if(IsVisible(hWnd)){

		pWin->dwStyle = pWin->dwStyle | WS_VISIBLE;

		SetInitRectClipRegion(pWin->pClipRgn,&(pWin->rect));


		ReCalClipRegion(_lGUI_pWindowsTree);

		InvalidateRect((HWND)_lGUI_pWindowsTree,NULL,true);

		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl=pControl->pNext;
		}

		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			scrInvalidateRect((HWND)pControl,NULL,true);
			pControl=pControl->pNext;
		}



		pCurStat = _lGUI_pAppStat;
		RecalClipRgnUnderThis(pCurStat);

		while(pCurStat){
			SendClipRgnChangeMsg(pCurStat);
			pCurStat=pCurStat->pNext;
		}

		pCurStat = _lGUI_pAppStat;
		while(pCurStat){
			memcpy(&(pCurStat->invd_rc),&rc,sizeof(RECT));
			SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
			//SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
		//	SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&rc,sizeof(RECT));
		//	SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
			
			pCurStat=pCurStat->pNext;
		}

		//paint
		winInvalidateRect(hWnd,NULL,true);
	}
	return true;
}


BOOL GUIAPI 
ResizeWindow(
	HWND hWnd,
	LPRECT lpRect
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;
	if(IsVisible(hWnd)){
		ShowWindow(hWnd,false);
		memcpy((void*)&(pWin->rect),(void*)lpRect,sizeof(RECT));
		ShowWindow(hWnd,true);
	}
	else{
		memcpy((void*)&(pWin->rect),(void*)lpRect,sizeof(RECT));
	}
	return true;
}

