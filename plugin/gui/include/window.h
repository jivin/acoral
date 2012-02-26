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

#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "lguiapp.h"
#include "gmessage.h"
#ifdef __cplusplus
extern "C" {
#endif
//  |----1---||----2---||----3---||----4---||---5----||---6----||---7----||---8----|
//	|.....controltype..||con  sta||.wintype||....winstyle......||...winstatus......|

// Window type
#define WS_TYPEMASK			0x000f0000L
#define WS_DESKTOP				0x00010000L
#define WS_MAIN					0x00020000L
#define WS_CHILD           			0x00030000L
#define WS_CONTROL          		0x00040000L
#define WS_MENUWIN          		0x00050000L
#define WS_IMEWIN           		0x00060000L
#define WS_SKBWIN           		0x00070000L
//window style
#define WS_STYLEMASK        		0x0000ff00L
#define WS_BORDER				0x00000100L
#define WS_CAPTION				0x00000200L
#define WS_CLOSEBOX			0x00000400L
#define WS_OVERLAPPEDWINDOW	0x00000700L
#define WS_VSCROLL				0x00000800L
#define WS_HSCROLL				0x00001000L
#define WS_THINBORDER			0x00002000L
#define WS_THICKBORDER			0x00004000L
#define WS_OKBUTTON			0x00008000L


//window status
#define WS_STATUSMASK			0x000000ffL
#define WS_VISIBLE          			0x00000001L
#define WS_FOCUS	        		0x00000002L
#define WS_DISABLE				0x00000004L
#define WS_ACTIVE	        		0x00000008L
#define WS_TRANS				0x00000010L

#define WS_OKBUTTONPRESSED	0x00000020L
#define WS_CLOSEBOXPRESSED	0x00000040L

typedef struct tagWindowsTree{
	char						lpszClassName[MAXLEN_CLASSNAME];
	char						lpszCaption[MAXLEN_WINCAPTION];	

	DWORD					dwData;			
	DWORD					dwAddData;		

	RECT					rect;			

	HMENU					hMenu;			

	DWORD					dwStyle;		
	int						iZOrder;		
	PMsgQueue				pMsgQueue;		

	PClipRegion				pClipRgn;		
	PClipRegion				pBackClipRgn;	

	PInvalidRegion			pInvRgn;		


	thread_t					threadid;
	
	LPSCROLLINFO			pHScroll;
	LPSCROLLINFO			pVScroll;
	LPSCROLLCURSTATE		pHCurState;
	LPSCROLLCURSTATE		pVCurState;
       
	PCARETINFO				pCaretInfo;

	struct tagWindowsTree*	pFocus;

	struct tagWindowsTree*	pParent;

	struct tagWindowsTree*	pControlHead;
	struct tagWindowsTree*	pControlTail;

	struct tagWindowsTree*	pChildHead;	
	struct tagWindowsTree*	pChildTail;	

	struct tagWindowsTree*	pNext;		
	struct tagWindowsTree*	pPrev;		
} WindowsTree;

typedef WindowsTree* PWindowsTree;
typedef struct{
	PWindowsTree curWin;
	PlGUIAppStat curStat;
}win_data;

//capture mouse
typedef struct tagWndCaptureMouse{
	PWindowsTree pWin;
	int iByWho;
} WndCaptureMouse;
typedef WndCaptureMouse* PWndCaptureMouse;

//PWindowsTree PortGetCurWin();
extern PWndCaptureMouse _lGUI_wndCaptureMouse;
//PWndCaptureMouse PortGetCapture();


HWND GUIAPI 
CreateWindow(
  char* lpClassName,	// pointer to registered class name
  char* lpWindowName,	// pointer to window name
  DWORD dwStyle,        // window style & window statue
  int x,                // horizontal position of window
  int y,                // vertical position of window
  int nWidth,           // window width
  int nHeight,          // window height
  HWND hWndParent,      // handle to parent or owner window
  HMENU hMenu,          // handle to menu or child-window identifier
  HANDLE hInstance,    	// handle to application instance
  LPVOID lpParam        // pointer to window-creation data
);


HWND 
CreateDesktop(
	const HWND hWnd
);


HWND 
CreateMain(
	const HWND hWnd
);


HWND 
CreateChild(
	const HWND hWnd
);


HWND 
CreateControl(
	const HWND hWnd
);


HWND
CreateMenuWin(
	const HWND hWnd
);


HWND 
CreateImeWin(
	const HWND hWnd
);


HWND 
CreateSkbWin(
	const HWND hWnd
);


void
MessageLoop(
	void* para
);


void GUIAPI 
DestroyWindow(
	HWND hWnd
);


void 
ExitWindow(
	HWND hWnd
);


void 
DestroyControl(
	HWND hWnd
);


void 
DestroyControl_Process(
	HWND hWnd,
	BOOL bByParent
);


void 
DestroyChild(
	HWND hWnd
);


void 
DestroyChild_Process(
	HWND hWnd,
	BOOL bByParent
);


void 
DestroyMain(
	HWND hWnd
);


void 
DestroyDesktop(
	HWND hWnd
);


void 
DestroyMenuWin(
	HWND hWnd
);


void 
DestroyImeWin(
	HWND hWnd
);


void 
DestroySkbWin(
	HWND hWnd
);


BOOL GUIAPI 
ShowWindow(
	HWND hWnd, 
	int nCmdShow
);


BOOL 
ShowDesktop(
	const HWND hWnd,
	int nCmdShow
);


BOOL 
ShowMain(
	const HWND hWnd,
	int nCmdShow
);


BOOL 
ShowChild(
	const HWND hWnd,
	int nCmdShow
);


BOOL 
ShowControl(
	const HWND hWnd,
	int nCmdShow
);


BOOL 
ShowMenuWin(
	const HWND hWnd,
	int nCmdShow
);


BOOL 
ShowImeWin(
	const HWND hWnd,
	int nCmdShow
);


BOOL 
ShowSkbWin(
	const HWND hWnd,
	int nCmdShow
);


BOOL GUIAPI 
UpdateWindow(
	const HWND hWnd
);


void 
UpdateControl(
	const HWND hWnd
);


void 
UpdateChild(
	const HWND hWnd
);


void 
UpdateMain(
	const HWND hWnd
);


void 
UpdateDesktop(
	const HWND hWnd
);


void 
UpdateMenuWin(
	const HWND hWnd
);


void 
UpdateImeWin(
	const HWND hWnd
);


void 
UpdateSkbWin(
	const HWND hWnd
);


BOOL GUIAPI 
MoveWindow(
	HWND hWnd, 
	int x, 
	int y
);


BOOL 
MoveWindowTo(
	HWND hWnd, 
	int x, 
	int y
);


BOOL GUIAPI 
ResizeWindow(
	HWND hWnd,
	LPRECT lpRect
);


BOOL 
MoveMainWindow(
	HWND hWnd, 
	int x, 
	int y
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WINDOW_H_ */
