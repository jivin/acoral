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
#include "../include/mouse.h"
#include "../include/keyboard.h"
#include "../include/gal.h"
#include "../include/rect.h"
#include "../include/invalidregion.h"	
#include "../include/clipregion.h" 
#include "../include/hdc.h"
#include "../include/hdcbitblt.h"
#include "../include/lguiapp.h"
#include "../include/gmessage.h"
#include "../include/gdi.h"
#include "../include/regclass.h"
#include "../include/shmem.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/scrollbar.h"
#include "../include/button.h"
#include "../include/system.h"

extern PWindowsTree _lGUI_pImeWindow;
extern PWindowsTree _lGUI_pSkbWindow;

extern PWindowsTree	_lGUI_pActiveWin;
extern PWindowsTree _lGUI_pFocus;
extern PlGUIAppStat  _lGUI_pAppStat;
//extern WndCaptureMouse _lGUI_wndCaptureMouse;

extern int Gal_iBytesDataType;
//semaphore for synchronization between client and server
//mutex_t mutex_cs;



//Re-calculate clip region of a window
void 
ReCalClipRegion(
	const HWND hWnd
)
{
	int iWinType;

	iWinType=GetWinType(hWnd);
	switch(iWinType){
	case WS_CONTROL:
		ReCalClipRegionControl(hWnd);
		break;
	case WS_CHILD:
		ReCalClipRegionChild(hWnd);
		break;
	case WS_MAIN:
		ReCalClipRegionMain(hWnd);
		break;
	case WS_DESKTOP:
		ReCalClipRegionDesktop(hWnd);
		break;
	}

}

void ReCalClipChild(PWindowsTree pWin){
	RECT rect,rc;
	PWindowsTree pControl;
	scrGetClientRect((HWND)pWin,&rect);
	pControl=pWin->pControlHead;
	while(pControl){
		if(!IsHideWindow((HWND)pControl)){
			IntersectRect(&rc,&(pControl->rect),&rect);
			SubtractClipRegion(pWin->pClipRgn,&rc);
		}
		pControl=pControl->pNext;
	}
}
//Re-calculate clip region of desktop
void 
ReCalClipRegionDesktop(
	const HWND hWnd
)
{
	RECT rc;
	PlGUIAppStat pStat;
	PWindowsTree pControl;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	SetRect(&rc,0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);

	EmptyClipRegion(pWin->pClipRgn);

	AddRectClipRegion (pWin->pClipRgn, &rc);
	CopyRect(&(pWin->pClipRgn->rcBound),&rc);
	//ime window
	if(IsVisible((HWND)_lGUI_pImeWindow))
		SubtractClipRegion(pWin->pClipRgn,&_lGUI_pImeWindow->rect);
	//soft key window
	if(IsVisible((HWND)_lGUI_pSkbWindow))
		SubtractClipRegion(pWin->pClipRgn,&_lGUI_pSkbWindow->rect);

	//clipped by all application main window
	pStat=_lGUI_pAppStat;
//	while(pStat&&pStat->bVisible){
	while(pStat){
		SubtractClipRegion(pWin->pClipRgn,&(pStat->rc));
		pStat=pStat->pNext;
	}

	//clipped by all desktop control
	pControl=pWin->pControlHead;
	while(pControl){
		if(!IsHideWindow((HWND)pControl))
			SubtractClipRegion(pWin->pClipRgn,&(pControl->rect));
		pControl=pControl->pNext;
	}
}

//Re-calculate clip region of main window
void 
ReCalClipRegionMain(
	const HWND hWnd
)
{
	RECT rect,rc;

	PWindowsTree pChild,pControl;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	//free clip region 
	EmptyClipRegion(pWin->pClipRgn);

	if(IsEmptyClipRegion(_lGUI_pAppStat->pClipRgn))
		return;
	//copy the origion clip region maintained by server
	CopyClipRegion (pWin->pClipRgn, _lGUI_pAppStat->pClipRgn);

	scrGetClientRect((HWND)pWin,&rect);
	//clipped by its sub window
	pChild=pWin->pChildHead;
	while(pChild){
		if(!IsHideWindow((HWND)pChild)){
			IntersectRect(&rc,&(pChild->rect),&rect);
			SubtractClipRegion(pWin->pClipRgn,&rc);
		}
		pChild=pChild->pNext;
	}
	//clipped by its control
	pControl=pWin->pControlHead;
	while(pControl){
		if(!IsHideWindow((HWND)pControl)){
			IntersectRect(&rc,&(pControl->rect),&rect);
			SubtractClipRegion(pWin->pClipRgn,&rc);
		}
		pControl=pControl->pNext;
	}
}

//Re-calculate clip region of child window
void 
ReCalClipRegionChild(
	const HWND hWnd
)
{

	PWindowsTree pBrother;
	PWindowsTree pWin;
	PlGUIAppStat curStat;
	RECT rect;
	pWin=(PWindowsTree)hWnd;
        curStat=PortGetCurStat(pWin->pParent->threadid);
	//free clip region 
	EmptyClipRegion(pWin->pClipRgn);

	if(IsEmptyClipRegion(curStat->pClipRgn))
		return;

	//copy the origion clip region maintained by server
	CopyClipRegion (pWin->pClipRgn,curStat->pClipRgn);

	//clip noclient area of the mainwindow
	scrGetClientRect((HWND)pWin->pParent,&rect);
	IntersectClipRegion(pWin->pClipRgn,&rect);

	//intersect with the windows boundary rect
	IntersectClipRegion(pWin->pClipRgn,&(pWin->rect));

	//clipped by brother which Z order is bigger than its.
	pBrother=pWin->pParent->pChildHead;
	while(pBrother!=pWin){
		if(!IsHideWindow((HWND)pBrother))
			SubtractClipRegion(pWin->pClipRgn,&(pBrother->rect));
		pBrother=pBrother->pNext;
	}
}

