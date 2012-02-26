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

#ifndef _WINNC_H_
#define _WINNC_H_

#ifdef __cplusplus
extern "C" {
#endif

//scrollbarstatus
#define SS_DISABLED		0x00000001

typedef struct tagSCROLLINFO {// si
	DWORD dwStatus;//  Enalble/Disable
	UINT cbSize;
	UINT fMask;
	int  nMin;
	int  nMax;
	UINT nPage;
	int  nPos;
	int  nTrackPos;
}   SCROLLINFO;
typedef SCROLLINFO *LPSCROLLINFO;

typedef struct tagSCROLLCURSTATE {
	int iSliderBarVal;//left or top value
	int iSliderBarLen;//length(heigh or width)
	int iClickPos;
}	SCROLLCURSTATE;
typedef SCROLLCURSTATE* LPSCROLLCURSTATE;



BOOL 
wndDrawNCArea(
	HWND hWnd, 
	HDC hDC
);

void 
wndDrawVScrollBar(
	LPRECT lpRect,
	HDC hDC,
	DWORD dwStyle,
	LPSCROLLCURSTATE pCurState
);

void 
wndDrawHScrollBar(
	LPRECT lpRect,
	HDC hDC,
	DWORD dwStyle,
	LPSCROLLCURSTATE pCurState
);

void 
wndGetBoundRect(
	HWND hWnd,
	LPRECT lpRectTop, 
	LPRECT lpRectBottom,
	LPRECT lpRectLeft,
	LPRECT lpRectRight
);


void 
wndGetCaptionRect(
	HWND hWnd,
	LPRECT lpRect
);

//vscroll pendown procedure
void 
VScrollBarPenDownProc(
	HWND hWnd,
	int x, 
	int y
);


//hscroll pendown procedure
void 
HScrollBarPenDownProc(
	HWND hWnd,
	int x, 
	int y
);


//vscroll penup procedure
void 
VScrollBarPenUpProc(
	HWND hWnd
);


//hscroll penup procedure
void 
HScrollBarPenUpProc(
	HWND hWnd
);


//vscroll penmove procedure
void 
VScrollBarPenMoveProc(
	HWND hWnd,
	int x, 
	int y
);


//hscroll penmove procedure
void 
HScrollBarPenMoveProc(
	HWND hWnd,
	int x, 
	int y
);


BOOL 
wndDrawNCFrame(
	HWND hWnd,
	HDC hDC
);


BOOL 
PaintCaption(
	HWND hWnd,
	HDC hDC
);


int  GUIAPI 
SetScrollInfo(
	HWND hwnd, 
	int fnBar, 
	LPSCROLLINFO lpsi,
	BOOL fRedraw
);


int  GUIAPI 
SetScrollPos(
	HWND hWnd,
	int nBar,
	int nPos,
	BOOL bRedraw
);


BOOL GUIAPI 
SetScrollRange(
	HWND hWnd,
	int nBar,
	int nMinPos,
	int nMaxPos,
	BOOL bRedraw
);


int  GUIAPI 
GetScrollInfo (
	HWND hWnd, 
	LPSCROLLINFO lpsi
);


BOOL 
wndGetVScrollBarRect(
	HWND hWnd,
	LPRECT lpRect
);


BOOL 
wndGetHScrollBarRect(
	HWND hWnd,
	LPRECT lpRect
);


BOOL GUIAPI 
EnableScrollBar(
	HWND hWnd, 
	int iSBar, 
	BOOL bEnable
);


BOOL 
IsVScrollBar(
	HWND hWnd
);


BOOL 
IsHScrollBar(
	HWND hWnd
);


void 
wndGetOkButtonRect(
	HWND hWnd,
	LPRECT lpRect
);


void 
wndGetCloseBoxRect(
	HWND hWnd, 
	LPRECT lpRect
);


void 
CloseBoxPenDownProc(
	HWND hWnd
);


void 
OkButtonPenDownProc(
	HWND hWnd
);


void 
CloseBoxPenUpProc(
	HWND hWnd
);


void 
OkButtonPenUpProc(
	HWND hWnd
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif


