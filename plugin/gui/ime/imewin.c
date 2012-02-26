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
#include "../include/bmp.h"
#include "../include/regclass.h"
#include "../include/hdc.h"
#include "../include/hdcbitblt.h"
#include "../include/imewin.h"
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
#include "../include/text.h"
#include "../include/lguiapp.h"
#include "../include/pinyin.h"
#include "../include/imewin.h"
#include "../include/keyboard_pro.h"
#include "../include/skbwin.h"
#include "../include/taskbar.h"

extern int Gal_iBytesDataType;


extern PWindowsTree _lGUI_pTaskBarWindow;

//[|----4 ----||------4------|][|-----4-----||----4----|][|----4----||----4----|][|----4----||----4----|]
//[|in method-||-L/R pressed-|][|-sdc/dbc  -||----4----|][|----4----||----4----|][|----4----||----4----|]


PWindowsTree _lGUI_pImeWindow;




extern PWindowsTree _lGUI_pSkbWindow;


//create installable input method
//create an ime link table represent ime which currently installed.

ImeDataTable imeDataTable;

static PLGUIIme pImeLinkTable = NULL;


extern LGUIIme pinyinIme;


static BOOL
InstallOneIme(
	PLGUIIme pLGUIIME
);


static BOOL
InstallLGUIIme();


static void
UnInstallLGUIIme();


static PLGUIIme
GetImePointerByIndex(
	int iIndex
);


static int 
GetCurrentIme();


static void 
SetCurrentIme(
	int iIme
);


static BOOL
LGUIInitIme();


static void 
LGUIDestroyIme();


static void 
LGUIResetIme();


static BOOL
LGUICharArrived(
	char ch
);


static void
LGUIFillBackwardSelection();


static void
LGUIFillForwardSelection();


static BOOL
IsChineseIme(
	int iIme
);


static BOOL 
ReadImeBmp(
	HWND hWnd
);


static void 
FreeImeBmp(
	HWND hWnd
);


static void 
RefreshListArea(
	HDC hDC
);


static void 
RefreshInputArea(
	HDC hDC
);


static void 
InvertLeftKey(
	HDC hDC
);


static void 
InvertRightKey(
	HDC hDC
);


static void 
SetLeftKeyPressed(
	HWND hWnd
);


static void 
SetRightKeyPressed(
	HWND hWnd
);


static void 
SetNoKeyPressed(
	HWND hWnd
);


static BOOL 
IsLeftKeyPressed(
	HWND hWnd
);


static BOOL 
IsRightKeyPressed(
	HWND hWnd
);


static int 
IsDbcOrSbc(
	HWND hWnd
);


static char 
Sbc2Dbc(
	char c
);


static void 
RefreshListDisplay(
	HWND hWnd
);


static void 
RefreshInputDisplay(
	HWND hWnd
);


void 
MoveUpImeWindow();


void 
MoveDownImeWindow();


void
ShowImeWindow();


void 
HideImeWindow();



void 
CreateImeWindow(
	LPRECT lpRect
)
{
	if(_lGUI_pImeWindow)
		DestroyImeWindow();

	CreateWindow("imewindow",NULL,WS_IMEWIN | WS_BORDER,lpRect->left,lpRect->top,
		(lpRect->right - lpRect->left + 1),(lpRect->bottom - lpRect->top + 1),
		_lGUI_pWindowsTree,NULL,NULL,NULL);
	//attention imewindow is invisible when create.

}


void 
DestroyImeWindow()
{
	DestroyWindow((HWND)_lGUI_pImeWindow);
}


BOOL RegisterImeWindow()
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)ImeWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "imewindow";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}



