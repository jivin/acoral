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
#include "../include/rect.h"
#include "../include/mouse.h"
#include "../include/keyboard.h"
#include "../include/gal.h"
#include "../include/blockheap.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/gmessage.h"
#include "../include/regclass.h"
#include "../include/hdc.h"
#include "../include/hdcbitblt.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/lguiapp.h"
#include "../include/gdi.h"
#include "../include/text.h"
#include "../include/button.h"
#include "../include/stack.h"
#include "../include/scrollbar.h"
#include "../include/lgui.h"
#include "../include/status.h"
#include "../include/idlecontrol.h"
#include "../include/imewin.h"
#include "../include/keyboard_pro.h"
#include "../include/skbwin.h"
#include "../include/pinyin.h"
#include "../include/system.h"
#include "../include/startmenu.h"
#include "../include/lguiapp.h"
#include "../include/app.h"
#include "../include/gtimer.h"

static LRESULT 
WndProc(
	HWND hWnd, 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam
);
static thread_t thread_client;

extern PlGUIAppStat  _lGUI_ServerStat;
extern PWindowsTree _lGUI_pServerWin;
extern PWindowsTree _lGUI_pActiveWin;
extern PWindowsTree _lGUI_pFocus;
extern PlGUIAppStat _lGUI_pAppStat;

extern PWindowsTree _lGUI_pImeWindow;
extern PWindowsTree _lGUI_pSkbWindow;

extern PWindowsTree _lGUI_pStartMenuWindow;

//extern WndCaptureMouse _lGUI_wndCaptureMouse;
extern char** environ;


#define ID_TABCONTROL	100
#define ID_TASKBAR 		101
#define ID_SCROLLBAR	102
#define ID_USER			1024


PDeskIconInfo pDeskIconInfo;

HWND hWndTab = NULL;

PlGUIAppStat 
GetActiveApplication();
//================================================================

int server()
{
	char pString[100];
	HANDLE hWnd;
	WNDCLASSEX wcex;
	MSG msg;
	if(!InitGUIServer())
		return 0;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(200,100,20));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "desktop";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);

	strcpy(pString,pEnvStr);
	strcat(pString,"/pics/sys/bg.bmp");

	hWnd = CreateWindow("desktop", "desktop", WS_DESKTOP ,
			 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, NULL, pString);

	if (!hWnd)  return false;

	ShowWindow(hWnd, true);
	//UpdateWindow(hWnd);
	//ShowWindow((HWND)_lGUI_pSkbWindow,true);

	//ShowWindow((HWND)_lGUI_pImeWindow,true);
	_lGUI_pServerWin=(PWindowsTree)hWnd;
	_lGUI_ServerStat->hand=(IpcHand)hWnd;
	while (GetMessage(&msg,hWnd)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	TerminateGUIServer();

	return 0;
}

