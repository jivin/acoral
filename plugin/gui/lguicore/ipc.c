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
#include "../include/lguiapp.h"
#include "../include/ipc.h"
#include "../include/mouse.h"


extern PWindowsTree		_lGUI_pActiveWin;
extern PWindowsTree		_lGUI_pFocus;
extern PlGUIAppStat		_lGUI_pAppStat;
//extern WndCaptureMouse _lGUI_wndCaptureMouse;
extern PlGUIAppStat _lGUI_pAppStat;
extern PWindowsTree _lGUI_pImeWindow;
extern PWindowsTree _lGUI_pSkbWindow;

//two global variable
//listen thread for server
//connect thread for client

//identify of listen socket for server
//identify of connect socket for client
extern PlGUIAppStat _lGUI_ServerStat;
#define SelfHand _lGUI_pWindowsTree
void 
TerminateIpc()
{
}


//Initial listen socket and create thread for server
BOOL
InitIpcServer()
{
	return true;
}

BOOL 
SendMsgByClient(
	int iMsg,
	void* pAttachData,
	int pAttachSize
){
	if(pAttachSize==0){
		SendNotifyMessage((HWND)_lGUI_ServerStat->hand,iMsg,(WPARAM)SelfHand,(LPARAM)pAttachData);
		return true;
	}

	if(pAttachSize<=sizeof(LPARAM)){
		SendNotifyMessage((HWND)_lGUI_ServerStat->hand,iMsg,(WPARAM)SelfHand,*((LPARAM *)pAttachData));
		return true;
	}
	return false;
}

BOOL 
SendMsgByServer(
	PlGUIAppStat pStat,
	int iMsg,
	void* pAttachData,
	int pAttachSize
)
{
	if(!pStat->hand)
		return false;

	if(pAttachSize==0){
		SendNotifyMessage((HWND)pStat->hand,iMsg,(WPARAM)pAttachData,(LPARAM)0);
		return true;
	}
	if(pAttachSize<=sizeof(WPARAM)){
		SendNotifyMessage((HWND)pStat->hand,iMsg,*((WPARAM *)pAttachData),(LPARAM)0);
		return true;
	}

	if(pAttachSize<=(sizeof(WPARAM)+sizeof(LPARAM))){
		SendNotifyMessage((HWND)pStat->hand,iMsg,*((WPARAM *)pAttachData),*((LPARAM *)((WPARAM *)pAttachData+1)));
		return true;
	}
	return false;
}

BOOL 
ProcessIpcMessageServer(
	IpcHand hand,
	int iMsg,  
	void* pAttachData 
)
{
	switch (iMsg){
		case LMSG_IPC_CREATEAPP:
			//create application
			CreateApplicationServer(hand,pAttachData);
			//UnLockMutexForSynchro();
			break;
		case LMSG_IPC_DESTROYAPP:
			//destory application
			DestroyApplicationServer(hand,pAttachData);
			//UnLockMutexForSynchro();
			break;
		case LMSG_IPC_SHOWMAINWIN:
			//Show Main Window
			ShowMainWinServer(hand, pAttachData);
			//UnLockMutexForSynchro();
			break;
		case LMSG_IPC_HIDEMAINWIN:
			//Hide Main Window
			HideMainWinServer(hand, pAttachData);
			break;
	}
	return true;
}