//Re-calculate clip region of control
void 
ReCalClipRegionControl(
	const HWND hWnd
)
{
	PlGUIAppStat pStat;
	PWindowsTree pControl;
	RECT rc;

	PWindowsTree pWin;
	PWindowsTree pChild;
	pWin=(PWindowsTree)hWnd;


	EmptyClipRegion(pWin->pClipRgn);
	if(!IsVisible(pWin))
		return;
	switch(GetWinType((HWND)(pWin->pParent))){
	case WS_DESKTOP:
		AddRectClipRegion (pWin->pClipRgn, &(pWin->rect));
		CopyRect(&(pWin->pClipRgn->rcBound),&(pWin->rect));
		//ime Window
		if(IsVisible((HWND)_lGUI_pImeWindow))
			SubtractClipRegion(pWin->pClipRgn,&_lGUI_pImeWindow->rect);
		//soft key window
		if(IsVisible((HWND)_lGUI_pSkbWindow))
			SubtractClipRegion(pWin->pClipRgn,&_lGUI_pSkbWindow->rect);

		//Intersect with desktop rect
		IntersectClipRegion(pWin->pClipRgn,&(pWin->pParent->rect));
		//clipped by all application main window
		pStat=_lGUI_pAppStat;
		while(pStat){
			SubtractClipRegion(pWin->pClipRgn,&(pStat->rc));
			pStat=pStat->pNext;
		}

		//clipped by brother which Z order is bigger than its
		pControl=pWin->pParent->pControlHead;
		while(pControl!=pWin){
			if(!IsHideWindow((HWND)pControl))
				SubtractClipRegion(pWin->pClipRgn,&(pControl->rect));
			pControl=pControl->pNext;
		}
		break;
	case WS_MAIN:
		if(IsEmptyClipRegion(_lGUI_pAppStat->pClipRgn))
			return;
		CopyClipRegion (pWin->pClipRgn, _lGUI_pAppStat->pClipRgn);

		//clip noclient area of the mainwindow
		scrGetClientRect((HWND)pWin->pParent,&rc);
		IntersectClipRegion(pWin->pClipRgn,&rc);


		IntersectClipRegion(pWin->pClipRgn,&(pWin->rect));
		//clipped by all sub window 
		pChild=pWin->pParent->pChildHead;
		while(pChild){
			if(!IsHideWindow((HWND)pChild))
				SubtractClipRegion(pWin->pClipRgn,&(pChild->rect));
			pChild=pChild->pNext;
		}
		//clipped by brother which Z order is bigger than its
		pControl=pWin->pParent->pControlHead;
		while(pControl!=pWin){
			if(!IsHideWindow((HWND)pControl))
				SubtractClipRegion(pWin->pClipRgn,&(pControl->rect));
			pControl=pControl->pNext;
		}

		break;

	case WS_CHILD:
	/*	if(IsEmptyClipRegion(_lGUI_pAppStat->pClipRgn))
			return;
		CopyClipRegion (pWin->pClipRgn, _lGUI_pAppStat->pClipRgn);

		//clip noclient area of the mainwindow
		scrGetClientRect((HWND)_lGUI_pWindowsTree,&rc);
		IntersectClipRegion(pWin->pClipRgn,&rc);

		//IntersectClipRegion(pWin->pClipRgn,&(pWin->pParent->rect));

		scrGetClientRect((HWND)(pWin->pParent),&rc);
		IntersectClipRegion(pWin->pClipRgn,&rc);


		pChild=_lGUI_pWindowsTree->pChildHead;
		while(pChild!=pWin->pParent){
			if(!IsHideWindow((HWND)pChild))
				SubtractClipRegion(pWin->pClipRgn,&(pChild->rect));
			pChild=pChild->pNext;
		}
		IntersectClipRegion(pWin->pClipRgn,&(pWin->rect));*/
		CopyClipRegion (pWin->pClipRgn, pWin->pParent->pClipRgn);
		IntersectClipRegion(pWin->pClipRgn,&(pWin->rect));
		pControl=pWin->pParent->pControlHead;
		while(pControl!=pWin){
			if(!IsHideWindow((HWND)pControl))
				SubtractClipRegion(pWin->pClipRgn,&(pControl->rect));
			pControl=pControl->pNext;
		}

		break;
	}

}

//Initialize window clip and invalidate region

BOOL 
InitWindowRgn(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	//clip region
	pWin->pClipRgn=(PClipRegion)malloc(sizeof(ClipRegion));
	if(!(pWin->pClipRgn))
		return false;
	InitClipRegion(pWin->pClipRgn);
	//backup clip region
	pWin->pBackClipRgn=(PClipRegion)malloc(sizeof(ClipRegion));
	if(!(pWin->pBackClipRgn))
		return false;
	InitClipRegion(pWin->pBackClipRgn);
	//invalidate regioin 
	pWin->pInvRgn=(PInvalidRegion)malloc(sizeof(InvalidRegion));
	if(!(pWin->pInvRgn))
		return false;
	InitInvalidRegion(pWin->pInvRgn);
	return true;
}