static LRESULT 
WndProc(
	HWND hWnd, 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	static int iTabValue;
	FILE_P fd;
	void* pData;
	static HDC hDC,hDCBack;
	static RECT rc;
	static PAINTSTRUCT ps;
	PWindowsTree pWin;
	char* pFILE_PName;
	static HWND hWndTaskBar,hWndScrollBar;
	pWin=(PWindowsTree)hWnd;
	switch(message){
		case LMSG_CREATE:
		{
			PDeskIconClass pClass;

			pFILE_PName=(char*)lParam;
			if(!pFILE_PName)
				return false;
			hDCBack = CreateCompatibleDC(NULL);
			if(!hDCBack)
				return false;

			fd= OPEN(pFILE_PName,RDONLY);
			if(fd<0)
				return false;

			if(!ReadBmpToBuf(fd, SCREEN_WIDTH,SCREEN_HEIGHT, (void*)(hDCBack->pData))){
				free(pData);
				CLOSE(fd);
				return false;
			}
			CLOSE(fd);
			pWin->dwAddData = (DWORD)hDCBack;
			//create tab control(hWndTab is a global variable)
			hWndTab=CreateWindow("tabcontrol","tab", WS_CONTROL | WS_VISIBLE,
			0,0,SCREEN_WIDTH,DESK_TAB_HEIGHT,hWnd,(HMENU)ID_TABCONTROL,NULL,NULL);

			GetDeskIconInfo();
			//add item to the tab control			
			pClass = pDeskIconInfo->pHead;
			while(pClass){
				SendMessage(hWndTab,TM_ADDITEM,
					(WPARAM)(pClass->lpszClassName),(LPARAM)(pClass->lpszIconName));
				pClass = pClass->pNext;
			}

			SendMessage(hWndTab,TM_SETCURTAB,(WPARAM)0,(LPARAM)NULL);
			iTabValue = 0;
			
			hWndTaskBar = CreateWindow("taskbar","taskbar",WS_CONTROL|WS_VISIBLE,
					0,DESK_TASKBAR_TOP,SCREEN_WIDTH,DESK_TASKBAR_HEIGHT,
					hWnd,(HMENU)ID_TASKBAR,NULL,NULL);

			/*
			iWidth = GetSystemMetrics(SM_CXVSCROLL);
			iHeight = DESK_CLIENT_BOTTOM - DESK_CLIENT_TOP + 1;
			hWndScrollBar = CreateWindow("scrollbar","scrollbar",WS_CONTROL|WS_VSCROLL|WS_VISIBLE,
				DESK_CLIENT_RIGHT-GetSystemMetrics(SM_CXVSCROLL),DESK_CLIENT_TOP+1,
				iWidth,iHeight,
				hWnd,(HMENU)ID_SCROLLBAR,NULL,NULL);


			//setup scrollbar according first class icon 
			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			si.nMax = pDeskIconInfo->pHead->iScrollMax;

			si.nMin = pDeskIconInfo->pHead->iScrollMin;
  			si.nPage = DESKICON_LINES_PER_PAGE;
			si.nPos = 0;
			SetScrollInfo(hWndScrollBar,SB_CTL,&si,false);
			*/
			SetRect(&rc, IMEWIN_LEFT, IMEWIN_TOP, IMEWIN_LEFT + IMEWIN_WIDTH - 1, IMEWIN_TOP + IMEWIN_HEIGHT - 1);
			CreateImeWindow(&rc);

			SetRect(&rc, SKBWIN_LEFT, SKBWIN_TOP, SKBWIN_LEFT + SKBWIN_WIDTH - 1,  SKBWIN_TOP + SKBWIN_HEIGHT - 1);
			CreateSkbWindow(&rc);

			CreateDeskIcon();
			break;
		}
		case LMSG_SHOWWINDOW:
		{
			PDeskIconClass pClass;
			pClass = GetCurrentClass(hWndTab);
			ShowClassDeskIcon(pClass);
			break;
		}
		case LMSG_COMMAND:
			switch(HIWORD(wParam)){
			case BN_CLICKED:
			
				DeskIconClicked(LOWORD(wParam));
				break;
			case TM_TABCHANGED:
			{
				int i,iValue;
				PDeskIconClass pClass;
				
				pClass = pDeskIconInfo->pHead;
				for(i=0;i<iTabValue;i++){
					pClass = pClass->pNext;
				}
				HideClassDeskIcon(pClass);

				iValue = (int)lParam;
				pClass = pDeskIconInfo->pHead;
				for(i=0;i<iValue;i++){
					pClass = pClass->pNext;
				}
				ShowClassDeskIcon(pClass);
				iTabValue = iValue;
				break;
			}
			case LMSG_VSCROLL:
			{
				ProDeskIconScroll(hWndScrollBar,hWndTab);
				break;
			}
			case STARTMENU_ITEM_CLICKED:
				ProcessStartMenuItemClicked(wParam, lParam);
				break;
			case STARTMENU_CLICKED://start button clicked
				ProcessStartMenuClicked();
				break;
			default:
				break;
			}
			break;
		case LMSG_PENDOWN:
			CaptureMouse(hWnd,BYCLIENT);
			break;
		case LMSG_PENMOVE:
			//hDC=GetWindowDC(hWnd);
			//winSetPixel(hDC,(int)wParam,(int)lParam,RGB(0,0,0));
			//ReleaseDC(hWnd,hDC);
			break;
		case LMSG_PENUP:
			DisCaptureMouse();
			break;
		case LMSG_PAINT:
			ps.bPaintDirect=false;

			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			BitBlt(hDC,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,(HDC)(pWin->dwAddData),0,0,SRCCOPY);
			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			DestroyImeWindow();
			DestroySkbWindow();
			DestroyStartMenuControl();
			ReleaseDC((HWND)_lGUI_pWindowsTree,(HDC)(pWin->dwAddData));
			PostQuitMessage(hWnd);
			break;

		default:
			return DefWndProcDesktop(hWnd, message, wParam, lParam);

	}
	return true;
}