BOOL 
CreateApplicationServer(
	IpcHand hand, 
	void *pAttachData
)
{
	PlGUIAppStat pStat;
	pStat =(PlGUIAppStat)pAttachData;
	if(!pStat)
		return false;
	if(!_lGUI_pAppStat)
		_lGUI_pAppStat=pStat;
	else{
		pStat->pNext=_lGUI_pAppStat;
		_lGUI_pAppStat=pStat;
	}
	pStat->bVisible=false;//it's invisible before show operate
	sem_post(&pStat->sem_cs);
	return true;
}
//Destroy an application
//socket id
// pAttachData iAttachSize no used 
BOOL 
DestroyApplicationServer(
	IpcHand hand, 
	void *pAttachData
)
{
	RECT *rc1;
	PWindowsTree pControl;
	PlGUIAppStat pStat,pCurStat,pPrevStat=NULL;

	BOOL bTop=false;

	pStat = _lGUI_pAppStat;

	if(!pStat)
		return false;
	while(pStat){
		if(pStat->hand==hand){
			break;
		}
		pPrevStat=pStat;
		pStat=pStat->pNext;
	}
	if(!pStat){
		return false;
	}
	if(pStat == _lGUI_pAppStat){
		bTop = true;
		_lGUI_pAppStat = _lGUI_pAppStat->pNext;
	}
	else
		pPrevStat->pNext = pStat->pNext;

	if(pStat->bVisible){//only if this application is visible
		if(pStat->pNext){
			//recalculate clip region of mainwindow under the current
			RecalClipRgnUnderThis(pStat->pNext);
			//send clip region change message to application
			pCurStat=pStat->pNext;
			while(pCurStat){
				SendClipRgnChangeMsg(pCurStat);
				pCurStat=pCurStat->pNext;
			}

			//if this application is at top layer
			// then we should active the nearest application under it
			if(bTop){
				SendMsgByServer(_lGUI_pAppStat,LMSG_IPC_ACTIVEAPP,NULL,0);
			}
			//send redraw message to application
			pCurStat=pStat->pNext;
			while(pCurStat){
			//	memcpy(&pCurStat->invd_rc,&pStat->rc,sizerc1=malloc(sizeof(RECT));
				rc1=malloc(sizeof(RECT));
				memcpy(rc1,&pStat->rc,sizeof(RECT));
				SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);
			//	SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,NULL,0);
				pCurStat=pCurStat->pNext;
			}
		}

		//recalculate clip region of desktop
		ReCalClipRegion(_lGUI_pWindowsTree);
		//recalculate clip region of desktop control
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			ReCalClipRegion(pControl);
			pControl=pControl->pNext;
		}
		//redraw desktop 
		scrInvalidateRect((HWND)_lGUI_pWindowsTree,&(pStat->rc),true);
		//redraw desktop control
		pControl=_lGUI_pWindowsTree->pControlHead;
		while(pControl){
			if(IsVisible(pControl))
				scrInvalidateRect((HWND)pControl,&(pStat->rc),true);
			pControl=pControl->pNext;
		}
		//if all of application have been destroyed,we should active desktop.
		if(!_lGUI_pAppStat){
			ActiveWindow(_lGUI_pWindowsTree);
		}
	}
	sem_post(&pStat->sem_cs);
	return true;
}



//Show Main Window
//para:
// socket id
// pAttachData iAttachSize no used 
BOOL 
ShowMainWinServer(
	IpcHand hand, 
	void *pAttachData
)
{
	PlGUIAppStat pStat,pCurStat,pPrevStat,pThisStat,pActiveStat;
	PWindowsTree pControl;
	RECT rect;
	//_lGUI_pWindowsTree	
	if(IsActive((HWND)_lGUI_pWindowsTree)){//if desktop is active
		pActiveStat = NULL;
	}
	else{
		pStat=_lGUI_pAppStat;
		while(pStat){
			if(pStat->bVisible)
				break;
			pStat = pStat->pNext;
		}
		pActiveStat = pStat;
	}

	pStat = _lGUI_pAppStat;
	pPrevStat = _lGUI_pAppStat;
	while(pStat){
		if(pStat->hand==hand)
			break;
		pPrevStat = pStat;
		pStat=pStat->pNext;
	}
	if(!pStat)
		return false;
	//Move this main window node to top
	if(pStat!=_lGUI_pAppStat){
		pThisStat = pStat->pNext;

		pPrevStat->pNext = pStat->pNext;
		pStat->pNext = _lGUI_pAppStat;
		_lGUI_pAppStat = pStat;
	}
	else
		pThisStat = NULL;

	pStat->bVisible=true;
	
	//clipped by desktop rect
	GetWindowRect((HWND)_lGUI_pWindowsTree,&rect);
	IntersectRect(&(pStat->rc),&(pStat->rc),&rect);
	//Initial the clip region 
	SetInitRectClipRegion (pStat->pClipRgn, &(pStat->rc));
	//clipped by imewin and skbwin to generate initial clip region
	if(IsVisible((HWND)_lGUI_pImeWindow))
		SubtractClipRegion(pStat->pClipRgn,&_lGUI_pImeWindow->rect);
	if(IsVisible((HWND)_lGUI_pSkbWindow))
		SubtractClipRegion(pStat->pClipRgn,&_lGUI_pSkbWindow->rect);
	//clip region of all main window under this will change.
	pCurStat=_lGUI_pAppStat->pNext;
	while(pCurStat!= pThisStat){
		if(IsIntersect(&(pCurStat->pClipRgn->rcBound),&(pStat->rc))){
			SubtractClipRegion (pCurStat->pClipRgn, &(pStat->rc));
			//send clipregion change message to application
			if(pCurStat->bVisible)
				SendClipRgnChangeMsg(pCurStat);
		}
		pCurStat = pCurStat->pNext;
	}

	//send disactive message to the top application before.
	if(pActiveStat){
		SendMsgByServer(pActiveStat,LMSG_IPC_DISACTIVEAPP,NULL,0);
	}

	//calculate clipregion of desktop
	SubtractClipRegion(_lGUI_pWindowsTree->pClipRgn,&(pStat->rc));
	//calculate clip region of desktop control 
	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		SubtractClipRegion(pControl->pClipRgn,&(pStat->rc));
		pControl=pControl->pNext;
	}
	//send confirm message to the new generated application
