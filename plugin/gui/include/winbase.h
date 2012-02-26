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

#ifndef _WINBASE_H_
#define _WINBASE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagPAINTSTRUCT {
	BOOL bPaintDirect;
    HDC  hdc;
    BOOL fErase;
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
} PAINTSTRUCT;
typedef PAINTSTRUCT* LPPAINTSTRUCT;

#define BYCLIENT  0
#define BYVSCROLL 1
#define BYHSCROLL 2
#define BYOKBUTTON	3
#define BYCLOSEBOX	4

BOOL 
InitWindowRgn (
	const HWND hWnd
);


void 
RePaintCoveredWindow(
	const HWND hWnd
);


void 
ReCalClipRegion(
	const HWND hWnd
);


void 
ReCalClipRegionControl(
	const HWND hWnd
);


void 
ReCalClipRegionChild(
	const HWND hWnd
);


void 
ReCalClipRegionMain(
	const HWND hWnd
);


void 
ReCalClipRegionDesktop(
	const HWND hWnd
);


void 
ReCalClipRegionApp();


BOOL 
scrInvalidateRect(
	const HWND hWnd, 
	const PRECT lpRect, 
	BOOL bErase
);


BOOL 
winInvalidateRect(
	const HWND hWnd, 
	const PRECT lpRect, 
	BOOL bErase
);


BOOL GUIAPI 
InvalidateRect(
	const HWND hWnd, 
	const PRECT lpRect, 
	BOOL bErase
);

/*inline*/ int 
wndGetBorder(
	HWND hWnd
);



HDC GUIAPI 
BeginPaint(
	const HWND hWnd,
	LPPAINTSTRUCT lpPaint
);


//called by BeginPaint
void /*inline*/ 
EraseRect2Memory(
	HDC hDCMem,
	PRECT pOrgRect,
	PRECT pOutRect,
	COLORREF crColor
);


BOOL GUIAPI 
EndPaint(
	const HWND hWnd,
	LPPAINTSTRUCT lpPaint
);



////====================================================================

void GUIAPI 
ScreenToWindow(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
ScreenToWindowRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
WindowToScreen(
	const HWND hWnd,
	int* px, 
	int* py
);


void GUIAPI 
WindowToScreenRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
WindowToClient(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
WindowToClientRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
ClientToWindow(
	const HWND hWnd, 
	int* px, 
	int* py
);


void GUIAPI 
ClientToWindowRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
ScreenToClient(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
ScreenToClientRect(
	const HWND hWnd,
	LPRECT lprc
);


void GUIAPI 
ClientToScreen(
	const HWND hWnd,
	int* px,
	int* py
);


void GUIAPI 
ClientToScreenRect(
	const HWND hWnd,
	LPRECT lprc
);




int 
GetWinType(
	const HWND hWnd
);


BOOL 
IsHideWindow(
	const HWND hWnd
);


BOOL 
IsDesktop(
	const HWND hWnd
);


BOOL 
IsMainWin(
	const HWND hWnd
);


BOOL 
IsChildWin(
	const HWND hWnd
);


BOOL 
IsControl(
	const HWND hWnd
);


BOOL 
IsEnable(
	const HWND hWnd
);


BOOL 
IsBorder(
	const HWND hWnd
);


BOOL 
IsActive(
	const HWND hWnd
);


BOOL 
IsFocus(
	const HWND hWnd
);


BOOL 
IsVisible(
	const HWND hWnd
);


BOOL 
IsTrans(
	HWND hWnd
);


BOOL 
IsCaption(
	HWND hWnd
);


BOOL 
IsOkButton(
	HWND hWnd
);


BOOL 
IsCloseBox(
	HWND hWnd
);


BOOL 
IsCloseBoxPressed(
	HWND hWnd
);


BOOL 
IsOkButtonPressed(
	HWND hWnd
);


void 
MoveWindowTopMost(
	const HWND hWnd
);


BOOL GUIAPI 
GetWindowRect(
	HWND hWnd,
	LPRECT lpRect
);


BOOL 
scrGetClientRect(
	const HWND hWnd, 
	LPRECT lpRect
);


BOOL GUIAPI 
GetClientRect(
	const HWND hWnd, 
	LPRECT lpRect
);


void GUIAPI 
SetFocus(
	const HWND hWnd
);


void 
UnSetFocus(
	const HWND hWnd
);


void 
SetFocusNoPaint(
	const HWND hWnd
);


void 
UnSetFocusNoPaint(
	const HWND hWnd
);


void 
ActiveApplication();


void 
DisactiveApplication();


void 
DisactiveWindow(
	const HWND hWnd
);


void 
ActiveWindow(
	const HWND hWnd
);


void 
CaptureMouse(
	HWND hWnd,int iByWho
);


void 
DisCaptureMouse();


HWND 
GetCaptureWindow();


LRESULT 
DefWindowProc(
	HWND hWnd, 
	UINT Msg, 
	WPARAM wParam, 
	LPARAM lParam
);


void 
LockMutexForSynchro();


void 
UnLockMutexForSynchro();


DWORD 
GetWindowStyle (
	HWND hWnd
);


BOOL /*inline*/ 
IsServerProcess();


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