LRESULT 
DefWndProcDesktop(
	HWND hWnd, 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	PWindowsTree pWin;
	PWindowsTree pControl;
	PInvalidRect		pInvRect;
	PClipRect		pClipRect;
	HDC hDC;
	RECT rc;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;

	pWin=(PWindowsTree)hWnd;
	hDC=(HDC)wParam;
	if(message&MTYPE_IPC){
		ProcessIpcMessageServer((IpcHand)wParam,message,(void *)lParam);
		return true;
	}

	switch(message){
	case LMSG_NCPAINT:
		break;
	case LMSG_ACTIVEWINDOW:
		if(pWin->pFocus){
			SetFocus(pWin->pFocus);
		}
		else{
			pControl=pWin->pControlTail;
			if(pControl)
				SetFocus(pControl);
		}
		break;
	case LMSG_DISACTIVEWINDOW:
		UnSetFocus((HWND)_lGUI_pFocus);
		UpdateWindow((HWND)_lGUI_pFocus);
		break;
	case LMSG_ERASEBKGND:
		pWndClass=GetRegClass(pWin->lpszClassName);
		if(!pWndClass)
			return (LRESULT)NULL;
		hBrush=pWndClass->hbrBackground;
		crColor=((BRUSH*)hBrush)->crBrushColor;
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
	case LMSG_SRVEXIT:
		DestroyWindow(hWnd);
		break;
/*	case LMSG_IPC_DESTROYAPP:
		DestroyApplicationServer((int)wParam,NULL,0);
		break;*/
	case LMSG_RAW_MOUSEDOWN:
		if(!srvRawMouseDown(wParam,lParam))
			return false;
		break;
	case LMSG_RAW_MOUSEUP:
		if(!srvRawMouseUp(wParam,lParam))
			return false;
		break;
	case LMSG_RAW_MOUSEMOVE:
		if(!srvRawMouseMove(wParam,lParam))
			return false;
		break;
	case LMSG_RAW_KEYDOWN:
		break;
	case LMSG_RAW_KEYUP:
		break;
	case LMSG_RAW_CHAR:
		break;
	}
	return true;
}