//	SendMsgByServer(pStat,LMSG_IPC_SHOWMAINWIN_ANS,NULL,0);
	GetBoundClipRegion(pStat->pClipRgn);
	sem_post(&pStat->sem_cs);
	return true;
}

//Hide main window
//Param
//  socket id
//  pAttachData iAttachSize no used 
BOOL 
HideMainWinServer(
	IpcHand hand, 
	void *pAttachData
)
{
	RECT *rc1;
	PlGUIAppStat pStat,pCurStat;
	PWindowsTree pControl;

	pStat=_lGUI_pAppStat;
	while(pStat){
		if(pStat->hand==hand)
			break;
		pStat=pStat->pNext;
	}
	if(!pStat)
		return false;

	pStat->bVisible=false;

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

	RecalClipRgnUnderThis(pStat->pNext);
	pCurStat=pStat->pNext;
	while(pCurStat){
	//	SendMsgByServer(pStat,LMSG_IPC_CLIPRGNCHANGE,NULL,0);
		SendClipRgnChangeMsg(pStat);
		pCurStat=pCurStat->pNext;
	}
	pCurStat=pStat->pNext;
	while(pCurStat){
		rc1=malloc(sizeof(RECT));
		memcpy(rc1,&pStat->rc,sizeof(RECT));
		SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,rc1,0);
	//	memcpy(&(pCurStat->invd_rc),&rc,sizeof(RECT));
	//	SendMsgByServer(pCurStat,LMSG_IPC_REDRAW,&(pStat->rc),sizeof(RECT));

		pCurStat=pCurStat->pNext;
	}
	return true;
}

void 
SendClipRgnChangeMsg(
	PlGUIAppStat pStat
){
	GetBoundClipRegion(pStat->pClipRgn);
	SendMsgByServer(pStat,LMSG_IPC_CLIPRGNCHANGE,NULL,0);
}
//recalculate clip regioin of all application main window under current window 
void 
RecalClipRgnUnderThis(
	const PlGUIAppStat pStat
)
{
	PlGUIAppStat pCurStat,pPrevStat;
	pCurStat=pStat;
	while(pCurStat){
		SetInitRectClipRegion(pCurStat->pClipRgn,&pCurStat->rc);

		pPrevStat=_lGUI_pAppStat;
		while(pPrevStat!=pCurStat){
			if(pPrevStat->bVisible)
				SubtractClipRegion(pCurStat->pClipRgn,&(pPrevStat->rc));
			pPrevStat=pPrevStat->pNext;
		}
		pCurStat=pCurStat->pNext;
	}
	if(IsVisible(_lGUI_pImeWindow)){
		pCurStat=pStat;
		while(pCurStat){
			SubtractClipRegion(pCurStat->pClipRgn,&(_lGUI_pImeWindow->rect));
			pCurStat=pCurStat->pNext;
		}
	}
	if(IsVisible(_lGUI_pSkbWindow)){
		pCurStat=pStat;
		while(pCurStat){
			SubtractClipRegion(pCurStat->pClipRgn,&(_lGUI_pSkbWindow->rect));
			pCurStat=pCurStat->pNext;
		}
	}

}