//repaint all other windows which covered by current window
void 
RePaintCoveredWindow(
	const HWND hWnd
)
{
	RECT rc;
	PWindowsTree pCurWin;
	PWindowsTree pWin;
	PWindowsTree pControl;
	BOOL bChild;
	pWin=(PWindowsTree)hWnd;

	bChild = IsChildWin(hWnd);

	CopyRect(&rc,&(pWin->rect));
	//1.parent window
	scrInvalidateRect((HWND)(pWin->pParent),&rc,true);
	//2.it's brother which Z order is small than its
	pCurWin=pWin->pNext;
	while(pCurWin){
		if(IsIntersect(&(pWin->rect),&(pCurWin->rect))){
			scrInvalidateRect((HWND)pCurWin,&rc,true);
			//controls of this window's brother should be repainted
			if(bChild){
				pControl = pCurWin->pControlHead;
				while(pControl){
					if(IsIntersect(&(pWin->rect),&(pCurWin->rect)))
						scrInvalidateRect((HWND)pControl,&rc,true);
					pControl = pControl->pNext;
				}
			}
		}
		pCurWin=pCurWin->pNext;
	}
	//3.if this is a child window, we should repaint the controls of its parent
	pCurWin = pWin->pParent->pControlHead;
	while(pCurWin){
		if(IsIntersect(&(pWin->rect),&(pCurWin->rect))){
			scrInvalidateRect((HWND)pCurWin,&rc,true);
		}
		pCurWin = pCurWin->pNext;
	}
}

//Re-calculate all windows' clip region in an application
void ReCalClipRegionApp()
{
	PWindowsTree pChild,pControl;
	//main window
	ReCalClipRegionMain(_lGUI_pWindowsTree);

	//sub windows
	pChild=_lGUI_pWindowsTree->pChildHead;
	while(pChild){
		ReCalClipRegionChild(pChild);
		pControl=pChild->pControlHead;
		while(pControl){
			ReCalClipRegionControl(pControl);
			pControl=pControl->pNext;
		}
		pChild=pChild->pNext;
	}
	//controls
	pControl=_lGUI_pWindowsTree->pControlHead;
	while(pControl){
		ReCalClipRegionControl(pControl);
		pControl=pControl->pNext;
	}

}

//screen coords
BOOL 
scrInvalidateRect(
	const HWND hWnd,
	const PRECT lpRect, 
	BOOL bErase
)
{
	PWindowsTree pWin;

	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	if(!lpRect){
		AddRectInvalidRegion(pWin->pInvRgn,&(pWin->rect),bErase);
		SendPaintMessage(hWnd);
	}
	else{
		if(!IsIntersect (lpRect, &(pWin->rect)))
			return true;
		AddRectInvalidRegion(pWin->pInvRgn,lpRect,bErase);
		SendPaintMessage(hWnd);
		return true;
	}
	return true;
}

//window coordsys
BOOL 
winInvalidateRect(
	const HWND hWnd, 
	const PRECT lpRect, 
	BOOL bErase
)
{
	PWindowsTree pWin;
	RECT rc;

	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	if(!lpRect){
		GetWindowRect(hWnd,&rc);
		return scrInvalidateRect(hWnd,&rc,bErase);
	}
	else{
		CopyRect(&rc,lpRect);
		WindowToScreenRect(hWnd,&rc);
		return scrInvalidateRect(hWnd,&rc,bErase);
	}
	return true;
}
//declare that one block of a window is invalidate
//to make window redraw itself
BOOL GUIAPI 
InvalidateRect(
	const HWND hWnd, 
	const PRECT lpRect, 
	BOOL bErase
)
{
	RECT rc;
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	if(!lpRect){
  		scrGetClientRect(hWnd,&rc);
		return scrInvalidateRect(hWnd,&rc,bErase);
	}
	else{
		CopyRect(&rc,lpRect);
		ClientToScreenRect(hWnd,&rc);
		return scrInvalidateRect(hWnd,&rc,bErase);
	}
	return true;
}


HDC GUIAPI 
BeginPaint(
	const HWND hWnd,
	LPPAINTSTRUCT lpPaint
)
{
	int iWinWidth, iWinHeight;
	PWindowsTree	pWin;
	RECT			rc;
	PClipRegion		pcrExchange;
	PClipRect		pClipRect;
	HDC hDC,hDCMem;
	HDC hDCClient, hDCClientMem;
	RECT rcTop,rcBottom,rcLeft,rcRight;
	BOOL bFlag=false;

	pWin=(PWindowsTree)hWnd;

	if(!pWin)
		return false;
	//whether the invalid region is empty
	if(IsEmptyInvalidRegion(pWin->pInvRgn))
		return NULL;

	if(!IntersectRect(&rc,&(pWin->pInvRgn->rcBound), &(pWin->pClipRgn->rcBound))){
		return NULL;
	}


	hDC=GetWindowDC(hWnd);
	if(!hDC)
		return NULL;
	if(!(lpPaint->bPaintDirect)){
		hDCMem=CreateCompatibleDC(hDC);
		if(!hDCMem)
			return NULL;
		ReleaseDC(hWnd,hDC);
		hDC=hDCMem;
	}
	lpPaint->hdc=hDC;
	//send erase background message
	SendMessage(hWnd,LMSG_ERASEBKGND,(WPARAM)hDC,(LPARAM)NULL);
	//calculate new clip region according to invalid region
	if(!IsEmptyClipRegion(pWin->pBackClipRgn))
		EmptyClipRegion(pWin->pBackClipRgn);

	ClipRgnIntersectInvRgn(pWin->pBackClipRgn,pWin->pClipRgn,pWin->pInvRgn);
	//exchange clip region pointer
	pcrExchange=pWin->pBackClipRgn;
	pWin->pBackClipRgn=pWin->pClipRgn;
	pWin->pClipRgn=pcrExchange;

	//Get the 4 noclient bound rect
	wndGetBoundRect(hWnd,&rcTop,&rcBottom,&rcLeft,&rcRight);

	if(!IsRectEmpty(&rcTop)){
		pClipRect=pWin->pClipRgn->pHead;
		while(pClipRect){
			if(IsIntersect(&pClipRect->rect,&rcTop)){
				bFlag=true;
				goto okret;
			}
			pClipRect=pClipRect->pNext;
		}
	}
	if(!IsRectEmpty(&rcBottom)){
		pClipRect=pWin->pClipRgn->pHead;
		while(pClipRect){
			if(IsIntersect(&pClipRect->rect,&rcBottom)){
				bFlag=true;
				goto okret;
			}
			pClipRect=pClipRect->pNext;
		}
	}

	if(!IsRectEmpty(&rcLeft)){
		pClipRect=pWin->pClipRgn->pHead;
		while(pClipRect){
			if(IsIntersect(&pClipRect->rect,&rcLeft)){
				bFlag=true;
				goto okret;
			}
			pClipRect=pClipRect->pNext;
		}
	}
	if(!IsRectEmpty(&rcRight)){
		pClipRect=pWin->pClipRgn->pHead;
		while(pClipRect){
			if(IsIntersect(&pClipRect->rect,&rcRight)){
				bFlag=true;
				goto okret;
			}
			pClipRect=pClipRect->pNext;
		}
	}

okret:
	if(bFlag)
		SendMessage(hWnd,LMSG_NCPAINT,(WPARAM)hDC,(LPARAM)NULL);//hDC is window coordsys


	scrGetClientRect(hWnd,&rc);
	//If invalidate region is not intersect with clip area, then we shoudn't redraw client area.
	if(IsIntersect(&(pWin->pInvRgn->rcBound), &rc)){
		//get a client dc, don't forget that we have drawn some thing in the no client area
		hDCClient = GetDC(hWnd);
		if(!(lpPaint->bPaintDirect)){
			hDCClientMem=CreateCompatibleDC(hDCClient);
			if(!hDCClientMem)
				return NULL;
			ReleaseDC(hWnd,hDCClient);
			hDCClient = hDCClientMem;
			iWinWidth = pWin->rect.right - pWin->rect.left + 1;
			iWinHeight = pWin->rect.bottom - pWin->rect.top + 1;
			memcpy(hDCClient->pData, hDC->pData, iWinWidth * iWinHeight *  Gal_iBytesDataType);
			ReleaseDC(hWnd,hDC);

		}
		lpPaint->hdc=hDCClient;
		
		return 	hDCClient;
	}
	else{
		EndPaint(hWnd,lpPaint);	
		return NULL;
	}

}



//called by BeginPaint
void /*inline*/ 
EraseRect2Memory(
	HDC hDCMem,
	PRECT pOrgRect,
	PRECT pOutRect,
	COLORREF crColor
)
{
	int iWidth;
	int x,y;
	PCOLORREF pData;
	iWidth=((PBITMAP)(hDCMem->hBitmap))->bmWidth;
	pData=(PCOLORREF)(hDCMem->pData);
	if(!pData)
		return;
	for(y=pOutRect->top;y<=pOutRect->bottom;y++){
		for(x=pOutRect->left;x<=pOutRect->right;x++){
			*(pData + (y - pOrgRect->top)*iWidth + (x -pOrgRect->left))=crColor;
		}
	}
}

BOOL GUIAPI 
EndPaint(
	const HWND hWnd,
	LPPAINTSTRUCT lpPaint
)
{
	HDC hDC;
	PWindowsTree pWin;
	int iWidth,iHeight;
	int iType;
	pWin=(PWindowsTree)hWnd;
	if(!lpPaint->hdc){
		//printerror("hDC is null in the function EndPaint.");
		return false;
	}
	iType=lpPaint->hdc->iType;
	if((iType!=DC_TYPE_MEM)&&(iType!=DC_TYPE_WIN)){
		//printerror("hdc type error int the function EndPaint.");
		return false;
	}

	//if DC is MemDC then we should output it
	if(lpPaint->hdc->iType==DC_TYPE_MEM){
		iWidth=pWin->rect.right-pWin->rect.left+1;
		iHeight=pWin->rect.bottom-pWin->rect.top+1;
		hDC=GetWindowDC(hWnd);
		winBitBlt(hDC,0,0,iWidth,iHeight,lpPaint->hdc,0,0,SRCCOPY);
		ReleaseDC(hWnd,hDC);
	}
	//Release lpPaint->hDC
	ReleaseDC(hWnd,lpPaint->hdc);
	//restore clip region 
	if(!IsEmptyClipRegion(pWin->pClipRgn))
		EmptyClipRegion(pWin->pClipRgn);
	CopyClipRegion(pWin->pClipRgn,pWin->pBackClipRgn);
	//free clip region
	EmptyClipRegion(pWin->pBackClipRgn);
	//clear invalidate area
	EmptyInvalidRegion(pWin->pInvRgn);
	return true;
}