LRESULT 
ImeWindowProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	HDC hDC;
	PAINTSTRUCT ps;
	PWindowsTree pWin;
	RECT rcLeftKey,rcRightKey;
	void* pSrcData;
	void* pDestData;
	char c;
	int iIme;
	char pRetChar;
	char pString[4];

	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;


	pWin=(PWindowsTree)hWnd;

	switch(iMsg)
	{

		case LMSG_CREATE:
			if(!ReadImeBmp(hWnd))
				return false;
			InstallLGUIIme();
			break;
		case LMSG_RESETIME:
			LGUIResetIme();
			break;
		case LMSG_ERASEBKGND:

			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;
			((BRUSH*)hBrush)->crBrushColor=RGB_WHITE;
 
			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;
		case LMSG_IME_LEFTKEY_PRESSED:
			LGUIFillBackwardSelection();
			RefreshListDisplay(hWnd);
			break;
		case LMSG_IME_RIGHTKEY_PRESSED:
			LGUIFillForwardSelection();
			RefreshListDisplay(hWnd);
			break;
		case LMSG_IME_LIST_PRESSED:
			break;
		case LMSG_CHAR:
		{
			c = (char)wParam;
			pRetChar='\0';	
			if(IsDbcOrSbc(hWnd) == IME_STATUS_DBC){
				pRetChar = Sbc2Dbc(c);
			}	
			if(pRetChar)
				SendChar2Client(pRetChar);
			else{
				iIme = GetCurrentIme(hWnd);
				if(IsChineseIme(iIme)){
					LGUICharArrived(c);
					RefreshListDisplay(hWnd);
					RefreshInputDisplay(hWnd);
				}
				else{
					pString[0]= c;
					pString[1]='\0';
					if(c == 8)
						SendKeyDown2Client(SCANCODE_BACKSPACE);
					else
						SendString2Client(pString);
					break;
				}
			}

			break;
		}	


		case LMSG_PENDOWN:
			CaptureMouse(hWnd,BYCLIENT);
			SetRect(&rcLeftKey,
				pWin->rect.right - 2 * IMEWIN_BORDER - 2 * IMEWIN_BUTTON_WIDTH,
				pWin->rect.top + IMEWIN_BORDER,
				pWin->rect.right - 2 * IMEWIN_BORDER - IMEWIN_BUTTON_WIDTH,
				pWin->rect.bottom - IMEWIN_BORDER);
			SetRect(&rcRightKey,
				pWin->rect.right - IMEWIN_BORDER - IMEWIN_BUTTON_WIDTH,
				pWin->rect.top + IMEWIN_BORDER,
				pWin->rect.right - IMEWIN_BORDER,
				pWin->rect.bottom - IMEWIN_BORDER);

			if(PtInRect(&rcLeftKey,(int)wParam,(int)lParam)){
				SetLeftKeyPressed(hWnd);
				SendMessage(hWnd,LMSG_IME_LEFTKEY_PRESSED,wParam,lParam);
				scrInvalidateRect(hWnd,&rcLeftKey,true);
			}
			else if(PtInRect(&rcRightKey,(int)wParam,(int)lParam)){
				SetRightKeyPressed(hWnd);
				SendMessage(hWnd,LMSG_IME_RIGHTKEY_PRESSED,wParam,lParam);
				scrInvalidateRect(hWnd,&rcRightKey,true);
			}
			break;
		case LMSG_PENUP:
			if(IsLeftKeyPressed(hWnd)){
				SetRect(&rcLeftKey,
					pWin->rect.right - 2 * IMEWIN_BORDER - 2 * IMEWIN_BUTTON_WIDTH,
					pWin->rect.top + IMEWIN_BORDER,
					pWin->rect.right - 2 * IMEWIN_BORDER - IMEWIN_BUTTON_WIDTH,
					pWin->rect.bottom - IMEWIN_BORDER);
				SetNoKeyPressed(hWnd);
				scrInvalidateRect(hWnd,&rcLeftKey,true);
			}
			else if(IsRightKeyPressed(hWnd)){
				SetRect(&rcRightKey,
					pWin->rect.right - IMEWIN_BORDER - IMEWIN_BUTTON_WIDTH,
					pWin->rect.top + IMEWIN_BORDER,
					pWin->rect.right - IMEWIN_BORDER,
					pWin->rect.bottom - IMEWIN_BORDER);
				SetNoKeyPressed(hWnd);
				scrInvalidateRect(hWnd,&rcRightKey,true);
			}
			DisCaptureMouse();


			break;
		case LMSG_PENMOVE:
			break;
		case LMSG_PAINT:
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			pDestData = hDC->pData;
			pSrcData = (void*)pWin->dwAddData;
			memcpy(pDestData,pSrcData,IMEWIN_WIDTH * IMEWIN_HEIGHT* Gal_iBytesDataType);
			if(IsLeftKeyPressed(hWnd))
				InvertLeftKey(hDC);
			if(IsRightKeyPressed(hWnd))
				InvertRightKey(hDC);

			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			FreeImeBmp(hWnd);
			UnInstallLGUIIme();
			break;
		case LMSG_COMMAND:
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}



//Install a new ime to lgui system
static BOOL
InstallOneIme(
	PLGUIIme pLGUIIME
)
{
	int iSize;
	void* pData;
	PTaskBar pTaskBar;
	PTaskBarItem pItemIme;
	PLGUIIme pIme;
	FILE_P fd;
	char pPathStr[256];
	
	if(!pLGUIIME)
		return false;
	if(!pImeLinkTable){
		pImeLinkTable = pLGUIIME;
	}
	else{
		pIme = pImeLinkTable;
		while(pIme->pNext)
			pIme = pIme->pNext;
		pIme->pNext = pLGUIIME;
	}
	//update taskbar data
	pTaskBar = (PTaskBar)(_lGUI_pTaskBarWindow->dwAddData);
	if(!pTaskBar)
		return false;
	pItemIme = pTaskBar->pItemIme;
	if(!pItemIme)
		return false;
	pItemIme->iTotalPictureNumber += 1;
	//pData;

	iSize = TASKBAR_BUTTONS_HEIGHT * TASKBAR_IMESWITCH_WIDTH * Gal_iBytesDataType; 
	pData = malloc(pItemIme->iTotalPictureNumber * iSize);
	if(!pData)
		return false;
	memcpy(pData, pItemIme->pData, (pItemIme->iTotalPictureNumber - 1) * iSize);
	
	strcpy(pPathStr, pEnvStr);
	strcat(pPathStr, pLGUIIME->pTaskBarIconPath);
	fd = OPEN(pPathStr, RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,
		TASKBAR_IMESWITCH_WIDTH,TASKBAR_BUTTONS_HEIGHT,
		(acoral_u8*)pData + (pItemIme->iTotalPictureNumber - 1) * iSize);
	CLOSE(fd);
	free(pItemIme->pData);
	pItemIme->pData = pData;
	return true;
}




static BOOL
InstallLGUIIme()
{
	if(!InstallOneIme(&pinyinIme))
		return false;
//	if(!InstallOneIme(&wubiIme))
//		return false;
	return true;
}


static void
UnInstallLGUIIme()
{
	PLGUIIme pIme, pNext;
	pIme = pImeLinkTable;
	while(pIme){
		pIme->DestroyIme();
		pNext = pIme->pNext;
		free(pIme);
		pIme = pNext;
	}
}


static PLGUIIme
GetImePointerByIndex(
	int iIndex
)
{
	PLGUIIme pIme;
	pIme = pImeLinkTable;
	while(pIme){
		if(pIme->iIndex == iIndex)
			return pIme;
		pIme = pIme->pNext;
	}
	return NULL;
}


static int 
GetCurrentIme()
{
	return _lGUI_pImeWindow->dwData >>28;
}


static void 
SetCurrentIme(
	int iIme
)
{
	_lGUI_pImeWindow->dwData = _lGUI_pImeWindow->dwData & 0x0fffffff | iIme << 28;

}


//chinese input method operate abstract functioin

static BOOL
LGUIInitIme()
{
	int iIndex;
	PLGUIIme pIme;
	iIndex = GetCurrentIme();
	pIme = GetImePointerByIndex(iIndex);
	if(!pIme)
		return false;
	return pIme->InitIme();
}

static void 
LGUIDestroyIme()
{
	int iIndex;
	PLGUIIme pIme;
	iIndex = GetCurrentIme();
	pIme = GetImePointerByIndex(iIndex);
	if(!pIme)
		return;
	pIme->DestroyIme();
}


static void 
LGUIResetIme()
{
	int iIndex;
	PLGUIIme pIme;
	iIndex = GetCurrentIme();
	pIme = GetImePointerByIndex(iIndex);
	if(!pIme)
		return;
	pIme->ResetIme();
}


static BOOL
LGUICharArrived(
	char ch
)
{
	int iIndex;
	PLGUIIme pIme;
	iIndex = GetCurrentIme();
	pIme = GetImePointerByIndex(iIndex);
	if(!pIme)
		return false;
	return pIme->CharArrived(ch);
}


static void
LGUIFillBackwardSelection()
{
	int iIndex;
	PLGUIIme pIme;
	iIndex = GetCurrentIme();
	pIme = GetImePointerByIndex(iIndex);
	if(!pIme)
		return;
	pIme->FillBackwardSelection();
}


static void
LGUIFillForwardSelection()
{
	int iIndex;
	PLGUIIme pIme;
	iIndex = GetCurrentIme();
	pIme = GetImePointerByIndex(iIndex);
	if(!pIme)
		return;
	pIme->FillForwardSelection();
}




static BOOL
IsChineseIme(
	int iIme
)
{
	if((iIme == IME_STATUS_ENGLISH) ||
		(iIme == IME_STATUS_HANDWRITE))
		return false;
	else
		return true;
}


static BOOL 
ReadImeBmp(
	HWND hWnd
)
{
	PWindowsTree pWin;
	char pString[256];
	char* pData;
	FILE_P fd;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;

	pData = (char*)bmalloc(IMEWIN_WIDTH*IMEWIN_HEIGHT*Gal_iBytesDataType);
	if(!pData)
		return false;
	strcpy(pString,pEnvStr);
	strcat(pString,IMEBMP_BACKGROUND);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,IMEWIN_WIDTH,IMEWIN_HEIGHT,pData);
	CLOSE(fd);

	pWin->dwAddData = (DWORD)pData;
	return true;
}


static void 
FreeImeBmp(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	bfree((void*)(pWin->dwAddData));
}



static void 
RefreshListArea(
	HDC hDC
)
{
	int iLeft, iTop, iWidth, iHeight;

	iLeft = IMEWIN_BORDER;
	iTop = IMEWIN_BORDER;
	iWidth = IMEWIN_LIST_WIDTH;
	iHeight = IMEWIN_HEIGHT - 2*IMEWIN_BORDER;
	FillRectangle(hDC,iLeft,iTop,iWidth-iLeft-1,iHeight-iTop-1,RGB_WHITE,RGB_WHITE);
}


static void 
RefreshInputArea(
	HDC hDC
)
{
	int iLeft, iTop, iWidth, iHeight;

	iLeft = 2 * IMEWIN_BORDER + IMEWIN_LIST_WIDTH ;
	iTop = IMEWIN_BORDER;
	iWidth = IMEWIN_INPUT_WIDTH;
	iHeight = IMEWIN_HEIGHT - 2*IMEWIN_BORDER;

	FillRectangle(hDC,iLeft,iTop,iWidth-iLeft-1,iHeight-iTop-1,RGB_WHITE,RGB_WHITE);
}


static void 
InvertLeftKey(
	HDC hDC
)
{
	char* pData;
	int iLeft, iTop, iWidth, iHeight;
	int x,y;
	pData = (char*)(hDC->pData);
	if(!pData)
		return;
	iLeft = 3 * IMEWIN_BORDER + IMEWIN_LIST_WIDTH + IMEWIN_INPUT_WIDTH;
	iTop = IMEWIN_BORDER;
	iWidth = IMEWIN_BUTTON_WIDTH * Gal_iBytesDataType;
	iHeight = IMEWIN_HEIGHT - 2 * IMEWIN_BORDER;
	pData+= (iTop * IMEWIN_WIDTH + iLeft) * Gal_iBytesDataType;

	for(y=0;y<iHeight;y++){
		for(x=0;x<iWidth;x++){
			*(pData+x) = ~(*(pData+x));
		}
		pData+= IMEWIN_WIDTH * Gal_iBytesDataType;
	}
}


static void 
InvertRightKey(
	HDC hDC
)
{
	char* pData;
	int iLeft, iTop, iWidth, iHeight;
	int x,y;
	pData = (char*)(hDC->pData);
	if(!pData)
		return;
	iLeft = 4 * IMEWIN_BORDER + IMEWIN_LIST_WIDTH + IMEWIN_INPUT_WIDTH + IMEWIN_BUTTON_WIDTH;
	iTop = IMEWIN_BORDER;
	iWidth = IMEWIN_BUTTON_WIDTH * Gal_iBytesDataType;
	iHeight = IMEWIN_HEIGHT - 2 * IMEWIN_BORDER;
	pData+= (iTop * IMEWIN_WIDTH + iLeft) * Gal_iBytesDataType;

	for(y=0;y<iHeight;y++){
		for(x=0;x<iWidth;x++){
			*(pData+x) = ~(*(pData+x));
		}
		pData+= IMEWIN_WIDTH * Gal_iBytesDataType;
	}
}


static void 
SetLeftKeyPressed(
	HWND hWnd
)
{
	PWindowsTree pWin;
	int iValue = 1;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;

	pWin->dwData = pWin->dwData & 0xf0ffffff | iValue << 24;
}


static void 
SetRightKeyPressed(
	HWND hWnd
)
{
	PWindowsTree pWin;
	int iValue = 1;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;

	pWin->dwData = pWin->dwData & 0xf0ffffff | iValue << 25;
}


static void 
SetNoKeyPressed(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;
	pWin->dwData = pWin->dwData & 0xf0ffffff;

}


static BOOL 
IsLeftKeyPressed(
	HWND hWnd
)
{
	int iValue;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;

	iValue = (pWin->dwData >> 24 ) & 0x00000f;


	if(iValue == IME_LEFT_PRESSED)
		return true;
	else
		return false;
}


static BOOL 
IsRightKeyPressed(
	HWND hWnd
)
{
	int iValue;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;

	iValue = (pWin->dwData >> 24 ) & 0x00000f;

	if(iValue == IME_RIGHT_PRESSED)
		return true;
	else
		return false;
}



static int 
IsDbcOrSbc(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;
	return (pWin->dwData & 0x00f00000)>> 20;
}


static char 
Sbc2Dbc(
	char c
)
{
	int i;
	for(i=0;i<SBC2DBC_TABLE_LENGTH;i++){
		if(sbc_dbc_table[i].sbc == c)
			break;
	}
	if(i == SBC2DBC_TABLE_LENGTH)
		return '\0';
	else
		return sbc_dbc_table[i].dbc;
}


static void 
RefreshListDisplay(
	HWND hWnd
)
{
	PWindowsTree pWin;
	RECT rcList;
	HDC hDC,hDCMem;
	HGDIOBJ hFont;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;
	SetRect(&rcList,0,0,IMEWIN_LIST_WIDTH - IMEWIN_BORDER,IMEWIN_HEIGHT - 3 * IMEWIN_BORDER);

	hDC = GetWindowDC(hWnd);
	if(!hDC)
		return;
	hDCMem = CreateCompatibleDC(hDC);
	if(!hDCMem)
		return;
	FillRectangle(hDCMem,
		rcList.left,rcList.top,rcList.right,rcList.bottom,
		RGB_WHITE,RGB_WHITE);
	hFont = GetStockObject(FONT_12_12);
	SelectObject(hDCMem,hFont);
	DeleteObject(hFont);
	DrawText(hDCMem, imeDataTable.pListChars, strlen(imeDataTable.pListChars),
		&rcList, DT_LEFT | DT_VCENTER);
	BitBlt(hDC,
		0,
		0,
		IMEWIN_LIST_WIDTH,
		IMEWIN_HEIGHT - 2*IMEWIN_BORDER,
		hDCMem,
		0,0,
		SRCCOPY);
	ReleaseDC(hWnd,hDCMem);

	ReleaseDC(hWnd,hDC);
}