BOOL 
srvRawMouseDown(
	WPARAM wParam,
	LPARAM lParam
)
{
	PWNDCLASSEX pWndClass;
	RECT rect;
	int iCounter;
	PWindowsTree pWin;
	POINT point;
	PClipRect pCRect;
	int x,y;
	PlGUIAppStat pStat,pCurStat,pPrevStat,pThisStat,pTopStat,pActiveStat;

	x=(int)wParam;
	y=(int)lParam;
	pPrevStat=_lGUI_pAppStat;
	pStat=_lGUI_pAppStat;
	pTopStat=_lGUI_pAppStat;

	if(IsVisible((HWND)_lGUI_pStartMenuWindow)){
		if(PtInRect(&(_lGUI_pStartMenuWindow->rect),x,y)){
			PostMessage((HWND)_lGUI_pStartMenuWindow,LMSG_PENDOWN,wParam,lParam);
			return true;
		}

		DestroyStartMenuControl();
	}

	if(IsVisible((HWND)_lGUI_pImeWindow)){
		if(PtInRect(&(_lGUI_pImeWindow->rect),x,y)){
			PostMessage((HWND)_lGUI_pImeWindow,LMSG_PENDOWN,wParam,lParam);
			return true;
		}
	}

	//if _lGUI_pSkbWindow is not null;
	if(IsVisible((HWND)_lGUI_pSkbWindow)){
		if(PtInRect(&(_lGUI_pSkbWindow->rect),x,y)){
			PostMessage((HWND)_lGUI_pSkbWindow,LMSG_PENDOWN,wParam,lParam);
			return true;
		}
	}

	while(pStat){
		if(PtInRect(&(pStat->rc),x,y)){
			point.x=x;
			point.y=y;
			if(pStat!=_lGUI_pAppStat){
				pPrevStat->pNext=pStat->pNext;
				pStat->pNext=_lGUI_pAppStat;
				_lGUI_pAppStat=pStat;

				pCurStat=_lGUI_pAppStat->pNext;
				pThisStat=pPrevStat->pNext;

				while(pCurStat!=pThisStat){
					if(pCurStat->bVisible)
						SubtractClipRegion (pCurStat->pClipRgn, &(_lGUI_pAppStat->rc));
					SendClipRgnChangeMsg(pCurStat);
					pCurStat=pCurStat->pNext;
				}
				SendMsgByServer(pTopStat,LMSG_IPC_DISACTIVEAPP,NULL,0);

				SetInitRectClipRegion (_lGUI_pAppStat->pClipRgn, &(_lGUI_pAppStat->rc));
				if(IsVisible((HWND)_lGUI_pImeWindow))
					SubtractClipRegion(_lGUI_pAppStat->pClipRgn,&_lGUI_pImeWindow->rect);
				if(IsVisible((HWND)_lGUI_pSkbWindow))
					SubtractClipRegion(_lGUI_pAppStat->pClipRgn,&_lGUI_pSkbWindow->rect);


				SendClipRgnChangeMsg(_lGUI_pAppStat);
				//memset(&(_lGUI_pAppStat->invd_rc),0,sizeof(RECT));
				SendMsgByServer(_lGUI_pAppStat,LMSG_IPC_REDRAW,NULL,0);

			}
			
			SendMsgByServer(_lGUI_pAppStat,LMSG_IPC_PENDOWN,(void*)&point,sizeof(POINT));
			DisactiveWindow(_lGUI_pWindowsTree);

			return true;
		}
		pPrevStat=pStat;
		pStat=pStat->pNext;
	}
 	iCounter=1;
	pWin=_lGUI_pWindowsTree->pControlHead;
	while(pWin){
		if(IsVisible((HWND)pWin) && PtInRect(&(pWin->pClipRgn->rcBound),x,y)){
			pCRect=pWin->pClipRgn->pHead;
			while(pCRect){
				if(PtInRect(&(pCRect->rect),x,y)){
					pWndClass=GetRegClass(pWin->lpszClassName);

					if(strcmp(pWndClass->lpszClassName,"DESKICON")==0){
						pActiveStat = GetActiveApplication();
						if(pActiveStat){//send disactive message to the application
							SendMsgByServer(pActiveStat,LMSG_IPC_DISACTIVEAPP,NULL,0);
						}
						_lGUI_pWindowsTree->pFocus = pWin;
						//if a window is active, then ActiveWindow function will do nothing.
						//so implement the function like below
						if(!IsActive(_lGUI_pWindowsTree))
							ActiveWindow(_lGUI_pWindowsTree);
						else
							//SetFocus(pWin);
							SetFocusNoPaint(pWin);
					}
					scrGetClientRect(pWin,&rect);
					if(PtInRect(&rect,(int)wParam,(int)lParam)){
						PostMessage((HWND)pWin,LMSG_PENDOWN,wParam,lParam);
					}
					else
						PostMessage((HWND)pWin,LMSG_NCPENDOWN,wParam,lParam);
					return true;
				}
				pCRect=pCRect->pNext;
			}
		}
		pWin=pWin->pNext;
		iCounter++;
	}
	_lGUI_wndCaptureMouse->pWin = _lGUI_pWindowsTree;
	_lGUI_wndCaptureMouse->iByWho = BYCLIENT;
	pActiveStat = GetActiveApplication();
	if(pActiveStat){//send disactive message to the application
		SendMsgByServer(pActiveStat,LMSG_IPC_DISACTIVEAPP,NULL,0);
	}

	ActiveWindow((HWND)_lGUI_pWindowsTree);

	PostMessage((HWND)_lGUI_pWindowsTree,LMSG_PENDOWN,wParam,lParam);

	return true;
}

//Get Current active application
//if no active application,return null;
PlGUIAppStat 
GetActiveApplication()
{
	PlGUIAppStat pActiveStat;
	if(_lGUI_pActiveWin)//if desktop is active,no application is active
		return NULL;
	pActiveStat = _lGUI_pAppStat;
	while(pActiveStat){
		if(pActiveStat->bVisible == true)
			break;
		pActiveStat = pActiveStat->pNext;
	}
	return pActiveStat;
}


BOOL 
srvRawMouseMove(
	WPARAM wParam,
	LPARAM lParam
)
{
	PClipRect pCRect;
	POINT point;
	int x,y;
	PlGUIAppStat pStat;

	x=(int)wParam;
	y=(int)lParam;
	if(_lGUI_wndCaptureMouse->pWin){
		if(PtInRect(&(_lGUI_wndCaptureMouse->pWin->pClipRgn->rcBound),x,y)){
			pCRect=_lGUI_wndCaptureMouse->pWin->pClipRgn->pHead;
			while(pCRect){
				if(PtInRect(&(pCRect->rect),x,y)){
					if(_lGUI_wndCaptureMouse->iByWho == BYCLIENT)
						PostMessage((HWND)(_lGUI_wndCaptureMouse->pWin),LMSG_PENMOVE,wParam,lParam);
					else
						PostMessage((HWND)(_lGUI_wndCaptureMouse->pWin),LMSG_NCPENMOVE,wParam,lParam);
					return true;
				}
				pCRect=pCRect->pNext;
			}
		}
	}
	else{
		pStat=_lGUI_pAppStat;
		if(pStat){
			if(PtInRect(&(pStat->rc),x,y)){
				point.x=x;
				point.y=y;
				SendMsgByServer(pStat,LMSG_IPC_PENMOVE,(void*)&point,sizeof(POINT));
				return true;
			}
		}
	}
	return true;
}


BOOL 
srvRawMouseUp(
	WPARAM wParam,
	LPARAM lParam
)
{
	POINT point;
	int x,y;
	PlGUIAppStat pStat;

	x=(int)wParam;
	y=(int)lParam;
	if(_lGUI_wndCaptureMouse->pWin){
		if(_lGUI_wndCaptureMouse->iByWho == BYCLIENT){
			PostMessage((HWND)_lGUI_wndCaptureMouse->pWin,LMSG_PENUP,wParam,lParam);
		}
		else{
			PostMessage((HWND)_lGUI_wndCaptureMouse->pWin,LMSG_NCPENUP,wParam,lParam);
		}

	}
	else{
		pStat=_lGUI_pAppStat;
		if(pStat){
			if(PtInRect(&(pStat->rc),x,y)){
				point.x = x;
				point.y = y;
				SendMsgByServer(pStat,LMSG_IPC_PENUP,(void*)&point,sizeof(POINT));
				return true;
			}
		}
	}
	return true;
}

extern struct DeskAppClass *DeskClass[];
BOOL 
GetDeskIconInfo()
{
	struct AppCfg **apps;
	struct DeskAppClass **class;
	PDeskIconClass pDeskIconClass;
	PDeskIconNode pDeskIconNode;
	pDeskIconInfo = (PDeskIconInfo)malloc(sizeof(DeskIconInfo));
	if(!pDeskIconInfo){
		return false;
	}
	memset(pDeskIconInfo,0,sizeof(DeskIconInfo));
	class=DeskClass;
	while(*class){
		pDeskIconClass = (PDeskIconClass)malloc(sizeof(DeskIconClass));
		if(!pDeskIconClass)
			return false;
		memset(pDeskIconClass,0,sizeof(DeskIconClass));
		if(!pDeskIconInfo->pHead){
			pDeskIconInfo->pHead = pDeskIconInfo->pTail = pDeskIconClass;
		}
		else{
			pDeskIconInfo->pTail->pNext = pDeskIconClass;
			pDeskIconInfo->pTail = pDeskIconClass;
		}
		//ID
		pDeskIconInfo->pTail->id =(*class)->id ;
		//name
		strcpy(pDeskIconInfo->pTail->lpszClassName,(*class)->Name);
		//icon
		strcpy(pDeskIconInfo->pTail->lpszIconName,pEnvStr);
		strcat(pDeskIconInfo->pTail->lpszIconName,"/pics/sys/");
		strcat(pDeskIconInfo->pTail->lpszIconName,(*class)->IconName);
		apps=(*class)->Apps;
		while(*apps){
				pDeskIconNode = (PDeskIconNode)malloc(sizeof(DeskIconNode));
				if(!pDeskIconNode)
					return false;
				memset(pDeskIconNode,0,sizeof(DeskIconNode));
				pDeskIconNode->entry =(*apps)->entry;
				strcpy(pDeskIconNode->lpszName,(*apps)->Name);
				strcpy(pDeskIconNode->lpszIconName,(*apps)->IconName);
				AddNodeToDeskIconClass(pDeskIconNode,pDeskIconClass);
				apps++;
        }
		pDeskIconInfo->iTotal++;
		class++;
	}
	return true;
}