//coordinate convert
////====================================================================
void GUIAPI 
ScreenToWindow(
	const HWND hWnd,
	int* px,
	int* py
)
{
	PWindowsTree pWin;
	int x=0,y=0;
	pWin=(PWindowsTree)hWnd;

	x=pWin->rect.left;
	y=pWin->rect.top;

	*px -=x;
	*py -=y;
}


void GUIAPI 
ScreenToWindowRect(
	const HWND hWnd,
	LPRECT lprc
)
{
	PWindowsTree pWin;
	int x=0,y=0;
	pWin=(PWindowsTree)hWnd;

	x=pWin->rect.left;
	y=pWin->rect.top;

	OffsetRect(lprc,(-1)*x,(-1)*y);
}


void GUIAPI 
WindowToScreen(
	const HWND hWnd,
	int* px, 
	int* py
)
{
	PWindowsTree pWin;
	int x=0,y=0;
	pWin=(PWindowsTree)hWnd;
	
	x=pWin->rect.left;
	y=pWin->rect.top;

	*px +=x;
	*py +=y;
}


void GUIAPI 
WindowToScreenRect(
	const HWND hWnd,
	LPRECT lprc
)
{
	PWindowsTree pWin;
	int x=0,y=0;
	pWin=(PWindowsTree)hWnd;
	
	x=pWin->rect.left;
	y=pWin->rect.top;
	
	OffsetRect(lprc,x,y);
}


void GUIAPI 
WindowToClient(
	const HWND hWnd,
	int* px,
	int* py
)
{
	int iBorder;
	int iCaption;
	iBorder=wndGetBorder(hWnd);
	if(IsCaption(hWnd))
		iCaption = WIN_CAPTION_HEIGHT;
	else
		iCaption = 0;
	*px-=iBorder;
	*py-=(iBorder+iCaption);
}


void GUIAPI 
WindowToClientRect(
	const HWND hWnd,
	LPRECT lprc
)
{
	int iBorder;
	int iCaption;
	iBorder=wndGetBorder(hWnd);
	if(IsCaption(hWnd))
		iCaption = WIN_CAPTION_HEIGHT;
	else
		iCaption = 0;	
	OffsetRect(lprc,(-1)*iBorder,(-1)*(iBorder+iCaption));
	
}



void GUIAPI 
ClientToWindow(
	const HWND hWnd, 
	int* px, 
	int* py
)
{
	int iBorder;
	int iCaption;
	iBorder=wndGetBorder(hWnd);
	if(IsCaption(hWnd))
		iCaption = WIN_CAPTION_HEIGHT;
	else
		iCaption = 0;		
	*px+=iBorder;
	*py+=(iBorder + iCaption);
}


void GUIAPI 
ClientToWindowRect(
	const HWND hWnd,
	LPRECT lprc
)
{
	int iBorder;
	int iCaption;
	iBorder=wndGetBorder(hWnd);
	if(IsCaption(hWnd))
		iCaption = WIN_CAPTION_HEIGHT;
	else
		iCaption = 0;		
	OffsetRect(lprc,iBorder,iBorder+iCaption);
}



void GUIAPI 
ScreenToClient(
	const HWND hWnd,
	int* px,
	int* py
)
{
	ScreenToWindow(hWnd,px,py);
	WindowToClient(hWnd,px,py);
}


void GUIAPI 
ScreenToClientRect(
	const HWND hWnd,
	LPRECT lprc
)
{
	ScreenToWindowRect(hWnd,lprc);
	WindowToClientRect(hWnd,lprc);
}


void GUIAPI 
ClientToScreen(
	const HWND hWnd,
	int* px,
	int* py
)
{
	ClientToWindow(hWnd,px,py);
	WindowToScreen(hWnd,px,py);

}


void GUIAPI 
ClientToScreenRect(
	const HWND hWnd,
	LPRECT lprc
)
{
	ClientToWindowRect(hWnd,lprc);
	WindowToScreenRect(hWnd,lprc);
}



int 
GetWinType(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	return pWin->dwStyle & WS_TYPEMASK;
}


BOOL 
IsHideWindow(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_VISIBLE)==WS_VISIBLE)
		return false;
	else
		return true;
}



BOOL 
IsDesktop(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_TYPEMASK)==WS_DESKTOP)
		return true;
	else
		return false;
}


BOOL 
IsMainWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_TYPEMASK)==WS_MAIN)
		return true;
	else
		return false;
}


BOOL 
IsChildWin(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_TYPEMASK)==WS_CHILD)
		return true;
	else
		return false;
}


BOOL 
IsControl(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_TYPEMASK)==WS_CONTROL)
		return true;
	else
		return false;
}


BOOL 
IsFocus(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	
	if(!pWin)
		return false;
	if((pWin->dwStyle & WS_FOCUS)==WS_FOCUS)
		return true;
	else
		return false;
}


BOOL 
IsVisible(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;

	if(!pWin)
		return false;
	if((pWin->dwStyle & WS_VISIBLE)==WS_VISIBLE)
		return true;
	else
		return false;
}


