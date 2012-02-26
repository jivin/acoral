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
#include "../include/regclass.h"
#include "../include/hdc.h"
#include "../include/startmenu.h"
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

extern int Gal_iBytesDataType;

static BOOL 
mnuAddItem(
	HWND hWnd,
	PMENUDATA pData, 
	PMENUITEM pMenuItem
);

static void 
mnuGetItemsRect(
	PMENUDATA pData,
	int pos, 
	RECT* prc
);


static void 
DestroyStartMenuItems(
	HWND hWnd
);


extern PlGUIAppStat _lGUI_pAppStat;
extern PWindowsTree _lGUI_pActiveWindow;



PWindowsTree _lGUI_pStartMenuWindow = NULL;


void 
CreateStartMenuControl(
	LPRECT lpRect
)
{
	if(_lGUI_pStartMenuWindow)
		DestroyStartMenuControl();

	CreateWindow("StartMenuControl",NULL,WS_MENUWIN|WS_VISIBLE|WS_BORDER,lpRect->left,lpRect->top,
		(lpRect->right - lpRect->left + 1),(lpRect->bottom - lpRect->top + 1),
		_lGUI_pWindowsTree,NULL,NULL,NULL);

}


void 
DestroyStartMenuControl()
{
	DestroyWindow((HWND)_lGUI_pStartMenuWindow);
	_lGUI_pStartMenuWindow = NULL;
}



void 
ShowStartMenuControl()
{
	ShowWindow((HWND)_lGUI_pStartMenuWindow,true);
}


void 
HideStartMenuControl()
{
	PMENUDATA pData;
	ShowWindow((HWND)_lGUI_pStartMenuWindow,false);
	pData = (PMENUDATA)(_lGUI_pStartMenuWindow->dwData);
	pData->itemHilighted = -1;

}


BOOL 
RegisterStartMenuControl()
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)StartMenuCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "startmenucontrol";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}


    PMENUDATA pData;