void 
GetDeskScrollInfo(
	int* iMax, 
	int* iPage, 
	int iTotalIcon
)
{
	*iPage = DESKICON_LINES_PER_PAGE;
	if(!iTotalIcon){
		*iMax = 0;
		return;
	}
	if(iTotalIcon <= DESKICON_NUMBER_PER_LINE){
		*iMax = 1;
		return;
	}
	*iMax = iTotalIcon/DESKICON_NUMBER_PER_LINE;
	if(iTotalIcon%DESKICON_NUMBER_PER_LINE!=0)
		*iMax++;

}


BOOL 
IsDeskIconInfoFILE_P(
	char* pString
)
{
	int iLen;
	iLen = strlen(pString);
	if(iLen<=8)
		return false;

	if(strcmp(pString+iLen-8,".desktop")==0)
		return true;
	else
		return false;
}


void 
DestroyDeskIconInfo()
{
	PDeskIconClass pDeskIconClass,pNextClass ;
	PDeskIconNode pDeskIconNode,pNextNode;
	pDeskIconClass = pDeskIconInfo->pHead;
	while(pDeskIconClass){
		pNextClass = pDeskIconClass->pNext;
		pDeskIconNode = pDeskIconClass->pHead;
		while(pDeskIconNode){
			pNextNode = pDeskIconNode->pNext;
			free(pDeskIconNode);
			pDeskIconNode->pNext;
		}
		free(pDeskIconClass);
		pDeskIconClass = pNextClass;

	}
	free(pDeskIconInfo);
}


BOOL 
AddNodeToDeskIconClass(
	PDeskIconNode pNode,
	PDeskIconClass pClass
)
{
	pNode->pNext = NULL;
	if(!pClass->pHead){
		pClass->pHead = pClass->pTail = pNode;
	}
	else{
		pClass->pTail->pNext = pNode;
		pClass->pTail = pNode;
	}
	pClass->iTotalIcon ++;
	GetDeskScrollInfo(&(pClass->iScrollMax),
		&(pClass->iScrollPage),pClass->iTotalIcon);
	return true;
}


void 
DeleteNodeFromDeskIconClass(
	char* lpszName,
	PDeskIconClass pClass
)
{
	PDeskIconNode pNode,pPrev;

	if(!pClass->pHead)
		return;
	if(pClass->pHead == pClass->pTail){
		if(strcmp(pClass->pHead->lpszName, lpszName)==0){
			free(pClass->pHead);
			pClass->pHead = pClass->pTail = NULL;
			pClass->iTotalIcon --;
			GetDeskScrollInfo(&(pClass->iScrollMax),
				&(pClass->iScrollPage),pClass->iTotalIcon);
		}
		return;
	}
	pNode = pPrev = pClass->pHead;
	while(pNode){
		if(strcmp(pNode->lpszName, lpszName)==0){
			if(pNode == pClass->pHead){
				pClass->pHead = pNode->pNext;
				free(pNode);
			}
			else if(pNode == pClass->pTail){
				pClass->pTail = pPrev;
				free(pNode);
			}
			else{
				pPrev->pNext = pNode->pNext;
				free(pNode);
			}
			pClass->iScrollMax --;
			pClass->iTotalIcon --;
			GetDeskScrollInfo(&(pClass->iScrollMax),
				&(pClass->iScrollPage),pClass->iTotalIcon);
		}
		pPrev = pNode;
		pNode = pNode->pNext;
	}
}