BOOL 
IsActive(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(pWin==_lGUI_pActiveWin)
		return true;
	else
		return false;
}


BOOL 
IsEnable(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	if((pWin->dwStyle & WS_DISABLE)==WS_DISABLE)
		return false;
	else
		return true;
}


BOOL 
IsBorder(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_BORDER)==WS_BORDER)
		return true;
	else
		return false;
}


BOOL 
IsTrans(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_TRANS) == WS_TRANS)
		return true;
	else
		return false;
}


BOOL 
IsCaption(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_CAPTION) == WS_CAPTION)
		return true;
	else
		return false;
}


BOOL 
IsOkButton(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_OKBUTTON) == WS_OKBUTTON)
		return true;
	else
		return false;
}


BOOL 
IsCloseBox(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_CLOSEBOX) == WS_CLOSEBOX)
		return true;
	else
		return false;
}


BOOL 
IsOkButtonPressed(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_OKBUTTONPRESSED) == WS_OKBUTTONPRESSED)
		return true;
	else
		return false;	
}


BOOL 
IsCloseBoxPressed(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if((pWin->dwStyle & WS_CLOSEBOXPRESSED) == WS_CLOSEBOXPRESSED)
		return true;
	else
		return false;	
}


void 
MoveWindowTopMost(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	PWindowsTree pCurWin;
	PWindowsTree pParent;
	int iZOrder=0;
	int iWinType;
	//if window type is desktop or main window then exit
	iWinType=GetWinType(hWnd);
	if((iWinType==WS_DESKTOP)||(iWinType==WS_MAIN))
		return;
	pWin=(PWindowsTree)hWnd;
	pParent=pWin->pParent;
	if(!pParent)
		return;
	if(iWinType==WS_CHILD){
		if(pParent->pChildHead==pWin)
			return;
		pWin->pPrev->pNext=pWin->pNext;
		pWin->pNext->pPrev=pWin->pPrev;
		pParent->pChildHead->pPrev=pWin;
		pWin->pNext=pParent->pChildHead;
		pParent->pChildHead=pWin;
	}
	else{//Is Control
		if(pParent->pControlHead==pWin)
			return;
		pWin->pPrev->pNext=pWin->pNext;
		pWin->pNext->pPrev=pWin->pPrev;
		pParent->pControlHead->pPrev=pWin;
		pWin->pNext=pParent->pControlHead;
		pParent->pControlHead=pWin;
	}
	
	pCurWin=pWin;
	while(pCurWin){
		pCurWin->iZOrder=iZOrder;
		iZOrder++;
		pCurWin=pCurWin->pNext;
	}
}


void 
SetFocusNoPaint(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return;
	if(!IsControl(hWnd))
		return;
	if(!IsVisible(hWnd))
		return;
	if(!IsActive((HWND)(pWin->pParent)))
		return;
	if(IsFocus(hWnd))
		return;

	if(_lGUI_pFocus)
		UnSetFocus((HWND)_lGUI_pFocus);
	_lGUI_pFocus=pWin;
	pWin->pParent->pFocus=pWin;
	pWin->dwStyle = pWin->dwStyle | WS_FOCUS;
}


void GUIAPI 
SetFocus(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return;
	if(!IsControl(hWnd))
		return;
	if(!IsVisible(hWnd))
		return;
	if(!IsActive((HWND)(pWin->pParent)))
		return;
	if(IsFocus(hWnd))
		return;

	if(_lGUI_pFocus)
		UnSetFocus((HWND)_lGUI_pFocus);
	_lGUI_pFocus=pWin;
	pWin->pParent->pFocus=pWin;
	pWin->dwStyle = pWin->dwStyle | WS_FOCUS;

	winInvalidateRect(hWnd,NULL,true);

	SendNotifyMessage(hWnd,LMSG_SETFOCUS,(WPARAM)NULL,(LPARAM)NULL);
}


void 
UnSetFocus(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return;
	if(!IsControl(hWnd))
		return;
	if(!IsFocus(hWnd))
		return;
	pWin->dwStyle = pWin->dwStyle & ~WS_FOCUS;
	//if((pWin->dwStyle & WS_VISIBLE)==WS_VISIBLE)
	winInvalidateRect(hWnd,NULL,true);
	SendNotifyMessage(hWnd,LMSG_KILLFOCUS,(WPARAM)NULL,(LPARAM)NULL);
}


void 
UnSetFocusNoPaint(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return;
	if(!IsControl(hWnd))
		return;
	if(!IsFocus(hWnd))
		return;
	pWin->dwStyle = pWin->dwStyle & ~WS_FOCUS;
}



void 
ActiveApplication()
{
	PWindowsTree pWin;
	pWin=_lGUI_pWindowsTree->pChildHead;

	_lGUI_pActiveWin = NULL;
	if(pWin){
		ActiveWindow(pWin);
	}
	else{
		ActiveWindow(_lGUI_pWindowsTree);
	}
}


void 
DisactiveApplication()
{
	if(!_lGUI_pActiveWin)
		return;
	DisactiveWindow((HWND)_lGUI_pActiveWin);

	_lGUI_wndCaptureMouse->pWin=NULL;

}