//===============================================================================
//****************************client*********************************************
//===============================================================================

//Initial ipc socket for client
BOOL 
InitIpcClient()
{
	return true;
}

//process ipc message for client
BOOL 
ProcessIpcMessageClient(
	int iMsg,  
	void* pAttachData,
	int iAttachSize
)
{
	PWindowsTree pControl,pChild;
	RECT rect;
	WPARAM wParam;
	
	switch (iMsg & MTYPE_MASK){
	case MTYPE_IPC:
		switch(iMsg){
		case LMSG_IPC_EXITSERVER:
			//server send the message to client when exit 
			DestroyWindow((HWND)_lGUI_pWindowsTree);
			break;
		case LMSG_IPC_SHOWMAINWIN_ANS:
			//confirm message for show main window 
			//together with initial clip region 
			GetBoundClipRegion(_lGUI_pAppStat->pClipRgn);
		//	UnLockMutexForSynchro();
			break;
		case LMSG_IPC_CLIPRGNCHANGE:
			//clip region changed 
			//printf("clipchange\n");
			ReCalClipRegionApp();
			break;
		case LMSG_IPC_REDRAW:
			//redraw message
			if(pAttachData==NULL)
				memset(&rect,0,sizeof(RECT));
			else
				memcpy(&rect,pAttachData,sizeof(RECT));
			//printf("redraw x:%d t:%d y:%d b:%d\n",rect.left,rect.top,rect.right,rect.bottom);
			free(pAttachData);
			scrInvalidateRect((HWND)_lGUI_pWindowsTree,&rect,true);
			//redraw sub window
			pChild=_lGUI_pWindowsTree->pChildHead;
			while(pChild){
				scrInvalidateRect((HWND)pChild,&rect,true);
				pControl=pChild->pControlHead;
				while(pControl){
					scrInvalidateRect((HWND)pControl,&rect,true);
					//scrInvalidateRect((HWND)pControl,NULL,true);
					pControl=pControl->pNext;
				}
				pChild=pChild->pNext;
			}
			//redraw control
			pControl=_lGUI_pWindowsTree->pControlHead;
			while(pControl){
				scrInvalidateRect((HWND)pControl,&rect,true);
				//scrInvalidateRect((HWND)pControl,NULL,true);
				pControl=pControl->pNext;
			}
			break;
		case LMSG_IPC_ACTIVEAPP:
			ActiveApplication();
			break;
		case LMSG_IPC_DISACTIVEAPP:
			DisactiveApplication();
			break;
		case LMSG_IPC_CHAR:
			if(!_lGUI_pFocus)
				return false;
			else{
				//printf("%s,%d\n",(char*)pAttachData,iAttachSize);
				SendMessage(_lGUI_pFocus,LMSG_CHAR,(WPARAM)pAttachData,(LPARAM)NULL);
			}	
			break;
		case LMSG_IPC_KEYDOWN:
			if(!_lGUI_pFocus)
				return false;
			else{
				wParam = (WPARAM)pAttachData;
				SendMessage(_lGUI_pFocus,LMSG_KEYDOWN,wParam,(LPARAM)NULL);
			}
			break;
		case LMSG_IPC_PENDOWN://Mouse message 
		case LMSG_IPC_PENUP://Mouse message 
		case LMSG_IPC_PENMOVE://Mouse message 
			return ProcessMouseMsgClient(iMsg,(int)pAttachData,(int)iAttachSize);
		}
		break;

/*	case MTYPE_KEY://key board 
		//iSendMsg=(iMsg & ~MTYPE_MASK) | MTYPE_KEY;
		//printf("%s\n",(char*)pAttachData);
		if(!_lGUI_pFocus)
			return false;
		else
			PostMessage(_lGUI_pFocus,iMsg,(WPARAM)pAttachData,(LPARAM)NULL);
		break;
*/
	}
	return true;
}


BOOL 
ProcessMouseMsgClient(
	int iMsg,
	int x, 
	int y
)
{
	switch(iMsg){
	case LMSG_IPC_PENDOWN:
		ProcessMouseDownMsgClient(iMsg,x,y);
		break;
	case LMSG_IPC_PENMOVE:
		ProcessMouseMoveMsgClient(iMsg,x,y);
		break;
	case LMSG_IPC_PENUP:
		ProcessMouseUpMsgClient(iMsg,x,y);
		break;
	}
	return true;
}