BOOL 
CreateDeskIcon()
{
	PDeskIconClass pClass;
	PDeskIconNode pNode;
	HWND hWnd;
	char pIconPath[MAX_VALUE_SIZE];
	char pBinPath[MAX_VALUE_SIZE];
	BOOL bFirst;
    
	pClass = pDeskIconInfo->pHead;
	while(pClass){
		bFirst = true;
		pNode = pClass->pHead;
		hWnd = NULL;
		while(pNode){
			strcpy(pIconPath,pEnvStr);
			strcat(pIconPath,"/pics/");
			strcat(pIconPath,pNode->lpszIconName);

			strcpy(pBinPath,pEnvStr);
			strcat(pBinPath,"/bin/");
			strcat(pBinPath,pNode->lpszBinName);
			hWnd = CreateWindow("deskicon",pNode->lpszName,WS_CONTROL | WS_TRANS,
				0,0,DESKICON_WIDTH,DESKICON_HEIGHT,
				(HWND)_lGUI_pWindowsTree,(HMENU)(GenDeskIconID()),
				(HANDLE)pIconPath,
				(LPVOID)pNode->entry);
				
			if(bFirst){
				pClass->hWndTail = hWnd;
				bFirst = false;
			}
			pNode = pNode->pNext;
		}
		pClass->hWndHead = hWnd;
		pClass = pClass->pNext;
	}
	return  true;
}

	
void 
ProDeskIconScroll(
	HWND hWndScrollBar,
	HWND hWndTab
)
{

	int iTabValue,i;
	PDeskIconClass pClass;
	LPSCROLLINFO lpsi;
	GetScrollInfo(hWndScrollBar,lpsi);

	iTabValue = SendMessage(hWndTab,TM_GETCURTAB,(WPARAM)NULL,(LPARAM)NULL);
	pClass = pDeskIconInfo->pHead;
	for(i=0;i<iTabValue;i++){
		pClass = pClass ->pNext;
	}
	HideClassDeskIcon(pClass);
	pClass->iScrollPos = lpsi->nPos;
	ShowClassDeskIcon(pClass);
}


void 
HideClassDeskIcon(
	PDeskIconClass pClass
)
{
	PWindowsTree pWinHead,pWinTail,pWin;
	pWinHead = (PWindowsTree)(pClass->hWndHead);
	pWinTail = (PWindowsTree)(pClass->hWndTail);
	
	if(!pWinHead)
		return;
	pWin = pWinHead;
	while(1){
		if(IsVisible((HWND)pWin)){
			ShowWindow((HWND)pWin,false);
			MoveWindow((HWND)pWin,(-1)*pWin->rect.left,(-1)*pWin->rect.top);
		}
		if(pWin == pWinTail)
			break;
		pWin = pWin->pNext;
	}
}


void 
ShowClassDeskIcon(
	PDeskIconClass pClass
)
{
	int iPos;
	
	PWindowsTree pWin,pWinHead,pWinTail;
	int i,x,y;
	
	int iWidth,iHeight,iXInterval,iYInterval,xPos,yPos;

	iPos = pClass->iScrollPos;

	pWinHead = (PWindowsTree)(pClass->hWndHead);
	pWinTail = (PWindowsTree)(pClass->hWndTail);
	if(!pWinHead)
		return;
	pWin = pWinHead;
	for(i=0;i<iPos*3;i++){
		pWin = pWin->pNext;
	}

	iWidth = SCREEN_WIDTH - GetSystemMetrics(SM_CXVSCROLL);
	iHeight = DESK_CLIENT_HEIGHT;
	iXInterval = (iWidth - DESKICON_NUMBER_PER_LINE*DESKICON_WIDTH)/
		(DESKICON_NUMBER_PER_LINE + 1);
	if(iXInterval < 0)
		return;
	iYInterval = (iHeight - DESKICON_LINES_PER_PAGE*DESKICON_HEIGHT)/
		(DESKICON_LINES_PER_PAGE + 1);
	if(iYInterval < 0)
		return;
	pWin = pWinHead;
	yPos = DESK_TAB_HEIGHT + iYInterval;
	for(y=0;y<DESKICON_LINES_PER_PAGE;y++){
		xPos = iXInterval;
		for(x=0;x<DESKICON_NUMBER_PER_LINE;x++){
			MoveWindow((HWND)pWin,xPos,yPos);
			ShowWindow((HWND)pWin,true);
			if(pWin == pWinTail)
				return;
			pWin = pWin->pNext;			
			xPos += (iXInterval + DESKICON_WIDTH);
		}
		yPos += (iYInterval + DESKICON_HEIGHT);
	}
}