void 
DisactiveWindow(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return;
	if(!IsActive(hWnd))
		return;
	if((pWin->dwStyle & WS_ACTIVE)==WS_ACTIVE){
		pWin->dwStyle = pWin->dwStyle & ~WS_ACTIVE;
		_lGUI_pActiveWin=NULL;		
		SendNotifyMessage(hWnd,LMSG_DISACTIVEWINDOW,(WPARAM)NULL,(LPARAM)NULL);

	}
}


void 
ActiveWindow(
	const HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(IsActive(hWnd))
		return;
	if(_lGUI_pActiveWin)
		DisactiveWindow((HWND)_lGUI_pActiveWin);

	if(_lGUI_pActiveWin!=pWin){
		_lGUI_pActiveWin=pWin;
		pWin->dwStyle = pWin->dwStyle | WS_ACTIVE;

		SendNotifyMessage(hWnd,LMSG_ACTIVEWINDOW,(WPARAM)NULL,(LPARAM)NULL);
	}

}


/*inline*/ int 
wndGetBorder(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return 0;
	if(pWin->dwStyle & WS_BORDER)
		return WIN_BORDER_WIDTH;
	else if(pWin->dwStyle & WS_THINBORDER)
		return WIN_THINBORDER_WIDTH;
	else if(pWin->dwStyle & WS_THICKBORDER)
		return WIN_THICKBORDER_WIDTH;
	else
		return 0;
	return 0;
}


BOOL GUIAPI 
GetWindowRect(
	HWND hWnd,
	LPRECT lpRect
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	CopyRect(lpRect,&(pWin->rect));
	return true;
}


BOOL 
scrGetClientRect(
	const HWND hWnd, 
	LPRECT lpRect
)
{
	PWindowsTree pWin;
	BOOL bVScroll;
	BOOL bHScroll;
	int iBorder;
	int iWidth,iHeight;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	if(pWin->dwStyle & WS_VSCROLL)
		bVScroll = true;
	else
		bVScroll = false;
	if(pWin->dwStyle & WS_HSCROLL)
		bHScroll = true;
	else
		bHScroll = false;
	iBorder = wndGetBorder(hWnd);
	memcpy(lpRect,&(pWin->rect),sizeof(RECT));
	//border & scrollbar
	iWidth=GetSystemMetrics(SM_CXVSCROLL);
	iHeight=GetSystemMetrics(SM_CYHSCROLL);
	if(iBorder)
		InflateRect(lpRect,(-1) * iBorder,(-1) * iBorder);
	if(bVScroll)
		SetRect(lpRect,lpRect->left,lpRect->top,lpRect->right-iWidth,lpRect->bottom);
	if(bHScroll)
		SetRect(lpRect,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom-iHeight);
	//caption?
	if(IsCaption(hWnd))
		SetRect(lpRect,lpRect->left,lpRect->top + WIN_CAPTION_HEIGHT,
			lpRect->right,lpRect->bottom);

	return true;
}


BOOL GUIAPI 
GetClientRect(
	const HWND hWnd, 
	LPRECT lpRect
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;
	scrGetClientRect(hWnd,lpRect);
	//convert to client coord
	if(pWin->pParent)
		ScreenToClientRect((HWND)(pWin->pParent),lpRect);
	return true;
}

DWORD 
GetWindowStyle (
	HWND hWnd
)
{
	PWindowsTree pWin = (PWindowsTree)hWnd;
    return pWin->dwStyle;
}



