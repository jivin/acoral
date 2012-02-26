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
#include "../include/text.h"





static void 
CalVScrollCurState(
	LPRECT lpRect,
	LPSCROLLINFO 
	pScrollInfo,
	LPSCROLLCURSTATE pCurState
);

static void 
CalHScrollCurState(
	LPRECT lpRect,
	LPSCROLLINFO pScrollInfo,
	LPSCROLLCURSTATE pCurState
);



static LPSCROLLINFO 
wndGetScrollBar (
	HWND hWnd, 
	int iSBar
);

static BOOL 
ScrollBarIsEnable(
	LPSCROLLINFO lpsi
);


void 
wndGetBoundRect(
	HWND hWnd,
	LPRECT lpRectLeft, 
	LPRECT lpRectTop,
	LPRECT lpRectRight,
	LPRECT lpRectBottom
)
{
	RECT rcWin;
	BOOL bVScroll, bHScroll;
	int iBorder,iCaption;
	int iVScrollBarWidth,iHScrollBarHeight;
	if(IsVScrollBar(hWnd)){
		iVScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
		bVScroll = true;
	}
	if(IsHScrollBar(hWnd)){
		iHScrollBarHeight = GetSystemMetrics(SM_CYHSCROLL);
		bHScroll = true;
	}

	iBorder=wndGetBorder(hWnd);

	if(IsCaption(hWnd))
		iCaption = WIN_CAPTION_HEIGHT;
	else
		iCaption = 0;

	GetWindowRect(hWnd,&rcWin);


	if(!iBorder){
		SetRect(lpRectLeft,0,0,0,0);
	}
	else{
		SetRect(lpRectLeft,rcWin.left,rcWin.top+iBorder,
			rcWin.left+iBorder,rcWin.bottom - iHScrollBarHeight);
	}

	if(!iBorder && !iCaption){
		SetRect(lpRectTop,0,0,0,0);
	}
	else{
		SetRect(lpRectTop,rcWin.left,rcWin.top,
			rcWin.right,rcWin.top + iBorder + iCaption);
	}


	if(!iBorder && !bVScroll)
		SetRect(lpRectRight,0,0,0,0);
	else
		SetRect(lpRectRight,rcWin.right-iBorder-iVScrollBarWidth+1,rcWin.top+iBorder+iCaption+1,
			rcWin.right,rcWin.bottom-iBorder-iHScrollBarHeight-1);

	if(!iBorder && !bHScroll)
		SetRect(lpRectBottom,0,0,0,0);
	else
		SetRect(lpRectBottom,rcWin.left,
			rcWin.bottom-iHScrollBarHeight-iBorder,rcWin.right,rcWin.bottom);
}

//Get Caption rect screen coordsys
void 
wndGetCaptionRect(
	HWND hWnd,
	LPRECT lpRect
)
{
	PWindowsTree pWin;
	RECT rcWin;
	int iBorder;
	pWin = (PWindowsTree)hWnd;
	GetWindowRect(hWnd,&rcWin);
	if(!IsCaption(hWnd)){
		SetRect(lpRect,0,0,0,0);
		return;
	}
	else{
		iBorder = wndGetBorder(hWnd);
		SetRect(lpRect,rcWin.left+iBorder,rcWin.top+iBorder,
			rcWin.right-iBorder,rcWin.top+iBorder+WIN_CAPTION_HEIGHT-1);
		
	}
}