BOOL 
ProcessMouseDownMsgClient(
	int iMsg, 
	int x, 
	int y
)
{
	PClipRect pCRect;
	RECT rcClient;
	PWindowsTree pWin,pControl;
	//1. within sub window
	pWin=_lGUI_pWindowsTree->pChildHead;
	while(pWin){
		//1.1within clip region of the sub window
		if(PtInRect(&(pWin->rect),x,y)){
			pCRect=pWin->pClipRgn->pHead;
			while(pCRect){
				if(PtInRect(&(pCRect->rect),x,y)){
					//Move the sub window top. 
					if(pWin->pParent->pChildHead!=pWin)
						ShowWindow((HWND)pWin,true);
					else{
						ActiveWindow((HWND)pWin);
					}
					scrGetClientRect((HWND)pWin,&rcClient);
					if(PtInRect(&rcClient,x,y))
						PostMessage(pWin,LMSG_PENDOWN,(WPARAM)x,(LPARAM)y);
					else
						PostMessage(pWin,LMSG_NCPENDOWN,(WPARAM)x,(LPARAM)y);
					return true;
				}
				pCRect=pCRect->pNext;
			}
			//1.2within the control of the sub window
			pControl=pWin->pControlHead;
			while(pControl){
				if(PtInRect(&(pControl->rect),x,y)){
					//move the sub window top 
					if(pWin->pParent->pChildHead!=pWin)
						ShowWindow((HWND)pControl,true);
					scrGetClientRect((HWND)pControl,&rcClient);
					if(PtInRect(&rcClient,x,y))
						PostMessage(pControl,LMSG_PENDOWN,(WPARAM)x,(LPARAM)y);
					else
						PostMessage(pControl,LMSG_NCPENDOWN,(WPARAM)x,(LPARAM)y);
					SetFocus((HWND)pControl);
					return true;
				}
				pControl=pControl->pNext;
			}
		}
		pWin=pWin->pNext;
	}
	//2.within the main window
	pCRect=_lGUI_pWindowsTree->pClipRgn->pHead;
	while(pCRect){
		if(PtInRect(&(pCRect->rect),x,y)){
			scrGetClientRect((HWND)_lGUI_pWindowsTree,&rcClient);
			if(PtInRect(&rcClient,x,y)){
				PostMessage(_lGUI_pWindowsTree,LMSG_PENDOWN,(WPARAM)x,(LPARAM)y);
			}
			else{
				PostMessage(_lGUI_pWindowsTree,LMSG_NCPENDOWN,(WPARAM)x,(LPARAM)y);

			}
			if(!IsActive(_lGUI_pWindowsTree))
				ActiveWindow(_lGUI_pWindowsTree);
			return true;
		}
		pCRect=pCRect->pNext;
	}
	//3.within the control of the main window 
	pWin=_lGUI_pWindowsTree->pControlHead;
	while(pWin){
		pCRect=pWin->pClipRgn->pHead;
		while(pCRect){
			if(PtInRect(&(pCRect->rect),x,y)){
				scrGetClientRect((HWND)pWin,&rcClient);
				if(PtInRect(&rcClient,x,y))
					PostMessage(pWin,LMSG_PENDOWN,(WPARAM)x,(LPARAM)y);
				else
					PostMessage(pWin,LMSG_NCPENDOWN,(WPARAM)x,(LPARAM)y);
				if(!IsActive((HWND)_lGUI_pWindowsTree)){
					RECT rc;
					_lGUI_pActiveWin = _lGUI_pWindowsTree;
					_lGUI_pWindowsTree->dwStyle |=  WS_ACTIVE;
					wndGetCaptionRect((HWND)_lGUI_pWindowsTree,&rc);
					scrInvalidateRect((HWND)_lGUI_pWindowsTree,&rc,true);				
				}
				SetFocus(pWin);
				return true;
			}
			pCRect=pCRect->pNext;
		}
		pWin=pWin->pNext;
	}
	return true;

}