static void 
RefreshInputDisplay(
	HWND hWnd
)
{
	PWindowsTree pWin;
	RECT rcInput;
	HDC hDC,hDCMem;
	HGDIOBJ hFont;

	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;
	SetRect(&rcInput,
		IMEWIN_BORDER + IMEWIN_LIST_WIDTH,
		0,
		IMEWIN_BORDER + IMEWIN_LIST_WIDTH + IMEWIN_INPUT_WIDTH,
		IMEWIN_HEIGHT - 3 * IMEWIN_BORDER);
	hDC = GetWindowDC(hWnd);
	if(!hDC)
		return;
	hDCMem = CreateCompatibleDC(hDC);
	if(!hDCMem)
		return;
	FillRectangle(hDCMem,
		rcInput.left,rcInput.top,rcInput.right,rcInput.bottom,
		RGB_WHITE,RGB_WHITE);

	hFont = GetStockObject(FONT_12_12);
	SelectObject(hDCMem,hFont);
	DeleteObject(hFont);
	DrawText(hDCMem, imeDataTable.pTotalInputChars, 
		strlen(imeDataTable.pTotalInputChars), &rcInput, DT_LEFT | DT_VCENTER);
	BitBlt(hDC,
		IMEWIN_BORDER + IMEWIN_LIST_WIDTH,
		0,
		IMEWIN_INPUT_WIDTH,
		IMEWIN_HEIGHT - 2*IMEWIN_BORDER,
		hDCMem,
		IMEWIN_BORDER + IMEWIN_LIST_WIDTH,
		0,
		SRCCOPY);
	ReleaseDC(hWnd,hDCMem);

	ReleaseDC(hWnd,hDC);
}






void 
MoveUpImeWindow()
{
	MoveWindowTo((HWND)_lGUI_pImeWindow,IMEWIN_LEFT,IMEWIN_TOP);
}


void 
MoveDownImeWindow()
{
	MoveWindowTo((HWND)_lGUI_pImeWindow,IMEWIN_LEFT,DESK_TASKBAR_TOP - IMEWIN_HEIGHT);

}


void 
ShowImeWindow()
{
	if(IsVisible((HWND)_lGUI_pSkbWindow)){
		MoveUpImeWindow();
	}
	else{
		MoveDownImeWindow();
	}
	if(!IsVisible((HWND)_lGUI_pImeWindow))
		ShowWindow((HWND)_lGUI_pImeWindow,true);
}


void 
HideImeWindow()
{
	if(IsVisible((HWND)_lGUI_pImeWindow))
		ShowWindow((HWND)_lGUI_pImeWindow,false);
}

void 
SwitchIme()
{
	int iCurIme;

	PTaskBar pTaskBar;
	PTaskBarItem pItemIme;
	
	pTaskBar = (PTaskBar)(_lGUI_pTaskBarWindow->dwAddData);
	if(!pTaskBar)
		return;
	pItemIme = pTaskBar->pItemIme;
	if(!pItemIme)
		return;
	iCurIme = GetCurrentIme();
	if(iCurIme == pItemIme->iCurValue)
		return;

	SetCurrentIme(pItemIme->iCurValue);
	
	switch(pItemIme->iCurValue){
	case IME_STATUS_ENGLISH:
		HideImeWindow();
		break;
	case IME_STATUS_HANDWRITE:
		if(!IsVisible((HWND)_lGUI_pImeWindow)){
			if(IsVisible((HWND)_lGUI_pSkbWindow)){
				MoveUpImeWindow();
				ShowImeWindow();
			}
			else{
				MoveDownImeWindow();
				ShowImeWindow();
			}
		}
		break;
	default:		//installed chinese input method including pinyin and wubi etc.
		if(!IsVisible((HWND)_lGUI_pImeWindow)){
			if(IsVisible((HWND)_lGUI_pSkbWindow)){
				MoveUpImeWindow();
				ShowImeWindow();
			}
			else{
				MoveDownImeWindow();
				ShowImeWindow();
			}
		}

		LGUIInitIme();
		break;
		
	}
	
}


void 
SetDbc()
{
	_lGUI_pImeWindow->dwData = _lGUI_pImeWindow->dwData | 0x00100000;
}


void 
SetSbc()
{
	_lGUI_pImeWindow->dwData = _lGUI_pImeWindow->dwData & 0xff0fffff;
}


void 
SwitchDbc(
	BOOL bValue
)
{
	if(bValue)
		SetDbc();
	else
		SetSbc();
}