////===================================Pen Down=======================================
//vscroll pendown procedure
void 
VScrollBarPenDownProc(
	HWND hWnd,
	int x, 
	int y
)
{
	int iArrowHeight,iBarWidth,iBarHeight,iSliderBarHeight,iPos,iTotalBarHeight;
	RECT rc,rcDownButton,rcUpButton,rcTotalBar,rcSliderBar;
	LPSCROLLCURSTATE pCurState;
	LPSCROLLINFO pScrollInfo;
	unsigned long iRetMsg;
	PWindowsTree pWin;

	pWin=(PWindowsTree)hWnd;

	if(!ScrollBarIsEnable(pWin->pVScroll))
		return;

	iArrowHeight=GetSystemMetrics(SM_CYVSCROLL);

	wndGetVScrollBarRect(hWnd,&rc);
	pScrollInfo=pWin->pVScroll;
	pCurState = pWin->pVCurState;


	iBarWidth	=rc.right - rc.left + 1;
	iBarHeight	=rc.bottom - rc.top + 1;
	//cal sliderbar value
	iSliderBarHeight =	pCurState->iSliderBarLen;
	iPos = pCurState->iSliderBarVal;

	iTotalBarHeight = iBarHeight- 2 * iArrowHeight;


	//DownButton
	SetRect(&rcDownButton,rc.left,rc.bottom-iArrowHeight,rc.right,rc.bottom);
	//UpButton
	SetRect(&rcUpButton,rc.left,rc.bottom-2*iArrowHeight,rc.right,rc.bottom-iArrowHeight);
	//TotalBar
	SetRect(&rcTotalBar,rc.left,rc.top,rc.right,rc.bottom-2*iArrowHeight);
	//SliderBar
	SetRect(&rcSliderBar,rc.left,rc.top+iPos,rc.right,rc.top+iPos+iSliderBarHeight);


	if(PtInRect(&rcDownButton,x,y)){
		pWin->pVScroll->dwStatus |=SS_DOWNRIGHT_PRESSDOWN;
		//pWin->dwStyle=pWin->dwStyle | SS_DOWNRIGHT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcDownButton,true);

		if((pScrollInfo->nPos + pScrollInfo->nPage -1) < pScrollInfo->nMax){
			pScrollInfo->nPos+=1;

			iPos = (pScrollInfo->nPos - pScrollInfo->nMin) * iTotalBarHeight/(pScrollInfo->nMax - pScrollInfo->nMin + 1);
			if((iTotalBarHeight- iPos)< iSliderBarHeight)
				iPos=iTotalBarHeight - iSliderBarHeight;

			pCurState->iSliderBarVal = iPos;
			scrInvalidateRect(hWnd,&rcTotalBar,true);
			iRetMsg=SB_LINEDOWN;
			if(IsControl(hWnd)){
				SendMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
			else{
				SendNotifyMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
		}
		else if(pCurState->iSliderBarVal + pCurState->iSliderBarLen < iTotalBarHeight -1){
			pCurState->iSliderBarVal = iTotalBarHeight - pCurState->iSliderBarLen;
			scrInvalidateRect(hWnd,&rcTotalBar,true);
		}
	}
	else if(PtInRect(&rcUpButton,x,y)){
		pWin->pVScroll->dwStatus |=SS_UPLEFT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcUpButton,true);
		if(pScrollInfo->nPos > pScrollInfo->nMin){
			pScrollInfo->nPos-=1;
			iPos = (pScrollInfo->nPos - pScrollInfo->nMin) * iTotalBarHeight/(pScrollInfo->nMax - pScrollInfo->nMin + 1);

			pCurState->iSliderBarVal = iPos;
			scrInvalidateRect(hWnd,&rcTotalBar,true);

			iRetMsg=SB_LINEUP;
			if(IsControl(hWnd)){
				SendMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
			else{
				SendNotifyMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
		}
	}
	else if(PtInRect(&rcSliderBar,x,y)){
		pWin->dwStyle=pWin->dwStyle | SS_BAR_PRESSDOWN;
		pCurState->iClickPos = y - rcSliderBar.top;
	}
	else{
		pCurState->iClickPos = 0;
	}
}


//hscroll pendown procedure
void 
HScrollBarPenDownProc(
	HWND hWnd,
	int x, 
	int y
)
{
	int iArrowWidth,iBarWidth,iBarHeight,iSliderBarWidth,iPos,iTotalBarWidth;
	RECT rc,rcRightButton,rcLeftButton,rcTotalBar,rcSliderBar;
	LPSCROLLCURSTATE pCurState;
	LPSCROLLINFO pScrollInfo;
	unsigned long iRetMsg;
	PWindowsTree pWin;


	pWin=(PWindowsTree)hWnd;

	if(!ScrollBarIsEnable(pWin->pHScroll))
		return;

	iArrowWidth=GetSystemMetrics(SM_CXHSCROLL);

	wndGetHScrollBarRect(hWnd,&rc);

	pScrollInfo=pWin->pHScroll;
	pCurState = pWin->pHCurState;


	iBarWidth	=rc.right - rc.left + 1;
	iBarHeight	=rc.bottom - rc.top + 1;
	iSliderBarWidth =	pCurState->iSliderBarLen;
	iPos = pCurState->iSliderBarVal;

	iTotalBarWidth = iBarWidth- 2 * iArrowWidth;

	//RightButton
	SetRect(&rcRightButton,rc.right-iArrowWidth,rc.top,rc.right,rc.bottom);
	//LeftButton
	SetRect(&rcLeftButton,rc.right-2*iArrowWidth,rc.top,rc.right-iArrowWidth,rc.bottom);
	//TotalBar
	SetRect(&rcTotalBar,rc.left,rc.top,rc.right-2*iArrowWidth,rc.bottom);
	//SliderBar
	SetRect(&rcSliderBar,rc.left+iPos,rc.top,rc.left+iPos+iSliderBarWidth,rc.bottom);

	//
	if(PtInRect(&rcRightButton,x,y)){
		pWin->pHScroll->dwStatus |=SS_DOWNRIGHT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcRightButton,true);
		if((pScrollInfo->nPos + pScrollInfo->nPage -1) < pScrollInfo->nMax){
			pScrollInfo->nPos+=1;

			iPos = (pScrollInfo->nPos - pScrollInfo->nMin) * iTotalBarWidth/(pScrollInfo->nMax - pScrollInfo->nMin + 1);
			if((rcTotalBar.right - iPos)< iSliderBarWidth)
				iPos=rcTotalBar.right - iSliderBarWidth;

			pCurState->iSliderBarVal = iPos;
			scrInvalidateRect(hWnd,&rcTotalBar,true);

			iRetMsg=SB_LINERIGHT;
			if(IsControl(hWnd)){
				SendMessage(hWnd,LMSG_HSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
			else{
				SendNotifyMessage(hWnd,LMSG_HSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
		}
		else if(pCurState->iSliderBarVal + pCurState->iSliderBarLen < iTotalBarWidth - 1){
			pCurState->iSliderBarVal = iTotalBarWidth - pCurState->iSliderBarLen;
			scrInvalidateRect(hWnd,&rcTotalBar,true);
		}
	}
	else if(PtInRect(&rcLeftButton,x,y)){
		pWin->pHScroll->dwStatus |=SS_UPLEFT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcLeftButton,true);
		if(pScrollInfo->nPos > pScrollInfo->nMin){
			pScrollInfo->nPos-=1;
			iPos = (pScrollInfo->nPos - pScrollInfo->nMin) * iTotalBarWidth/(pScrollInfo->nMax - pScrollInfo->nMin + 1);

			pCurState->iSliderBarVal = iPos;
			scrInvalidateRect(hWnd,&rcTotalBar,true);
			iRetMsg=SB_LINELEFT;
			if(IsControl(hWnd)){
				SendMessage(hWnd,LMSG_HSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
			else{
				SendNotifyMessage(hWnd,LMSG_HSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
		}
	}
	else if(PtInRect(&rcSliderBar,x,y)){
		pWin->dwStyle=pWin->dwStyle | SS_BAR_PRESSDOWN;
		pCurState->iClickPos = x - rcSliderBar.left;
	}
	else{
		pCurState->iClickPos = 0;
	}

}


////===================================Pen Up=========================================
//vscroll penup procedure
void 
VScrollBarPenUpProc(
	HWND hWnd
)
{
	PWindowsTree pWin;
	int iArrowHeight,iBarHeight,iBarWidth;
	RECT rc,rcDownButton,rcUpButton;

	pWin=(PWindowsTree)hWnd;

	if(!ScrollBarIsEnable(pWin->pVScroll))
		return;

	iArrowHeight=GetSystemMetrics(SM_CYVSCROLL);
	//GetClientRect(hWnd,&rc);
	//GetWindowRect(hWnd,&rc);
	wndGetVScrollBarRect(hWnd,&rc);
	iBarWidth	=rc.right - rc.left + 1;
	iBarHeight	=rc.bottom - rc.top + 1;
	//DownButton
	SetRect(&rcDownButton,rc.left,rc.bottom-iArrowHeight,rc.right,rc.bottom);
	//UpButton
	SetRect(&rcUpButton,rc.left,rc.bottom-2*iArrowHeight,rc.right,rc.bottom-iArrowHeight);

	//restore  the clicked button
	if((pWin->pVScroll->dwStatus & SS_DOWNRIGHT_PRESSDOWN) !=0){
		pWin->pVScroll->dwStatus = pWin->pVScroll->dwStatus & ~SS_DOWNRIGHT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcDownButton,true);
	}
	else if((pWin->pVScroll->dwStatus & SS_UPLEFT_PRESSDOWN) !=0){
		pWin->pVScroll->dwStatus = pWin->pVScroll->dwStatus & ~SS_UPLEFT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcUpButton,true);
	}
	else if((pWin->pVScroll->dwStatus & SS_BAR_PRESSDOWN) !=0){
		pWin->pVScroll->dwStatus =pWin->pVScroll->dwStatus & ~SS_BAR_PRESSDOWN;
	}
}


//hscroll penup procedure
void 
HScrollBarPenUpProc(
	HWND hWnd
)
{
	PWindowsTree pWin;
	int iArrowWidth,iBarHeight,iBarWidth;
	RECT rc,rcRightButton,rcLeftButton;

	pWin=(PWindowsTree)hWnd;

	if(!ScrollBarIsEnable(pWin->pHScroll))
		return;

	iArrowWidth=GetSystemMetrics(SM_CXHSCROLL);

	wndGetHScrollBarRect(hWnd,&rc);

	iBarWidth	=rc.right - rc.left + 1;
	iBarHeight	=rc.bottom - rc.top + 1;

	//RightButton
	SetRect(&rcRightButton,rc.right-iArrowWidth,rc.top,rc.right,rc.bottom);
	//LeftButton
	SetRect(&rcLeftButton,rc.right-2*iArrowWidth,rc.top,rc.right-iArrowWidth,rc.bottom);
	//restore  the clicked button
	if((pWin->pHScroll->dwStatus & SS_DOWNRIGHT_PRESSDOWN) !=0){
		pWin->pHScroll->dwStatus = pWin->pHScroll->dwStatus & ~SS_DOWNRIGHT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcRightButton,true);
	}
	else if((pWin->pHScroll->dwStatus & SS_UPLEFT_PRESSDOWN) !=0){
		pWin->pHScroll->dwStatus = pWin->pHScroll->dwStatus & ~SS_UPLEFT_PRESSDOWN;
		scrInvalidateRect(hWnd,&rcLeftButton,true);
	}
	else if((pWin->pHScroll->dwStatus & SS_BAR_PRESSDOWN) !=0){
		pWin->pHScroll->dwStatus = pWin->pHScroll->dwStatus & ~SS_BAR_PRESSDOWN;
	}
}
////===================================Pen Move=======================================
//vscroll penmove procedure
void 
VScrollBarPenMoveProc(
	HWND hWnd,
	int x, 
	int y
)
{
	int iArrowHeight,iBarWidth,iBarHeight,iSliderBarHeight,iPos,iTotalBarHeight;
	RECT rc,rcDownButton,rcUpButton,rcTotalBar,rcSliderBar;
	LPSCROLLCURSTATE pCurState;
	LPSCROLLINFO pScrollInfo;
	unsigned long iRetMsg;
	PWindowsTree pWin;

	pWin=(PWindowsTree)hWnd;

	if(!ScrollBarIsEnable(pWin->pVScroll))
		return;

	if(!(pWin->pVCurState->iClickPos))
		return;

	wndGetVScrollBarRect(hWnd,&rc);

	iBarWidth	=rc.right - rc.left + 1;
	iBarHeight	=rc.bottom - rc.top + 1;
	iArrowHeight=GetSystemMetrics(SM_CYVSCROLL);

	pScrollInfo=pWin->pVScroll;
	pCurState = pWin->pVCurState;

	iTotalBarHeight = iBarHeight- 2 * iArrowHeight;
	iSliderBarHeight = pCurState->iSliderBarLen;
	iPos = pCurState->iSliderBarVal;

	//DownButton
	SetRect(&rcDownButton,rc.left,rc.bottom-iArrowHeight,rc.right,rc.bottom);
	//UpButton
	SetRect(&rcUpButton,rc.left,rc.bottom-2*iArrowHeight,rc.right,rc.bottom-iArrowHeight);
	//TotalBar
	SetRect(&rcTotalBar,rc.left,rc.top,rc.right,rc.bottom-2*iArrowHeight);
	//SliderBar
	SetRect(&rcSliderBar,rc.left,rc.top+iPos,rc.right,rc.top+iPos+iSliderBarHeight);

	if((y - rcSliderBar.top) > pCurState->iClickPos){//move down
		if(rcSliderBar.bottom == rcTotalBar.bottom - 1){//already reach bottom
			pScrollInfo->nPos = pScrollInfo->nMax - pScrollInfo->nPage + 1;
			pCurState->iSliderBarVal = iTotalBarHeight - iSliderBarHeight - 1;
			scrInvalidateRect(hWnd,&rcTotalBar,true);
		}
		else{
			if((y + iSliderBarHeight - pCurState->iClickPos)>=rcTotalBar.bottom){
				pScrollInfo->nPos=pScrollInfo->nMax - pScrollInfo->nPage + 1 ;
				pCurState->iSliderBarVal = iTotalBarHeight - iSliderBarHeight - 1;
				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
			else{
				iPos = y - rcTotalBar.top - pCurState->iClickPos;
				pCurState->iSliderBarVal = iPos;
				pScrollInfo->nPos = iPos * (pScrollInfo->nMax - pScrollInfo->nMin + 1)/iTotalBarHeight;
				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
		}
		iRetMsg=SB_THUMBTRACK;
		if(IsControl(hWnd)){
			SendMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
		}
		else{
			SendNotifyMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
		}
	}
	else{//move up
		if(rcSliderBar.top == rcTotalBar.top)

			return;
		else{
			if(y - rcTotalBar.top<= pCurState->iClickPos){
				pScrollInfo->nPos=pScrollInfo->nMin;
				pCurState->iSliderBarVal = 0;
				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
			else{
				iPos = y - rcTotalBar.top - pCurState->iClickPos;
				pCurState->iSliderBarVal = iPos;
				pScrollInfo->nPos = iPos * (pScrollInfo->nMax - pScrollInfo->nMin + 1)/iTotalBarHeight + pScrollInfo->nMin;
				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
			iRetMsg=SB_THUMBTRACK;
			if(IsControl(hWnd)){
				SendMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
			else{
				SendNotifyMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
		}
	}
}



//hscroll penmove procedure
void 
HScrollBarPenMoveProc(
	HWND hWnd,
	int x, 
	int y
)
{
	int iArrowWidth,iBarWidth,iBarHeight,iSliderBarWidth,iPos,iTotalBarWidth;
	RECT rc,rcRightButton,rcLeftButton,rcTotalBar,rcSliderBar;
	LPSCROLLCURSTATE pCurState;
	LPSCROLLINFO pScrollInfo;
	unsigned long iRetMsg;
	PWindowsTree pWin;

	pWin=(PWindowsTree)hWnd;

	if(!ScrollBarIsEnable(pWin->pHScroll))
		return;

	if(!(pWin->pHCurState->iClickPos))
		return;
	wndGetHScrollBarRect(hWnd,&rc);

	iBarWidth	=rc.right - rc.left + 1;
	iBarHeight	=rc.bottom - rc.top + 1;
	iArrowWidth=GetSystemMetrics(SM_CXHSCROLL);

	pScrollInfo=pWin->pHScroll;
	pCurState = pWin->pHCurState;

	iTotalBarWidth = iBarWidth- 2 * iArrowWidth;
	iSliderBarWidth = pCurState->iSliderBarLen;
	iPos = pCurState->iSliderBarVal;

	//RightButton
	SetRect(&rcRightButton,rc.right-iArrowWidth,rc.top,rc.right,rc.bottom);
	//LeftButton
	SetRect(&rcLeftButton,rc.right-2*iArrowWidth,rc.top,rc.right-iArrowWidth,rc.bottom);
	//TotalBar
	SetRect(&rcTotalBar,rc.left,rc.top,rc.right-2*iArrowWidth,rc.bottom);
	//SliderBar
	SetRect(&rcSliderBar,rc.left+iPos,rc.top,rc.left+iPos+iSliderBarWidth,rc.bottom);


	if((x - rcSliderBar.left) > pCurState->iClickPos){
		if(rcSliderBar.right == rcTotalBar.right - 1){
			pScrollInfo->nPos=pScrollInfo->nMax - pScrollInfo->nPage;
			pCurState->iSliderBarVal = iTotalBarWidth - iSliderBarWidth - 1;
			scrInvalidateRect(hWnd,&rcTotalBar,true);
		}
		else{
			if((x + iSliderBarWidth - pCurState->iClickPos)>=rcTotalBar.right){
				pScrollInfo->nPos=pScrollInfo->nMax - pScrollInfo->nPage;
				pCurState->iSliderBarVal = iTotalBarWidth - iSliderBarWidth - 1;
				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
			else{
				iPos = x -rcTotalBar.left - pCurState->iClickPos;
				pCurState->iSliderBarVal = iPos;
				pScrollInfo->nPos = iPos * (pScrollInfo->nMax - pScrollInfo->nMin + 1)/iTotalBarWidth;

				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
		}
		iRetMsg=SB_THUMBTRACK;
		if(IsControl(hWnd)){
			SendMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
		}
		else{
			SendNotifyMessage(hWnd,LMSG_HSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
		}
	}
	else{//move left
		if(rcSliderBar.left == rcTotalBar.left)
			return;
		else{
			if(x - rcTotalBar.left<= pCurState->iClickPos){
				pScrollInfo->nPos=pScrollInfo->nMin;
				pCurState->iSliderBarVal = 0;
				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
			else{
				iPos = x -rcTotalBar.left - pCurState->iClickPos;
				pCurState->iSliderBarVal = iPos;
				pScrollInfo->nPos = iPos * (pScrollInfo->nMax - pScrollInfo->nMin + 1)/iTotalBarWidth + pScrollInfo->nMin;
				scrInvalidateRect(hWnd,&rcTotalBar,true);
			}
			iRetMsg=SB_THUMBTRACK;
			if(IsControl(hWnd)){
				SendMessage(hWnd,LMSG_VSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
			else{
				SendNotifyMessage(hWnd,LMSG_HSCROLL,(WPARAM)iRetMsg,(LPARAM)NULL);
			}
		}
	}
}


BOOL 
wndDrawNCFrame(
	HWND hWnd,
	HDC hDC
)
{
	PWindowsTree pWin;
	BOOL bHScroll,bVScroll;
	RECT rcVBar,rcHBar,rcCorner;
	LPSCROLLCURSTATE pCurState;
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
	//draw bound
	wndDrawNCArea(hWnd, hDC);
	 //draw vert scroll
	 if(bVScroll){
		pCurState = pWin->pVCurState;
		wndGetVScrollBarRect(hWnd,&rcVBar);
		ScreenToWindowRect(hWnd,&rcVBar);
		wndDrawVScrollBar(&rcVBar,hDC,pWin->pVScroll->dwStatus,pCurState);
	}
	if(bHScroll){
		pCurState = pWin->pHCurState;
		wndGetHScrollBarRect(hWnd,&rcHBar);
		ScreenToWindowRect(hWnd,&rcHBar);
		wndDrawHScrollBar(&rcHBar,hDC,pWin->pHScroll->dwStatus,pCurState);
	}
	if(bVScroll && bHScroll){
		SetRect(&rcCorner,rcHBar.right,rcVBar.bottom,rcVBar.right,rcHBar.bottom);
		//draw right bottom corner box
		winFillRectangle(hDC,
			rcCorner.left+1,rcCorner.top,rcCorner.right,rcCorner.bottom,
			RGB_SYSTEMHIBRIGHT,RGB_SYSTEMHIBRIGHT);
	}

	return true;
}

//draw bound
BOOL 
wndDrawNCArea(
	HWND hWnd, 
	HDC hDC
)
{
	PWindowsTree pWin;
	RECT rcWin;
	POINT point;
	HPEN hPenGray;
	HPEN hPenBright;
	HPEN hPenDark;
	HPEN hPenBlack;
	
	PEN penBackup;
	pWin=(PWindowsTree)hWnd;
	if(!(pWin->dwStyle & WS_BORDER) && !(pWin->dwStyle & WS_THINBORDER) && !(pWin->dwStyle & WS_THICKBORDER))
		return false;

	
	if(!GetWindowRect(hWnd,&rcWin))
		return false;
	ScreenToWindowRect(hWnd,&rcWin);

	//backup current pen
	memcpy(&penBackup,hDC->hPen,sizeof(PEN));
	
	hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
	hPenGray	=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
	hPenBright	=CreatePen(PS_SOLID,1,RGB_SYSTEMBRIGHT);
	hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);

	SelectObject(hDC,hPenGray);
	winMoveToEx(hDC,rcWin.left,rcWin.bottom,&point);
	winLineTo(hDC,rcWin.left,rcWin.top);
	winLineTo(hDC,rcWin.right,rcWin.top);
	SelectObject(hDC,hPenBlack);
	winMoveToEx(hDC,rcWin.left,rcWin.bottom,&point);
	winLineTo(hDC,rcWin.right,rcWin.bottom);
	winLineTo(hDC,rcWin.right,rcWin.top);

	if(pWin->dwStyle & WS_BORDER)
		goto okret;

	SelectObject(hDC,hPenBright);
	winMoveToEx(hDC,rcWin.left+1,rcWin.bottom-1,&point);
	winLineTo(hDC,rcWin.left+1,rcWin.top+1);
	winLineTo(hDC,rcWin.right-1,rcWin.top+1);
	SelectObject(hDC,hPenDark);
	winMoveToEx(hDC,rcWin.left+1,rcWin.bottom-1,&point);
	winLineTo(hDC,rcWin.right-1,rcWin.bottom-1);
	winLineTo(hDC,rcWin.right-1,rcWin.top+1);

	if(pWin->dwStyle & WS_THINBORDER)
		goto okret;

	SelectObject(hDC,hPenDark);
	winMoveToEx(hDC,rcWin.left+2,rcWin.bottom-4,&point);
	winLineTo(hDC,rcWin.left+2,rcWin.top+2);
	winLineTo(hDC,rcWin.right-4,rcWin.top+2);
	SelectObject(hDC,hPenBright);
	winMoveToEx(hDC,rcWin.right-2,rcWin.top+1,&point);
	winLineTo(hDC,rcWin.right-2,rcWin.bottom-2);
	SelectObject(hDC,hPenBlack);
	winMoveToEx(hDC,rcWin.left+3,rcWin.bottom-4,&point);
	winLineTo(hDC,rcWin.left+3,rcWin.top+3);
	winLineTo(hDC,rcWin.right-4,rcWin.top+3);
	SelectObject(hDC,hPenGray);
	winMoveToEx(hDC,rcWin.right-3,rcWin.top+2,&point);
	winLineTo(hDC,rcWin.right-3,rcWin.bottom-3);
	winLineTo(hDC,rcWin.left+2,rcWin.bottom-3);


okret:
	//restore pen
	memcpy(hDC->hPen,&penBackup,sizeof(PEN));

	if(IsCaption(hWnd)){
		PaintCaption(hWnd,hDC);
	}
	//destroy the hdc object
	DeleteObject(hPenBlack);
	DeleteObject(hPenGray);
	DeleteObject(hPenBright);
	DeleteObject(hPenDark);
	return true;

}

BOOL 
PaintCaption(
	HWND hWnd,
	HDC hDC
)
{
	COLORREF crTextColor;
	char pString[256];
	int iLeft;
	BOOL bCloseBox;
	PWindowsTree pWin;
	int iBorder;
	RECT rcWin,rcCaption;
	HGDIOBJ  hFont;
	pWin = (PWindowsTree)hWnd;
	iBorder = wndGetBorder(hWnd);
	if(!GetWindowRect(hWnd,&rcWin))
		return;
	ScreenToWindowRect(hWnd,&rcWin);
	SetRect(&rcCaption,rcWin.left + iBorder, rcWin.top + iBorder,
		rcWin.right - iBorder, rcWin.top + iBorder + WIN_CAPTION_HEIGHT-1);
	//backup text color
	crTextColor = hDC->crTextColor;
	if(IsActive(hWnd)){
		SetTextColor(hDC,RGB_WHITE);

		winFillRectangle(hDC,rcCaption.left,rcCaption.top,
			rcCaption.right, rcCaption.bottom, WIN_CAPTION_ACTIVE_COLOR, WIN_CAPTION_ACTIVE_COLOR);
	}
	else{
		SetTextColor(hDC,RGB_BLACK);

		winFillRectangle(hDC,rcCaption.left,rcCaption.top,
			rcCaption.right, rcCaption.bottom, WIN_CAPTION_DISACTIVE_COLOR, WIN_CAPTION_DISACTIVE_COLOR);
	}

	hFont = GetStockObject(FONT_12_12);
	SelectObject(hDC,hFont);
	DeleteObject(hFont);
	//draw caption text
	winDrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),&rcCaption,DT_LEFT|DT_VCENTER);
	//restore text color
	SetTextColor(hDC,crTextColor);
	//CLOSE BOX
	if(IsCloseBox(hWnd))
		bCloseBox = true;
	else
		bCloseBox = false;
	if(bCloseBox){
		strcpy(pString,pEnvStr);
		strcat(pString,"/pics/sys/");
		if(IsCloseBoxPressed(hWnd))
			strcat(pString,WIN_CLOSEBOX_FILESPEC_1);
		else
			strcat(pString,WIN_CLOSEBOX_FILESPEC_0);

		winShowBitmapWithTrColor(hDC,rcCaption.right - WIN_CLOSEBOX_WIDTH,iBorder,
			pString,RGB_BLACK);
	}
	//OK BOX
	if(IsOkButton(hWnd)){
		strcpy(pString,pEnvStr);
		strcat(pString,"/pics/sys/");
		if(IsOkButtonPressed(hWnd))
			strcat(pString, WIN_OKBUTTON_FILESPEC_1);
		else
			strcat(pString, WIN_OKBUTTON_FILESPEC_0);
			
		iLeft = rcCaption.right - WIN_OKBUTTON_WIDTH;
		if(bCloseBox)
			iLeft -= WIN_CLOSEBOX_WIDTH;
		winShowBitmapWithTrColor(hDC,iLeft,iBorder,pString,RGB_BLACK);
	}

	return true;
}




void 
wndDrawVScrollBar(
	LPRECT lpRect,
	HDC hDC,
	DWORD dwStyle,
	LPSCROLLCURSTATE pCurState
)
{
	int iArrowHeight,iWinWidth,iWinHeight;
	int iPos,iSliderBarHeight,iTotalBarHeight;
	RECT rcDownButton,rcUpButton,rcTotalBar,rcSliderBar;

	HPEN hPenBlack,hPenGray,hPenBright,hPenDark;

	PEN penBackup;
	
	POINT point;
	int y;

	iWinWidth	=lpRect->right - lpRect->left + 1;
	iWinHeight	=lpRect->bottom - lpRect->top + 1;
	iArrowHeight=GetSystemMetrics(SM_CYVSCROLL);
	iTotalBarHeight = iWinHeight- 2 * iArrowHeight;

	//DownButton
	SetRect(&rcDownButton,lpRect->left,lpRect->bottom-iArrowHeight,lpRect->right,lpRect->bottom);
	//UpButton
	SetRect(&rcUpButton,lpRect->left,lpRect->bottom-2*iArrowHeight,lpRect->right,lpRect->bottom-iArrowHeight);
	//TotalBar
	SetRect(&rcTotalBar,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom-iArrowHeight);

	hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
	hPenGray	=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
	hPenBright	=CreatePen(PS_SOLID,1,RGB_SYSTEMBRIGHT);
	hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);

	memcpy(&penBackup, hDC->hPen,sizeof(PEN));

	winMoveToEx(hDC,rcTotalBar.left,rcTotalBar.top,&point);
	winLineTo(hDC,rcTotalBar.left,rcTotalBar.bottom);

	winFillRectangle(hDC,
		rcTotalBar.left+1,rcTotalBar.top,rcTotalBar.right,rcTotalBar.bottom,
		RGB_SYSTEMHIBRIGHT,RGB_SYSTEMHIBRIGHT);

	
	//1.down arrow
	if((dwStyle & SS_DOWNRIGHT_PRESSDOWN)!=0){//DOWN BUTTON PRESSED
		winFillRectangle(hDC,rcDownButton.left,rcDownButton.top,rcDownButton.right,rcDownButton.bottom,
			RGB_SYSTEMBRIGHT,RGB_SYSTEMDARK);
		//Draw the triangle
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcDownButton.left+4,rcDownButton.top+7,&point);
		winLineTo(hDC,rcDownButton.left+8,rcDownButton.top+7);
		winMoveToEx(hDC,rcDownButton.left+5,rcDownButton.top+8,&point);
		winLineTo(hDC,rcDownButton.left+7,rcDownButton.top+8);
		winSetPixel(hDC,rcDownButton.left+6,rcDownButton.top+9,RGB_SYSTEMBLACK);
	}
	else{//DOWN BUTTON NOT PRESSED
		//left & top bound
		SelectObject(hDC,hPenBright);
		winMoveToEx(hDC,rcDownButton.right-1,rcDownButton.top,&point);
		winLineTo(hDC,rcDownButton.left,rcDownButton.top);
		winLineTo(hDC,rcDownButton.left,rcDownButton.bottom-1);
		//Center
		for(y=rcDownButton.top+2;y<=rcDownButton.bottom-2;y++){
			winMoveToEx(hDC,rcDownButton.left+2,y,&point);
			winLineTo(hDC,rcDownButton.right-2,y);
		}
		//left & top bright bound
		SelectObject(hDC,hPenGray);
		winMoveToEx(hDC,rcDownButton.right-2,rcDownButton.top+1,&point);
		winLineTo(hDC,rcDownButton.left+1,rcDownButton.top+1);
		winLineTo(hDC,rcDownButton.left+1,rcDownButton.bottom-2);
		//right & bottom dark bound
		SelectObject(hDC,hPenDark);
		winMoveToEx(hDC,rcDownButton.left+1,rcDownButton.bottom-1,&point);
		winLineTo(hDC,rcDownButton.right-1,rcDownButton.bottom-1);
		winLineTo(hDC,rcDownButton.right-1,rcDownButton.top+1);
		//bottom & right black bound
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcDownButton.left,rcDownButton.bottom,&point);
		winLineTo(hDC,rcDownButton.right,rcDownButton.bottom);
		winLineTo(hDC,rcDownButton.right,rcDownButton.top);
		//the triangle
		winMoveToEx(hDC,rcDownButton.left+4,rcDownButton.top+6,&point);
		winLineTo(hDC,rcDownButton.left+8,rcDownButton.top+6);
		winMoveToEx(hDC,rcDownButton.left+5,rcDownButton.top+7,&point);
		winLineTo(hDC,rcDownButton.left+7,rcDownButton.top+7);
		winSetPixel(hDC,rcDownButton.left+6,rcDownButton.top+8,RGB_SYSTEMBLACK);
		//
	}

	//2.up arrow
	if((dwStyle & SS_UPLEFT_PRESSDOWN)!=0){//UP BUTTON PRESSED
		winFillRectangle(hDC,rcUpButton.left,rcUpButton.top,rcUpButton.right,rcUpButton.bottom,
			RGB_SYSTEMBRIGHT,RGB_SYSTEMDARK);
		//Draw the triangle
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcUpButton.left+5,rcUpButton.top+9,&point);
		winLineTo(hDC,rcUpButton.left+9,rcUpButton.top+9);
		winMoveToEx(hDC,rcUpButton.left+6,rcUpButton.top+8,&point);
		winLineTo(hDC,rcUpButton.left+8,rcUpButton.top+8);
		winSetPixel(hDC,rcUpButton.left+7,rcUpButton.top+7,RGB_SYSTEMBLACK);


	}
	else{//UP BUTTON NOT PRESSED
		//left & top bound
		SelectObject(hDC,hPenBright);
		winMoveToEx(hDC,rcUpButton.right-1,rcUpButton.top,&point);
		winLineTo(hDC,rcUpButton.left,rcUpButton.top);
		winLineTo(hDC,rcUpButton.left,rcUpButton.bottom-1);
		//Center
		for(y=rcUpButton.top+2;y<=rcUpButton.bottom-2;y++){
			winMoveToEx(hDC,rcUpButton.left+2,y,&point);
			winLineTo(hDC,rcUpButton.right-2,y);
		}
		//left & top bright bound
		SelectObject(hDC,hPenGray);
		winMoveToEx(hDC,rcUpButton.right-2,rcUpButton.top+1,&point);
		winLineTo(hDC,rcUpButton.left+1,rcUpButton.top+1);
		winLineTo(hDC,rcUpButton.left+1,rcUpButton.bottom-2);
		//right & bottom dark bound
		SelectObject(hDC,hPenDark);
		winMoveToEx(hDC,rcUpButton.left+1,rcUpButton.bottom-1,&point);
		winLineTo(hDC,rcUpButton.right-1,rcUpButton.bottom-1);
		winLineTo(hDC,rcUpButton.right-1,rcUpButton.top+1);
		//bottom & right bound
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcUpButton.left,rcUpButton.bottom,&point);
		winLineTo(hDC,rcUpButton.right,rcUpButton.bottom);
		winLineTo(hDC,rcUpButton.right,rcUpButton.top);
		//the triangle
		winMoveToEx(hDC,rcUpButton.left+4,rcUpButton.top+8,&point);
		winLineTo(hDC,rcUpButton.left+8,rcUpButton.top+8);
		winMoveToEx(hDC,rcUpButton.left+5,rcUpButton.top+7,&point);
		winLineTo(hDC,rcUpButton.left+7,rcUpButton.top+7);
		winSetPixel(hDC,rcUpButton.left+6,rcUpButton.top+6,RGB_SYSTEMBLACK);
	}
	//3.bar
	
	if(ScrollBarIsEnable(((PWindowsTree)(hDC->hWnd))->pVScroll)){
		//SliderBar
		iPos = pCurState->iSliderBarVal;

		iSliderBarHeight = pCurState->iSliderBarLen;

		SetRect(&rcSliderBar,lpRect->left,lpRect->top+iPos,lpRect->right,lpRect->top+iPos+iSliderBarHeight);


		//left top
		SelectObject(hDC,hPenBright);
		winMoveToEx(hDC,rcSliderBar.right-1,rcSliderBar.top,&point);
		winLineTo(hDC,rcSliderBar.left,rcSliderBar.top);
		winLineTo(hDC,rcSliderBar.left,rcSliderBar.bottom-1);
		//center
		winFillRectangle(hDC,rcSliderBar.left+2,rcSliderBar.top+2,
			rcSliderBar.right-2,rcSliderBar.bottom-2,
			RGB_SYSTEMBRIGHT,RGB_SYSTEMBRIGHT);

		//right bottom
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcSliderBar.left,rcSliderBar.bottom,&point);
		winLineTo(hDC,rcSliderBar.right,rcSliderBar.bottom);
		winLineTo(hDC,rcSliderBar.right,rcSliderBar.top);

		//left top gray bound
		SelectObject(hDC,hPenGray);
		winMoveToEx(hDC,rcSliderBar.right-2,rcSliderBar.top+1,&point);
		winLineTo(hDC,rcSliderBar.left+1,rcSliderBar.top+1);
		winLineTo(hDC,rcSliderBar.left+1,rcSliderBar.bottom-2);

		//right bottom dark bound
		SelectObject(hDC,hPenDark);
		winMoveToEx(hDC,rcSliderBar.right-1,rcSliderBar.top+1,&point);
		winLineTo(hDC,rcSliderBar.right-1,rcSliderBar.bottom-1);
		winLineTo(hDC,rcSliderBar.left+1,rcSliderBar.bottom-1);
	}
	//restore pen
	memcpy(hDC->hPen,&penBackup,sizeof(PEN));
	
	//destroy the hdc object
	DeleteObject(hPenBlack);
	DeleteObject(hPenGray);
	DeleteObject(hPenBright);
	DeleteObject(hPenDark);
}


void 
wndDrawHScrollBar(
	LPRECT lpRect,
	HDC hDC,
	DWORD dwStyle,
	LPSCROLLCURSTATE pCurState
)
{
	int iArrowWidth,iWinWidth,iWinHeight;
	int iPos,iSliderBarWidth,iTotalBarWidth;
	RECT rcRightButton,rcLeftButton,rcTotalBar,rcSliderBar;

	HPEN hPenBlack,hPenGray,hPenBright,hPenDark;

	POINT point;
	int y;
	PEN penBackup;
	
	iWinWidth	=lpRect->right - lpRect->left + 1;
	iWinHeight	=lpRect->bottom - lpRect->top + 1;
	iArrowWidth=GetSystemMetrics(SM_CXHSCROLL);
	iTotalBarWidth = iWinWidth- 2 * iArrowWidth;

	//RightButton
	SetRect(&rcRightButton,lpRect->right-iArrowWidth,lpRect->top,lpRect->right,lpRect->bottom);
	//LeftButton
	SetRect(&rcLeftButton,lpRect->right-2*iArrowWidth,lpRect->top,lpRect->right-iArrowWidth,lpRect->bottom);
	//TotalBar
	SetRect(&rcTotalBar,lpRect->left,lpRect->top,lpRect->right-iArrowWidth,lpRect->bottom);

	hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
	hPenGray	=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
	hPenBright	=CreatePen(PS_SOLID,1,RGB_SYSTEMBRIGHT);
	hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);
	
	memcpy(&penBackup,hDC->hPen,sizeof(PEN));
	
	SelectObject(hDC,hPenBlack);

	winMoveToEx(hDC,rcTotalBar.left,rcTotalBar.top,&point);
	winLineTo(hDC,rcTotalBar.right,rcTotalBar.top);

	winFillRectangle(hDC,
		rcTotalBar.left,rcTotalBar.top + 1,rcTotalBar.right,rcTotalBar.bottom,
		RGB_SYSTEMHIBRIGHT,RGB_SYSTEMHIBRIGHT);


	//1.right arrow
	if((dwStyle & SS_DOWNRIGHT_PRESSDOWN)!=0){//DOWN BUTTON PRESSED
		winFillRectangle(hDC,rcRightButton.left,rcRightButton.top,rcRightButton.right,rcRightButton.bottom,
			RGB_SYSTEMBRIGHT,RGB_SYSTEMDARK);
		//Draw the triangle
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcRightButton.left+7,rcRightButton.top+5,&point);
		winLineTo(hDC,rcRightButton.left+7,rcRightButton.top+9);
		winMoveToEx(hDC,rcRightButton.left+8,rcRightButton.top+6,&point);
		winLineTo(hDC,rcRightButton.left+8,rcRightButton.top+8);
		winSetPixel(hDC,rcRightButton.left+9,rcRightButton.top+7,RGB_SYSTEMBLACK);

	}
	else{//DOWN BUTTON NOT PRESSED
		 //left & top bound
		//printf("down right not pressed\n");
		SelectObject(hDC,hPenBright);
		winMoveToEx(hDC,rcRightButton.left,rcRightButton.bottom-1,&point);
		winLineTo(hDC,rcRightButton.left,rcRightButton.top);
		winLineTo(hDC,rcRightButton.right-1,rcRightButton.top);
		//Center
		for(y=rcRightButton.top+2;y<=rcRightButton.bottom-2;y++){
			winMoveToEx(hDC,rcRightButton.left+2,y,&point);
			winLineTo(hDC,rcRightButton.right-2,y);
		}
		//left & top bright bound
		SelectObject(hDC,hPenGray);
		winMoveToEx(hDC,rcRightButton.left+1,rcRightButton.bottom-2,&point);
		winLineTo(hDC,rcRightButton.left+1,rcRightButton.top+1);
		winLineTo(hDC,rcRightButton.right-2,rcRightButton.top+1);
		//right & bottom dark bound
		SelectObject(hDC,hPenDark);
		winMoveToEx(hDC,rcRightButton.left+1,rcRightButton.bottom-1,&point);
		winLineTo(hDC,rcRightButton.right-1,rcRightButton.bottom-1);
		winLineTo(hDC,rcRightButton.right-1,rcRightButton.top+1);
		//bottom & right black bound
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcRightButton.left,rcRightButton.bottom,&point);
		winLineTo(hDC,rcRightButton.right,rcRightButton.bottom);
		winLineTo(hDC,rcRightButton.right,rcRightButton.top);

		//Draw the triangle
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcRightButton.left+6,rcRightButton.top+4,&point);
		winLineTo(hDC,rcRightButton.left+6,rcRightButton.top+8);
		winMoveToEx(hDC,rcRightButton.left+7,rcRightButton.top+5,&point);
		winLineTo(hDC,rcRightButton.left+7,rcRightButton.top+7);
		winSetPixel(hDC,rcRightButton.left+8,rcRightButton.top+6,RGB_SYSTEMBLACK);
	}

	//2.left arrow
	if((dwStyle & SS_UPLEFT_PRESSDOWN)!=0){//UP BUTTON PRESSED
		winFillRectangle(hDC,rcLeftButton.left,rcLeftButton.top,rcLeftButton.right,rcLeftButton.bottom,
			RGB_SYSTEMBRIGHT,RGB_SYSTEMDARK);
		//Draw the triangle
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcLeftButton.left+8,rcLeftButton.top+5,&point);
		winLineTo(hDC,rcLeftButton.left+8,rcLeftButton.top+9);
		winMoveToEx(hDC,rcLeftButton.left+7,rcLeftButton.top+6,&point);
		winLineTo(hDC,rcLeftButton.left+7,rcLeftButton.top+8);
		winSetPixel(hDC,rcLeftButton.left+6,rcLeftButton.top+7,RGB_SYSTEMBLACK);
	}
	else{//Left BUTTON NOT PRESSED
		//left & top bound
		SelectObject(hDC,hPenBright);
		winMoveToEx(hDC,rcLeftButton.left,rcLeftButton.bottom-1,&point);
		winLineTo(hDC,rcLeftButton.left,rcLeftButton.top);
		winLineTo(hDC,rcLeftButton.right-1,rcLeftButton.top);
		//Center
		for(y=rcLeftButton.top+2;y<=rcLeftButton.bottom-2;y++){
			winMoveToEx(hDC,rcLeftButton.left+2,y,&point);
			winLineTo(hDC,rcLeftButton.right-2,y);
		}
		//left & top bright bound

		SelectObject(hDC,hPenGray);
		winMoveToEx(hDC,rcLeftButton.left+1,rcLeftButton.bottom-1,&point);
		winLineTo(hDC,rcLeftButton.left+1,rcLeftButton.top+1);
		winLineTo(hDC,rcLeftButton.right-1,rcLeftButton.top+1);
		//right & bottom dark bound
		SelectObject(hDC,hPenDark);
		winMoveToEx(hDC,rcLeftButton.left+1,rcLeftButton.bottom-1,&point);
		winLineTo(hDC,rcLeftButton.right-1,rcLeftButton.bottom-1);
		winLineTo(hDC,rcLeftButton.right-1,rcLeftButton.top+1);
		//bottom & right bound
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcLeftButton.left,rcLeftButton.bottom,&point);
		winLineTo(hDC,rcLeftButton.right,rcLeftButton.bottom);
		winLineTo(hDC,rcLeftButton.right,rcLeftButton.top);
		//the triangle
		winMoveToEx(hDC,rcLeftButton.left+7,rcLeftButton.top+4,&point);
		winLineTo(hDC,rcLeftButton.left+7,rcLeftButton.top+8);
		winMoveToEx(hDC,rcLeftButton.left+6,rcLeftButton.top+5,&point);
		winLineTo(hDC,rcLeftButton.left+6,rcLeftButton.top+7);
		winSetPixel(hDC,rcLeftButton.left+5,rcLeftButton.top+6,RGB_SYSTEMBLACK);

	}
	if(ScrollBarIsEnable(((PWindowsTree)(hDC->hWnd))->pHScroll)){
		//SliderBar
		iPos = pCurState->iSliderBarVal;

		iSliderBarWidth = pCurState->iSliderBarLen;

		SetRect(&rcSliderBar,lpRect->left+iPos,lpRect->top,lpRect->left+iPos+iSliderBarWidth,lpRect->bottom);

		//3.bar
		//left top
		SelectObject(hDC,hPenBright);
		winMoveToEx(hDC,rcSliderBar.left,rcSliderBar.bottom-1,&point);
		winLineTo(hDC,rcSliderBar.left,rcSliderBar.top);
		winLineTo(hDC,rcSliderBar.right-1,rcSliderBar.top);
		//center
		winFillRectangle(hDC,rcSliderBar.left+2,rcSliderBar.top+2,
			rcSliderBar.right-2,rcSliderBar.bottom-2,
			RGB_SYSTEMBRIGHT,RGB_SYSTEMBRIGHT);

		//right bottom
		SelectObject(hDC,hPenBlack);
		winMoveToEx(hDC,rcSliderBar.left,rcSliderBar.bottom,&point);
		winLineTo(hDC,rcSliderBar.right,rcSliderBar.bottom);
		winLineTo(hDC,rcSliderBar.right,rcSliderBar.top);

		//left top gray bound
		SelectObject(hDC,hPenGray);
		winMoveToEx(hDC,rcSliderBar.left+1,rcSliderBar.bottom-2,&point);
		winLineTo(hDC,rcSliderBar.left+1,rcSliderBar.top+1);
		winLineTo(hDC,rcSliderBar.right-1,rcSliderBar.top+1);

		//right bottom dark bound
		SelectObject(hDC,hPenDark);
		winMoveToEx(hDC,rcSliderBar.left+1,rcSliderBar.bottom-1,&point);
		winLineTo(hDC,rcSliderBar.right-1,rcSliderBar.bottom-1);
		winLineTo(hDC,rcSliderBar.right-1,rcSliderBar.top+1);
	}
	
	//restore pen
	memcpy(hDC->hPen,&penBackup,sizeof(PEN));
	//destroy the hdc object
	DeleteObject(hPenBlack);
	DeleteObject(hPenGray);
	DeleteObject(hPenBright);
	DeleteObject(hPenDark);
}


//=======================================ScrollBar===========================================

BOOL 
wndGetVScrollBarRect(
	HWND hWnd,
	LPRECT lpRect
)
{
	PWindowsTree pWin;
	RECT rcWin;
	int iWidth,iHeight;
	int iBorder;
	int iCaption;
	pWin=(PWindowsTree)hWnd;


	if(!pWin)
		return false;
	if(!(pWin->dwStyle & WS_VSCROLL)){
		SetRect(lpRect,0,0,0,0);
		return false;
	}
	iBorder=wndGetBorder(hWnd);
	GetWindowRect(hWnd,&rcWin);
	if(IsCaption(hWnd))
		iCaption = WIN_CAPTION_HEIGHT;
	else
		iCaption = 0;

	iWidth=GetSystemMetrics(SM_CXVSCROLL);
	iHeight=GetSystemMetrics(SM_CYHSCROLL);
	if(pWin->dwStyle & WS_HSCROLL){
		SetRect(lpRect,rcWin.right+1-iWidth-iBorder,rcWin.top+iBorder+iCaption+1,
			rcWin.right-iBorder,rcWin.bottom-iBorder-iHeight);
	}
	else{
		SetRect(lpRect,rcWin.right+1-iWidth-iBorder,rcWin.top+iBorder+iCaption+1,
			rcWin.right-iBorder,rcWin.bottom-iBorder);
	}			 	
	
	return true;
}


BOOL 
wndGetHScrollBarRect(
	HWND hWnd,
	LPRECT lpRect
)
{
	PWindowsTree pWin;
	RECT rcWin;
	int iWidth,iHeight;
	int iBorder;
	int iCaption;
	pWin=(PWindowsTree)hWnd;

	if(!pWin)
		return false;
	if(!(pWin->dwStyle & WS_HSCROLL)){
		SetRect(lpRect,0,0,0,0);
		return false;
	}
	GetWindowRect(hWnd,&rcWin);
	iBorder=wndGetBorder(hWnd);

	if(IsCaption(hWnd))
		iCaption = WIN_CAPTION_HEIGHT;
	else
		iCaption = 0;
		
	iWidth=GetSystemMetrics(SM_CXVSCROLL);
	iHeight=GetSystemMetrics(SM_CYHSCROLL);
	if(pWin->dwStyle & WS_VSCROLL){
		SetRect(lpRect,rcWin.left+iBorder,rcWin.bottom+1-iHeight,
			rcWin.right-iBorder-iWidth,rcWin.bottom-iBorder);
	}
	else{
		SetRect(lpRect,rcWin.left+iBorder,rcWin.bottom+1-iHeight,
			rcWin.right-iBorder,rcWin.bottom-iBorder);
	}
	return true;
}


static void 
CalVScrollCurState(
	LPRECT lpRect,
	LPSCROLLINFO pScrollInfo,
	LPSCROLLCURSTATE pCurState
)
{
	RECT rcTotalBar;
	int iWinHeight, iArrowHeight,iTotalBarHeight,iSliderBarHeight, iPos;

	iWinHeight	=lpRect->bottom - lpRect->top + 1;
	iArrowHeight=GetSystemMetrics(SM_CYVSCROLL);
	//TotalBar
	SetRect(&rcTotalBar,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom-2*iArrowHeight);
	//height of total bar
	iTotalBarHeight = iWinHeight- 2 * iArrowHeight;

	if(pScrollInfo->nMax - pScrollInfo->nMin + 1 <= pScrollInfo->nPage)
		iSliderBarHeight = iTotalBarHeight - 1;
	else{
		iSliderBarHeight = iTotalBarHeight * pScrollInfo->nPage / (pScrollInfo->nMax - pScrollInfo->nMin + 1);
		if(iSliderBarHeight<MIN_SLIDERBAR_LENGTH)
			iSliderBarHeight=MIN_SLIDERBAR_LENGTH;
	}
	pCurState->iSliderBarLen = iSliderBarHeight;


	iPos = (pScrollInfo->nPos - pScrollInfo->nMin) * iTotalBarHeight/(pScrollInfo->nMax - pScrollInfo->nMin + 1);
	if(iPos < 0)
		iPos = 0;
	if((rcTotalBar.bottom - iPos)< iSliderBarHeight)
		iPos=rcTotalBar.bottom - iSliderBarHeight;

	pCurState->iSliderBarVal = iPos;
}

static void 
CalHScrollCurState(
	LPRECT lpRect,
	LPSCROLLINFO pScrollInfo,
	LPSCROLLCURSTATE pCurState
)
{
	RECT rcTotalBar;
	int iWinWidth,iArrowWidth,iTotalBarWidth,iSliderBarWidth, iPos;

	iWinWidth = lpRect->right - lpRect->left + 1;
	iArrowWidth = GetSystemMetrics(SM_CXHSCROLL);
	SetRect(&rcTotalBar,lpRect->left,lpRect->top,lpRect->right-2*iArrowWidth,lpRect->bottom);


	iTotalBarWidth = iWinWidth - 2* iArrowWidth;

	if(pScrollInfo->nMax - pScrollInfo->nMin + 1 <= pScrollInfo->nPage)
		iSliderBarWidth = iTotalBarWidth - 1;
	else{
		iSliderBarWidth = iTotalBarWidth * pScrollInfo->nPage / (pScrollInfo->nMax - pScrollInfo->nMin + 1);
		if(iSliderBarWidth<MIN_SLIDERBAR_LENGTH)
			iSliderBarWidth=MIN_SLIDERBAR_LENGTH;
	}
	pCurState->iSliderBarLen = iSliderBarWidth;

	iPos = (pScrollInfo->nPos - pScrollInfo->nMin) * iTotalBarWidth/(pScrollInfo->nMax - pScrollInfo->nMin + 1);
	if(iPos < 0)
		iPos = 0;
	if((rcTotalBar.right - iPos)< iSliderBarWidth)
		iPos=rcTotalBar.right - iSliderBarWidth;
	pCurState->iSliderBarVal = iPos;

}


//enable  scrollbar of a window
BOOL GUIAPI 
EnableScrollBar (
	HWND hWnd, 
	int iSBar, 
	BOOL bEnable
)
{
    LPSCROLLINFO pSBar;
    PWindowsTree pWin;
    RECT rcBar;

    pWin = (PWindowsTree)hWnd;


    if (iSBar == SB_HORZ) {
        if (pWin->dwStyle & WS_HSCROLL)
            pSBar=pWin->pHScroll;
		else
			return false;
    }
    else if (iSBar == SB_VERT) {
        if (pWin->dwStyle & WS_VSCROLL)
            pSBar=pWin->pVScroll;
		else
			return false;
    }

	if(bEnable){
		if(pSBar->dwStatus & SS_DISABLED)
			pSBar->dwStatus &= ~SS_DISABLED;
	}
	else{
		if(!(pSBar->dwStatus & SS_DISABLED))
			pSBar->dwStatus |=SS_DISABLED;
	}
    if (iSBar == SB_VERT)
        wndGetVScrollBarRect (pWin, &rcBar);
    else
        wndGetHScrollBarRect (pWin, &rcBar);

	InvalidateRect(hWnd,&rcBar,true);

    return TRUE;
}


int GUIAPI 
SetScrollInfo(
	HWND hWnd, 
	int fnBar, 
	LPSCROLLINFO lpsi, 
	BOOL fRedraw
)
{
	PWindowsTree pWin;
	LPSCROLLINFO pScrollInfo;
	RECT rc;

	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;

	if(fnBar == SB_CTL){
		if(pWin->dwStyle & WS_VSCROLL)
			pScrollInfo = pWin->pVScroll;
		else
			pScrollInfo = pWin->pHScroll;
	}
	else{
		if(fnBar == SB_VERT){
			pScrollInfo = pWin->pVScroll;
		}
		else{
			pScrollInfo = pWin->pHScroll;
		}
	}

	if(lpsi->fMask & SIF_RANGE){
		pScrollInfo->nMin=lpsi->nMin;
		pScrollInfo->nMax=lpsi->nMax;
	}

	if(lpsi->fMask & SIF_PAGE){
		pScrollInfo->nPage=lpsi->nPage;
	}
	if(lpsi->fMask & SIF_POS){
		if(lpsi->nPos < pScrollInfo->nMin)
			pScrollInfo->nPos=pScrollInfo->nMin;
		else if(lpsi->nPos > (pScrollInfo->nMax - pScrollInfo->nPage))
			pScrollInfo->nPos=pScrollInfo->nMax - pScrollInfo->nPage;
		else
			pScrollInfo->nPos=lpsi->nPos;
		pScrollInfo->nTrackPos=lpsi->nPos;
	}


	if(fnBar == SB_CTL){
		if(pWin->dwStyle & WS_VSCROLL){
			wndGetVScrollBarRect(hWnd,&rc);
			CalVScrollCurState(&rc,pScrollInfo,pWin->pVCurState);
		}
		else{
			wndGetHScrollBarRect(hWnd,&rc);
			CalHScrollCurState(&rc,pScrollInfo,pWin->pHCurState);
		}
			pScrollInfo = pWin->pHScroll;
	}
	else{
		if(fnBar == SB_VERT){
			wndGetVScrollBarRect(hWnd,&rc);
			CalVScrollCurState(&rc,pScrollInfo,pWin->pVCurState);
		}
		else{
			wndGetHScrollBarRect(hWnd,&rc);
			CalHScrollCurState(&rc,pScrollInfo,pWin->pHCurState);
		}
	}


	if(fRedraw)
		scrInvalidateRect(hWnd,NULL,true);

	return true;
}
int GUIAPI 
GetScrollInfo(
	HWND hWnd, 
	LPSCROLLINFO lpsi
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(IsVScrollBar(hWnd))
		lpsi = pWin->pVScroll;
	else
		lpsi = pWin->pHScroll;
	return true;
}



int GUIAPI 
SetScrollPos(
	HWND hWnd,
	int nBar,
	int nPos,
	BOOL bRedraw
)
{
	PWindowsTree pWin;
	LPSCROLLINFO pScrollInfo;

	RECT rc;
	pWin=(PWindowsTree)hWnd;
	if(!pWin)
		return false;

	if(nBar == SB_CTL){
		if(pWin->dwStyle & WS_VSCROLL)
			pScrollInfo = pWin->pVScroll;
		else
			pScrollInfo = pWin->pHScroll;
	}
	else{
		if(nBar == SB_VERT){
			if(!(pWin->dwStyle & WS_VSCROLL))
				return false;
			else
				pScrollInfo = pWin->pVScroll;
		}
		else{
			if(!(pWin->dwStyle & WS_HSCROLL))
				return false;
			else
				pScrollInfo = pWin->pHScroll;
		}
	}
	if(nPos < pScrollInfo->nMin)
		pScrollInfo->nPos = pScrollInfo->nMin;
	else if(nPos > (pScrollInfo->nMax - pScrollInfo->nPage))
		pScrollInfo->nPos =pScrollInfo->nMax - pScrollInfo->nPage;
	else
		pScrollInfo->nPos=nPos;

	if(nBar == SB_CTL){
		if(pWin->dwStyle & WS_VSCROLL){
			wndGetVScrollBarRect(hWnd,&rc);
			CalVScrollCurState(&rc,pScrollInfo,pWin->pVCurState);
		}
		else{
			wndGetHScrollBarRect(hWnd,&rc);
			CalHScrollCurState(&rc,pScrollInfo,pWin->pHCurState);
		}
	}
	else{
		if(nBar == SB_VERT){
			wndGetVScrollBarRect(hWnd,&rc);
			CalVScrollCurState(&rc,pScrollInfo,pWin->pVCurState);
		}
		else{
			wndGetHScrollBarRect(hWnd,&rc);
			CalHScrollCurState(&rc,pScrollInfo,pWin->pHCurState);
		}
	}
	
	if(bRedraw)
		scrInvalidateRect(hWnd,NULL,true);
	return true;
}

BOOL GUIAPI 
SetScrollRange(
	HWND hWnd,
	int nBar,
	int nMinPos,
	int nMaxPos,
	BOOL bRedraw
)
{
	PWindowsTree pWin;
	LPSCROLLINFO pScrollInfo;
	RECT rc;

	pWin=(PWindowsTree)hWnd;
	if(nBar == SB_CTL){
		if(pWin->dwStyle & WS_VSCROLL)
			pScrollInfo = pWin->pVScroll;
		else
			pScrollInfo = pWin->pHScroll;
	}
	else{
		if(nBar == SB_VERT){
			if(!(pWin->dwStyle & WS_VSCROLL))
				return false;
			else
				pScrollInfo = pWin->pVScroll;
		}
		else{
			if(!(pWin->dwStyle & WS_HSCROLL))
				return false;
			else
				pScrollInfo = pWin->pHScroll;
		}
	}

	pScrollInfo->nMin=nMinPos;
	pScrollInfo->nMax=nMaxPos;


	if(nBar == SB_CTL){
		if(pWin->dwStyle & WS_VSCROLL){
			wndGetVScrollBarRect(hWnd,&rc);
			CalVScrollCurState(&rc,pScrollInfo,pWin->pVCurState);
		}
		else{
			wndGetHScrollBarRect(hWnd,&rc);
			CalHScrollCurState(&rc,pScrollInfo,pWin->pHCurState);
		}
	}
	else{
		if(nBar == SB_VERT){
			wndGetVScrollBarRect(hWnd,&rc);
			CalVScrollCurState(&rc,pScrollInfo,pWin->pVCurState);
		}
		else{
			wndGetHScrollBarRect(hWnd,&rc);
			CalHScrollCurState(&rc,pScrollInfo,pWin->pHCurState);
		}
	}

	if(bRedraw)
		scrInvalidateRect(hWnd,NULL,true);
	return true;
}




BOOL 
IsVScrollBar(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(pWin->dwStyle & WS_VSCROLL)
		return true;
	else
		return false;
}


BOOL 
IsHScrollBar(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if(pWin->dwStyle & WS_HSCROLL)
		return true;
	else
		return false;
}




static BOOL 
ScrollBarIsEnable(
	LPSCROLLINFO lpsi
)
{
	if(lpsi->dwStatus & SS_DISABLED)
		return false;
	else
		return true;

}



static LPSCROLLINFO 
wndGetScrollBar (
	HWND hWnd, 
	int iSBar
)
{
	PWindowsTree pWin;
    pWin = (PWindowsTree)hWnd;

    if (iSBar == SB_HORZ) {
        if (pWin->dwStyle & WS_HSCROLL)
			return pWin->pHScroll;
    }
    else if (iSBar == SB_VERT) {
        if (pWin->dwStyle & WS_VSCROLL)
			return pWin->pVScroll;
    }
    return NULL;
}


//get ok button rect screen coordsys 
void 
wndGetOkButtonRect(
	HWND hWnd,
	LPRECT lpRect
)
{
	int iOkButtonWidth,iCloseBoxWidth;
	RECT rcCaption;
	wndGetCaptionRect(hWnd,&rcCaption);
	if(!IsOkButton(hWnd)){
		SetRect(lpRect,0,0,0,0);
		return;
	}
	iOkButtonWidth = WIN_OKBUTTON_WIDTH;
	if(IsCloseBox(hWnd)){
		iCloseBoxWidth = WIN_CLOSEBOX_WIDTH;
	}
	else{
		iCloseBoxWidth = 0;
	}
	
	SetRect(lpRect,rcCaption.right - iOkButtonWidth - iCloseBoxWidth, rcCaption.top,
		rcCaption.right - iCloseBoxWidth,rcCaption.bottom);

}


//get Close box rect screen coordsys
void 
wndGetCloseBoxRect(
	HWND hWnd, 
	LPRECT lpRect
)
{
	RECT rcCaption;
	wndGetCaptionRect(hWnd,&rcCaption);
	if(!IsCloseBox(hWnd)){
		SetRect(lpRect,0,0,0,0);
		return;
	}
	SetRect(lpRect,rcCaption.right - WIN_CLOSEBOX_WIDTH,rcCaption.top,
		rcCaption.right,rcCaption.bottom);
}


void 
CloseBoxPenDownProc(
	HWND hWnd
)
{
	PWindowsTree pWin;
	RECT rc;
	pWin = (PWindowsTree)hWnd;
	pWin->dwStyle |= WS_CLOSEBOXPRESSED;
	//invalidte
	wndGetCloseBoxRect(hWnd,&rc);
	scrInvalidateRect(hWnd, &rc, true);
	
}
void 
OkButtonPenDownProc(
	HWND hWnd
)
{
	PWindowsTree pWin;
	RECT rc;
	pWin = (PWindowsTree)hWnd;
	pWin->dwStyle |= WS_OKBUTTONPRESSED;
	//invalidte
	wndGetOkButtonRect(hWnd,&rc);
	scrInvalidateRect(hWnd, &rc, true);	
}



void 
CloseBoxPenUpProc(
	HWND hWnd
)
{
	PWindowsTree pWin;
	RECT rc;
	pWin = (PWindowsTree)hWnd;
	pWin->dwStyle = pWin->dwStyle & ~WS_CLOSEBOXPRESSED;
	//invalidte
	wndGetCloseBoxRect(hWnd,&rc);
	scrInvalidateRect(hWnd, &rc, true);
	//send message
	SendNotifyMessage(hWnd,LMSG_CLOSE,(WPARAM)NULL,(LPARAM)NULL);
	
}
void 
OkButtonPenUpProc(
	HWND hWnd
)
{
	PWindowsTree pWin;
	RECT rc;
	pWin = (PWindowsTree)hWnd;
	pWin->dwStyle = pWin->dwStyle & ~WS_OKBUTTONPRESSED;
	//invalidte
	wndGetOkButtonRect(hWnd,&rc);
	scrInvalidateRect(hWnd, &rc, true);	
	//send message
	SendNotifyMessage(hWnd,LMSG_OK,(WPARAM)NULL,(LPARAM)NULL);
	
}