PDeskIconClass 
GetCurrentClass(
	HWND hWndTab
)
{
	int i,iValue;
	PDeskIconClass pClass;
	iValue = SendMessage(hWndTab,TM_GETCURTAB,(WPARAM)NULL,(LPARAM)NULL);
	pClass = pDeskIconInfo->pHead;
	for(i=0;i<iValue;i++){
		pClass = pClass->pNext;
	}
	return pClass;
}


void 
DeskIconClicked(
	WORD id
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)(GetDeskIconHandle(id));

	//strcpy(pFILE_PSpec,(char*)(pWin->dwAddData));
	LaunchApp((void (*)(void *args))(pWin->dwAddData));

}


int 
GenDeskIconID()
{
	PWindowsTree pWin;
	int iMax=0;
	pWin = _lGUI_pWindowsTree->pControlHead;
	while(pWin){
		if((int)(pWin->hMenu ) > iMax)
			iMax = (int)(pWin->hMenu);
		pWin = pWin->pNext;
	}
	iMax ++;	
	return iMax;
}


HWND 
GetDeskIconHandle(
	int id
)
{
	PWindowsTree pWin;
	pWin = _lGUI_pWindowsTree->pControlHead;
	while(pWin){
		if((int)(pWin->hMenu)==id)
			return (HWND)pWin;
		pWin = pWin->pNext;
	}
	return NULL;
}


BOOL 
LaunchApp(
	void(*entry)(void *args) 
)
{
/*	pid_t child;
	char* args[]={NULL};
	if((child=fork())==-1){
		printerror("fork error");
		exit(EXIT_FAILURE);
	}
	else if(child==0){
		execve(pFILE_PName,args,environ);
	}*/
	pthread_create(&thread_client,NULL,
				entry,NULL);

	DisactiveWindow(_lGUI_pWindowsTree);
	return true;
}


void 
ProcessStartMenuClicked()
{
	RECT rc;
	SetRect(&rc,1, 0, DEF_MENU_WIDTH,SCREEN_HEIGHT - DESK_TASKBAR_HEIGHT);
	CreateStartMenuControl(&rc);
	AddStartMenuItems();
	ShowStartMenuControl();

}


void 
ProcessStartMenuItemClicked(
	WPARAM wParam, 
	LPARAM lParam
)
{
	PDeskIconClass pClass;

	int iMenuId;
	int iCounter = 0;
	iMenuId = LOWORD((DWORD)wParam);
	pClass = pDeskIconInfo->pHead;

	while(pClass){
		if(pClass->id == iMenuId)
			break;
		iCounter ++;
		pClass = pClass->pNext;
	}
	if(!pClass)
		return;
	SendMessage(hWndTab,TM_SETCURTAB,(WPARAM)iCounter,(LPARAM)NULL);

	
	DestroyStartMenuControl();
	
}


BOOL 
AddStartMenuItems()
{
	HBITMAP hBitmap;
	MENUITEM item;
	PDeskIconClass pClass;
	pClass = pDeskIconInfo->pHead;
	if(!pClass)
		return false;
	while(pClass){
		item.id = pClass->id;
		strcpy(item.lpszName,pClass->lpszClassName);
		hBitmap = CreateBitmap(pClass->lpszIconName);
		item.pBitmap = (PBITMAP)hBitmap;
		SendMessage((HWND)_lGUI_pStartMenuWindow,SM_ADDITEM,(WPARAM)&item,(LPARAM)NULL);
		DeleteObject(hBitmap);
		pClass = pClass->pNext;
	}
	return  true;
}