LRESULT 
StartMenuCtrlProc(
	HWND hWnd,
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	RECT rc,rcItem,rcWin;
	HDC hDC;

	PAINTSTRUCT ps;
	int iWidth,iHeight;

	PWindowsTree pWin;
	int iCounter,iTotal;
	int iRetMsg;

    
	int x,y;

	pWin=(PWindowsTree)hWnd;
	switch(iMsg)
	{
		case LMSG_CREATE:
			pData = (PMENUDATA)malloc(sizeof(MENUDATA));
			if(!pData)
				return false;
			memset(pData,0, sizeof(MENUDATA));
			pWin->dwData = (DWORD)pData;
			pData->itemHeight = DEF_MENU_ITEM_HEIGHT;//default item height
			pData->itemHilighted = -1;
			pWin->dwData = (DWORD)pData;
			break;
		case LMSG_PENDOWN:
			CaptureMouse(hWnd,BYCLIENT);

			scrGetClientRect(hWnd,&rc);
			ScreenToClientRect(hWnd,&rc);

			x = (int)wParam;
			y = (int)lParam;

			ScreenToClient(hWnd,&x,&y);

			pData = (PMENUDATA)(pWin->dwData);
			iTotal =pData->itemCount;
			for(iCounter=0;iCounter<iTotal;iCounter++){
				mnuGetItemsRect(pData,iCounter,&rcItem);
				rcItem.left=rc.left;
				rcItem.right =rc.right;
				if(PtInRect(&rcItem,x,y)){
					pData->itemHilighted = iCounter;
					InvalidateRect(hWnd,&rcItem,true);
					break;
				}
			}
			break;
		case LMSG_PENUP:
			DisCaptureMouse();


			scrGetClientRect(hWnd,&rc);
			ScreenToClientRect(hWnd,&rc);

			x = (int)wParam;
			y = (int)lParam;

			ScreenToClient(hWnd,&x,&y);
			pData = (PMENUDATA)(pWin->dwData);
			iTotal =pData->itemCount;
			for(iCounter=0;iCounter<iTotal;iCounter++){
				mnuGetItemsRect(pData,iCounter,&rcItem);
				rcItem.left=rc.left;
				rcItem.right =rc.right;
				if(PtInRect(&rcItem,x,y)){

					iRetMsg=STARTMENU_ITEM_CLICKED;
					iRetMsg=iRetMsg<<16;
					iRetMsg=iRetMsg | (unsigned long)(pData->items[iCounter].id);
					SendNotifyMessage((HWND)(pWin->pParent), LMSG_COMMAND,(WPARAM)iRetMsg,(LPARAM)NULL);
					break;
				}
			}

			break;
		case LMSG_PENMOVE:
			break;
		case LMSG_PAINT:
		{
			HFONT hFont;

			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			scrGetClientRect(hWnd,&rc);
			
			ScreenToClientRect(hWnd,&rc);
			GetWindowRect(hWnd,&rcWin);
			iWidth	=rcWin.right - rcWin.left + 1;
			iHeight	=rcWin.bottom - rcWin.top + 1;

			pData = (PMENUDATA)(pWin->dwData);
			if(!pData)
				return false;
			iTotal =pData->itemCount;
			hFont = GetStockObject(FONT_12_12);			
			SelectObject(hDC,hFont);
			for(iCounter=0;iCounter<iTotal;iCounter++){
				mnuGetItemsRect(pData,iCounter,&rcItem);
				rcItem.left=rc.left;
				rcItem.right =rc.right;

				if(iCounter  == pData->itemHilighted)
					FillRectangle(hDC,rcItem.left,rcItem.top,rcItem.right,rcItem.bottom,RGB_SYSTEMBRIGHT,RGB_SYSTEMBRIGHT);
				OutputBmpObjWithTrColor(hDC,rcItem.left , rcItem.top + 2,
					iWidth,iHeight,(HBITMAP)(pData->items[iCounter].pBitmap),RGB(230,230,230));
				
				rcItem.left +=14;				
				DrawText(hDC,pData->items[iCounter].lpszName,
					strlen(pData->items[iCounter].lpszName),&rcItem,DT_CENTER);
			}
			DeleteObject(hFont);
			EndPaint(hWnd, &ps);
			break;
		}
		case LMSG_DESTROY:
			DestroyStartMenuItems(hWnd);
			break;
		case SM_ADDITEM:	
			pData = (PMENUDATA)(pWin->dwData);
			mnuAddItem(hWnd,pData,(PMENUITEM)wParam);
            return true;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}


static BOOL 
mnuAddItem(
	HWND hWnd,
	PMENUDATA pData, 
	PMENUITEM pMenuItem
)
{
	RECT rc;
	PWindowsTree pWin;
	
	int iBorder;
	iBorder = wndGetBorder((HWND)_lGUI_pStartMenuWindow);
	
	pWin = (PWindowsTree)hWnd;
	
	CopyRect(&rc,&(pWin->rect));
	if(pData->itemCount == DEF_MENU_BUFFER_LEN)
		return false;

	memcpy(&(pData->items[pData->itemCount]),pMenuItem,sizeof(MENUITEM));
	
	pData->items[pData->itemCount].pBitmap = (PBITMAP)malloc(sizeof(BITMAP));
	if(pMenuItem->pBitmap)      //modify,add
		memcpy(pData->items[pData->itemCount].pBitmap,pMenuItem->pBitmap,sizeof(BITMAP));	
	(pData->items[pData->itemCount].pBitmap)->bmBits = 
		malloc(pMenuItem->pBitmap->bmWidth * pMenuItem->pBitmap->bmHeight * Gal_iBytesDataType);
	memcpy(pData->items[pData->itemCount].pBitmap->bmBits, pMenuItem->pBitmap->bmBits,
		pMenuItem->pBitmap->bmWidth* pMenuItem->pBitmap->bmHeight * Gal_iBytesDataType);

	pData->itemCount ++;


	SetRect(&_lGUI_pStartMenuWindow->rect,rc.left,
		rc.bottom - pData->itemCount*pData->itemHeight - 2*iBorder ,rc.right,rc.bottom);

	return true;
}


static void 
mnuGetItemsRect (
	PMENUDATA pData,
    int pos, 
	RECT* prc
)
{
	int iBorder;
	iBorder = wndGetBorder((HWND)_lGUI_pStartMenuWindow);
	if(pos<0)
		return;
	prc->top = iBorder + pData->itemHeight * (pData->itemCount - pos - 1);
	prc->bottom = prc->top + pData->itemHeight;
}



void 
DestroyStartMenuItems(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PMENUDATA pData;
	int i;
	pWin = (PWindowsTree)hWnd;
	pData = (PMENUDATA)(pWin->dwData);
	for(i=0;i<pData->itemCount;i++){
		if(pData->items[i].pBitmap){
			if(pData->items[i].pBitmap->bmBits)
				free(pData->items[i].pBitmap->bmBits);
			free(pData->items[i].pBitmap);
		}
	}
}