BOOL 
ProcessMouseMoveMsgClient(
	int iMsg, 
	int x, 
	int y
)
{
	PClipRect pCRect;
	RECT rcClient;
	PWindowsTree pWin,pControl;
	PWindowsTree _lGUI_CurWin=_lGUI_pWindowsTree;

	//who captured the mouse 
	if(_lGUI_wndCaptureMouse->pWin){
		//if within the clip region of the window/control which captured the mouse 
		//then send message to it
		if(PtInRect(&(_lGUI_wndCaptureMouse->pWin->pClipRgn->rcBound),x,y)){
			pCRect=_lGUI_wndCaptureMouse->pWin->pClipRgn->pHead;
			while(pCRect){
				if(PtInRect(&(pCRect->rect),x,y)){
					scrGetClientRect((HWND)(_lGUI_wndCaptureMouse->pWin),&rcClient);
					if((_lGUI_wndCaptureMouse->iByWho == BYCLIENT) && PtInRect(&rcClient,x,y)){
						PostMessage(_lGUI_wndCaptureMouse->pWin,LMSG_PENMOVE,(WPARAM)x,(LPARAM)y);
						return true;
					}
					if((_lGUI_wndCaptureMouse->iByWho == BYVSCROLL) ||(_lGUI_wndCaptureMouse->iByWho == BYHSCROLL)){
						PostMessage(_lGUI_wndCaptureMouse->pWin,LMSG_NCPENMOVE,(WPARAM)x,(LPARAM)y);
						return true;
					}
				}
				pCRect=pCRect->pNext;
			}
		}
	}
	else{//if no window/control captured mouse
		//then send the message to window/control which mouse is being over it
		//1.within a sub window
		pWin=_lGUI_CurWin->pChildHead;
		while(pWin){
			if(PtInRect(&(pWin->rect),x,y)){
				//1.1 within the clip region of the sub window
				pCRect=pWin->pClipRgn->pHead;
				while(pCRect){
					if(PtInRect(&(pCRect->rect),x,y)){
						scrGetClientRect((HWND)pWin,&rcClient);
						if(PtInRect(&rcClient,x,y))
							PostMessage(pWin,LMSG_PENMOVE,(WPARAM)x,(LPARAM)y);
						else
							PostMessage(pWin,LMSG_NCPENMOVE,(WPARAM)x,(LPARAM)y);
						return true;
					}
					pCRect=pCRect->pNext;
				}
				//1.2 within controls of the sub window
				pControl=pWin->pControlHead;
				while(pControl){
					if(PtInRect(&(pControl->rect),x,y)){
						scrGetClientRect((HWND)pControl,&rcClient);
						if(PtInRect(&rcClient,x,y))
							PostMessage(pControl,LMSG_PENMOVE,(WPARAM)x,(LPARAM)y);
						else
							PostMessage(pControl,LMSG_NCPENMOVE,(WPARAM)x,(LPARAM)y);
						return true;
					}
					pControl=pControl->pNext;
				}
			}
			pWin=pWin->pNext;
		}
		//2.within clip region of main window 
		pCRect=_lGUI_CurWin->pClipRgn->pHead;
		while(pCRect){
			if(PtInRect(&(pCRect->rect),x,y)){
					scrGetClientRect((HWND)_lGUI_CurWin,&rcClient);
					if(PtInRect(&rcClient,x,y))
						PostMessage(_lGUI_CurWin,LMSG_PENMOVE,(WPARAM)x,(LPARAM)y);
					else
						PostMessage(_lGUI_CurWin,LMSG_NCPENMOVE,(WPARAM)x,(LPARAM)y);
				return true;
			}
			pCRect=pCRect->pNext;
		}
		//3.within clip region of controls of main window
		pWin=_lGUI_CurWin->pControlHead;
		while(pWin){
			pCRect=pWin->pClipRgn->pHead;
			while(pCRect){
				if(PtInRect(&(pCRect->rect),x,y)){
					scrGetClientRect((HWND)pWin,&rcClient);
					if(PtInRect(&rcClient,x,y))
						PostMessage(pWin,LMSG_PENMOVE,(WPARAM)x,(LPARAM)y);
					else
						PostMessage(pWin,LMSG_NCPENMOVE,(WPARAM)x,(LPARAM)y);

					return true;
				}
				pCRect=pCRect->pNext;
			}
			pWin=pWin->pNext;
		}
	}
	return true;
}