LRESULT 
DefWindowProc(
	HWND hWnd, 
	UINT iMsg, 
	WPARAM wParam,
	LPARAM lParam
)
{
	PWindowsTree pWin;
	PWindowsTree pControl;
	PInvalidRect pInvRect;
	PClipRect	 pClipRect;
	HDC hDC;
	RECT rc,rcHScrollBar,rcVScrollBar;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	pWin=(PWindowsTree)hWnd;
	hDC=(HDC)wParam;
	if(iMsg&MTYPE_IPC){
		ProcessIpcMessageClient(iMsg,(void *)wParam,(int) lParam);
		return true;
	}
	switch(iMsg){
	case LMSG_ACTIVEWINDOW:
		if(pWin->pFocus){
			SetFocus(pWin->pFocus);
		}
		else{
			pControl=pWin->pControlTail;
			if(pControl)
    			SetFocus(pControl);
		}
		//set the caption rect invalidate
		wndGetCaptionRect(hWnd,&rc);
		//printf("active:%d,%d,%d,%d\n",
		//	rc.left,rc.top,rc.right,rc.bottom);
		scrInvalidateRect(hWnd,&rc,true);
		break;
	case LMSG_DISACTIVEWINDOW:
		UnSetFocus((HWND)(pWin->pFocus));
		//set the caption rect invalidate
		wndGetCaptionRect(hWnd,&rc);
		//printf("disactive:%d,%d,%d,%d\n",
		//	rc.left,rc.top,rc.right,rc.bottom);		
		scrInvalidateRect(hWnd,&rc,true);
		break;
	case LMSG_ERASEBKGND:
		pWndClass=GetRegClass(pWin->lpszClassName);
		if(!pWndClass)
			return (LRESULT)NULL;
		hBrush=pWndClass->hbrBackground;
		crColor=((BRUSH*)hBrush)->crBrushColor;
		//erase invalid region background according to invalidatrect
		pInvRect=pWin->pInvRgn->pHead;
		if(hDC->iType==DC_TYPE_MEM){//Is Memory DC
			while(pInvRect){
				pClipRect=pWin->pClipRgn->pHead;
				if(pInvRect->bErase){
					while(pClipRect){
						if(IntersectRect(&rc,&(pInvRect->rect),&(pClipRect->rect)))
							EraseRect2Memory(hDC,&(pWin->rect),&rc,crColor);
						pClipRect=pClipRect->pNext;
					}
				}
				pInvRect=pInvRect->pNext;
			}
		}
		else{
			//fill the intersect of invalidate region and clip region
			while(pInvRect){
				pClipRect=pWin->pClipRgn->pHead;
				if(pInvRect->bErase){
					while(pClipRect){
						if(IntersectRect(&rc,&(pInvRect->rect),&(pClipRect->rect)))
							Gal_EraseRect(&rc,crColor);
						pClipRect=pClipRect->pNext;
					}
				}
				pInvRect=pInvRect->pNext;
			}
		}
		break;

	case LMSG_SETFOCUS:

		break;
	case LMSG_KILLFOCUS:
		//pWin->dwStyle=pWin->dwStyle & ~ WS_FOCUS;

		break;
	case LMSG_NCPAINT:
		wndDrawNCFrame (hWnd,(HDC)wParam);
		
		break;
	//=========================================================================
	case LMSG_NCPENDOWN:
		wndGetOkButtonRect(hWnd,&rc);
		if(PtInRect(&rc,(int)wParam,(int)lParam)){
			CaptureMouse(hWnd,BYOKBUTTON);
			OkButtonPenDownProc(hWnd);
			break;
		}
		wndGetCloseBoxRect(hWnd,&rc);
		if(PtInRect(&rc,(int)wParam,(int)lParam)){
			CaptureMouse(hWnd,BYCLOSEBOX);
			CloseBoxPenDownProc(hWnd);
			break;
		}
			
		wndGetVScrollBarRect(hWnd,&rcVScrollBar);
		wndGetHScrollBarRect(hWnd,&rcHScrollBar);
		if(PtInRect(&rcVScrollBar,(int)wParam,(int)lParam)){
			CaptureMouse(hWnd,BYVSCROLL);
			VScrollBarPenDownProc(hWnd,(int)wParam,(int)lParam);
		}
		else if(PtInRect(&rcHScrollBar,(int)wParam,(int)lParam)){
			CaptureMouse(hWnd,BYHSCROLL);
			HScrollBarPenDownProc(hWnd,(int)wParam,(int)lParam);
		}
		break;
	case LMSG_NCPENMOVE:
		if(_lGUI_wndCaptureMouse->iByWho == BYVSCROLL){
			VScrollBarPenMoveProc(hWnd,(int)wParam,(int)lParam);
		}
		else if(_lGUI_wndCaptureMouse->iByWho == BYHSCROLL){
			HScrollBarPenMoveProc(hWnd,(int)wParam,(int)lParam);
		}
		break;
	case LMSG_NCPENUP:
		if(_lGUI_wndCaptureMouse->pWin){
			if(_lGUI_wndCaptureMouse->iByWho == BYOKBUTTON){
				OkButtonPenUpProc(hWnd);
				break;
			}
			else if(_lGUI_wndCaptureMouse->iByWho == BYCLOSEBOX){
				CloseBoxPenUpProc(hWnd);
				break;
			}
		}
			
		wndGetVScrollBarRect(hWnd,&rcVScrollBar);
		wndGetHScrollBarRect(hWnd,&rcHScrollBar);
		if(_lGUI_wndCaptureMouse->pWin){
			if(_lGUI_wndCaptureMouse->iByWho == BYVSCROLL)
				VScrollBarPenUpProc(hWnd);
			else if(_lGUI_wndCaptureMouse->iByWho == BYHSCROLL)
				HScrollBarPenUpProc(hWnd);
		}
		else{
			if((pWin->dwStyle & WS_VSCROLL) && PtInRect(&rcVScrollBar,(int)wParam,(int)lParam))
				VScrollBarPenUpProc(hWnd);
			else if((pWin->dwStyle & WS_HSCROLL) && PtInRect(&rcHScrollBar,(int)wParam,(int)lParam))
				HScrollBarPenUpProc(hWnd);
		}
		DisCaptureMouse();
		break;
	}
	return 0;
}




//===========================================================================


void 
CaptureMouse(
	HWND hWnd,
	int iByWho
)
{

	_lGUI_wndCaptureMouse->pWin = (PWindowsTree)hWnd;
	_lGUI_wndCaptureMouse->iByWho = iByWho;
}

void 
DisCaptureMouse()
{
	_lGUI_wndCaptureMouse->pWin = NULL;
}

HWND 
GetCaptureWindow()
{
	return (HWND)_lGUI_wndCaptureMouse->pWin;
}

//=================================

BOOL /*inline*/ 
IsServerProcess()
{
	char lpszClassName[256];
	int i=0;
	strcpy(lpszClassName,_lGUI_pWindowsTree->lpszClassName);
	while(lpszClassName[i]){
		lpszClassName[i]=toupper(lpszClassName[i]);
		i++;
	}

	if(strcmp(lpszClassName,"DESKTOP")==0)
		return true;
	else
		return false;
}


/*
//client/server mutex  
void 
LockMutexForSynchro()
{
	pthread_mutex_lock(&_lGUI_CurStat->mutex_cs);
}

void 
UnLockMutexForSynchro()
{
	pthread_mutex_unlock(&_lGUI_CurStat->mutex_cs);
}
*/