BOOL 
ProcessMouseUpMsgClient(
	int iMsg, 
	int x,
	int y
)
{
	PClipRect pCRect;
	RECT rcClient;
	PWindowsTree pWin,pControl;


	if(_lGUI_wndCaptureMouse->pWin){
		//send the message to which captured mouse before.
		if(_lGUI_wndCaptureMouse->iByWho == BYCLIENT){
			PostMessage((HWND)_lGUI_wndCaptureMouse->pWin,LMSG_PENUP,(WPARAM)x,(LPARAM)y);
		}
		else{
			PostMessage((HWND)_lGUI_wndCaptureMouse->pWin,LMSG_NCPENUP,(WPARAM)x,(LPARAM)y);
		}

	}
	else{
		//if no window/control captured mouse before
		//then send the message to the control/window where mouse put up.
		//1.within a sub window
		pWin=_lGUI_pWindowsTree->pChildHead;
		while(pWin){
			if(PtInRect(&(pWin->rect),x,y)){
				//1.1 within clip region of the sub window
				pCRect=pWin->pClipRgn->pHead;
				while(pCRect){
					if(PtInRect(&(pCRect->rect),x,y)){
						scrGetClientRect((HWND)pWin,&rcClient);
						if(PtInRect(&rcClient,x,y))
							PostMessage(pWin,LMSG_PENUP,(WPARAM)x,(LPARAM)y);
						else
							PostMessage(pWin,LMSG_NCPENUP,(WPARAM)x,(LPARAM)y);
						return true;
					}
					pCRect=pCRect->pNext;
				}
				//1.2 within controls of the sub window
				pControl=pWin->pControlHead;
				while(pControl){
					if(PtInRect(&(pControl->rect),x,y)){
						scrGetClientRect((HWND)pControl,&rcClient);
						if(PtInRect(&rcClient,x,y))
							PostMessage(pControl,LMSG_PENUP,(WPARAM)x,(LPARAM)y);
						else
							PostMessage(pControl,LMSG_NCPENUP,(WPARAM)x,(LPARAM)y);
						return true;
					}
					pControl=pControl->pNext;
				}
			}
			pWin=pWin->pNext;
		}
		//2.within clip region of main window
		pCRect=_lGUI_pWindowsTree->pClipRgn->pHead;
		while(pCRect){
			if(PtInRect(&(pCRect->rect),x,y)){
				scrGetClientRect((HWND)_lGUI_pWindowsTree,&rcClient);
				if(PtInRect(&rcClient,x,y))
					PostMessage(_lGUI_pWindowsTree,LMSG_PENUP,(WPARAM)x,(LPARAM)y);
				else
					PostMessage(_lGUI_pWindowsTree,LMSG_NCPENUP,(WPARAM)x,(LPARAM)y);
				return true;
			}
			pCRect=pCRect->pNext;
		}
		//3.within controls of main window
		pWin=_lGUI_pWindowsTree->pControlHead;
		while(pWin){
			pCRect=pWin->pClipRgn->pHead;
			while(pCRect){
				if(PtInRect(&(pCRect->rect),x,y)){
					scrGetClientRect((HWND)pWin,&rcClient);
					if(PtInRect(&rcClient,x,y))
						PostMessage(pWin,LMSG_PENUP,(WPARAM)x,(LPARAM)y);
					else
						PostMessage(pWin,LMSG_NCPENUP,(WPARAM)x,(LPARAM)y);
					return true;
				}
				pCRect=pCRect->pNext;
			}
			pWin=pWin->pNext;
		}
	}
	return true;
}


void 
SendString2Client(
	char* pString
)
{

	if(!_lGUI_pAppStat)
		return;
	SendMsgByServer(_lGUI_pAppStat,LMSG_IPC_CHAR,(void*)pString,strlen(pString));
}

void 
SendChar2Client(
	char ch
)
{
	int tmp=(int)ch;
	if(!_lGUI_pAppStat)
		return;
	SendMsgByServer(_lGUI_pAppStat,LMSG_IPC_CHAR,(void*)tmp,0);
}

void 
SendKeyDown2Client(
	BYTE byteKeyValue
)
{
	int tmp=(int)byteKeyValue;
	if(!_lGUI_pAppStat)
		return;
	SendMsgByServer(_lGUI_pAppStat,LMSG_IPC_KEYDOWN,(void*)tmp,0);
}
