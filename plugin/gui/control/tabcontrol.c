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
#include "../include/mledit.h"
#include "../include/scrollbar.h"
#include "../include/text.h"
#include "../include/tabcontrol.h"

static void 
ClearTabControlData(
	HWND hWnd
);
 

BOOL 
RegisterTabControl()
{ 
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)TabControlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "tabcontrol";
	wcex.hIconSm		= 0;
	
	RegisterClass(&wcex);			
	return true;
}


LRESULT 
TabControlProc(
	HWND hWnd,
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	PWindowsTree pWin;
	RECT rc,rcWin;
	HDC hDC;
	
	PAINTSTRUCT ps;
	int iWidth,iHeight;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	PTabControl pTabControl;

	pWin=(PWindowsTree)hWnd;
	pTabControl = (PTabControl)(pWin->dwAddData);
	
	switch(iMsg)
	{
		case LMSG_CREATE:
			pTabControl = malloc(sizeof(TabControl));
			if(!pTabControl)
				return false;
			memset(pTabControl,0,sizeof(TabControl));
			pWin->dwAddData = (DWORD)pTabControl;

			break;
		case LMSG_COMMAND:
			break;
		case LMSG_PENDOWN:
		{
			int x,y;
			int iCurItem,iTotalItem;
			int iCurPosLeft,iCurPosRight;
			int i,iFlag=0;
			x = (int)wParam;
			y = (int)lParam;
			ScreenToClient(hWnd,&x,&y);
			
			GetClientRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			//get curleft and curright according iCurItem
			iCurItem = pTabControl->iCurItem;
			iTotalItem = pTabControl->iTotalItem;
			
			for(i=0;i<iTotalItem;i++){
				if(i==iCurItem){
					iCurPosLeft = rc.left + i*ITEM_HEADER_WIDTH;
					iCurPosRight= rc.right - (iTotalItem-i-1)*ITEM_HEADER_WIDTH;
					iFlag = 1;
				}	
				else{
					if(iFlag){
						iCurPosLeft = rc.right - (iTotalItem-i)*ITEM_HEADER_WIDTH+1;
						iCurPosRight = rc.right - (iTotalItem-i-1)*ITEM_HEADER_WIDTH-1;
					}
					else{
						iCurPosLeft = rc.left + i*ITEM_HEADER_WIDTH+1;
						iCurPosRight = rc.left + (i+1)*ITEM_HEADER_WIDTH-1;
					}
				}
				if((x>=iCurPosLeft)&&(x<=iCurPosRight)){
					SendMessage(hWnd,TM_SETCURTAB,(WPARAM)i,(LPARAM)NULL);
					InvalidateRect(hWnd,NULL,true);
					break;
				}
			}	
			CaptureMouse(hWnd,BYCLIENT);	 			
			break;
		}		
		case LMSG_PENMOVE:
			break;
		case TM_SETCURTAB:
		{
			int i,iValue;
			PTabControlItem pItem;
			unsigned long iRetMsg;

			iValue = (int)wParam;
			if(iValue<0 || iValue>pTabControl->iTotalItem-1)
				return false;
			if(pTabControl->iCurItem == iValue)
				return true;
			pTabControl->iCurItem = iValue;
			//CHANGE pCurItem pointer
			pItem = pTabControl->pItemHead;
			for(i=0;i<pTabControl->iTotalItem;i++){
				if(i == iValue)
					break;
				pItem = pItem->pNext;
			}
			
			if(pTabControl->pCurItem && pTabControl->pCurItem!=pItem){
				iRetMsg=TM_TABCHANGED;
				iRetMsg=iRetMsg<<16;
				iRetMsg=iRetMsg | (unsigned long)(pWin->hMenu);

				SendNotifyMessage((HWND)(pWin->pParent), LMSG_COMMAND,(WPARAM)iRetMsg,(LPARAM)iValue);
			}
			
			pTabControl->pCurItem = pItem;
			break;
		}
		case TM_GETCURTAB:
		{
			return ((PTabControl)(pWin->dwAddData))->iCurItem;
			break;
		}
		case TM_ADDITEM:
		{
			char* strName;
			char pBmpPath[256];
			PTabControlItem pItem,pCurItem;
		
			strName = (char*)wParam;

			if(!pTabControl)
				return false;
			pItem = (PTabControlItem)malloc(sizeof(TabControlItem));
			memset(pItem,0,sizeof(TabControlItem));
			if(!pItem)
				return false;
			if(strName){
				//pItem->lpszCaption = (char*)malloc(strlen(strName));
				pItem->lpszCaption = (char*)malloc(strlen(strName)+1);//modify,strlen+1
				if(!(pItem->lpszCaption))
					return false;
				strcpy(pItem->lpszCaption,strName);
				//memcpy(pItem->lpszCaption,strName,strlen(strName));
			}
			if((char*)lParam){
				strcpy(pBmpPath,(char*)lParam);
				pItem->pBitmap = (PBITMAP)CreateBitmap(pBmpPath);
			}
			
			//add to tail
			if(!pTabControl->pItemHead)
				pTabControl->pItemHead = pItem;
			else{
				pCurItem = pTabControl->pItemHead;
				while(pCurItem->pNext){
					pCurItem = pCurItem->pNext;
				}
				pCurItem->pNext = pItem;
			}
		

			pTabControl->iTotalItem++;
			if(pTabControl->iTotalItem == 1){
				pTabControl->iCurItem =0;
				pTabControl->pCurItem =pTabControl->pItemHead;
			}
			else{
				pTabControl->iCurItem++;
			}
			
			break;
		}
		case LMSG_PENUP:
			DisCaptureMouse();
			break;
		case LMSG_ERASEBKGND:			
			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;

			((BRUSH*)hBrush)->crBrushColor=RGB_SYSTEMHIBRIGHT;

			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;
			
		case LMSG_PAINT:
		{
			HPEN hPenBlack,hPenGray,hPenBright,hPenDark;
			HFONT hFont;
			POINT point;
			RECT rcText;
			int iCurPosRight = 0;
			int iCurPosLeft = 0;
			int iFlag = 0;

			PTabControlItem pItem;
			int iTotalItem,i;
			pItem = pTabControl->pItemHead;
			iTotalItem = pTabControl->iTotalItem;
			if(!iTotalItem)
				return false;
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;			
			}
			
			GetClientRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			GetWindowRect(hWnd,&rcWin);
			iWidth	=rcWin.right - rcWin.left + 1;
			iHeight	=rcWin.bottom - rcWin.top + 1;
			
			
			hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
			hPenGray	=CreatePen(PS_SOLID,1,RGB_DKGRAY);
			hPenBright	=CreatePen(PS_SOLID,1,RGB_WHITE);
			hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);
			
			//begin draw the control
			for(i=0;i<iTotalItem;i++){
				if(i==pTabControl->iCurItem){
					iCurPosLeft = rc.left + i*ITEM_HEADER_WIDTH;
					iCurPosRight= rc.right - (iTotalItem-i-1)*ITEM_HEADER_WIDTH;
					MoveToEx(hDC,iCurPosLeft,rc.bottom,&point);
					SelectObject(hDC,hPenBright);
					LineTo(hDC,iCurPosLeft,rc.top+CORNER_LENGTH);
					LineTo(hDC,iCurPosLeft+CORNER_LENGTH,rc.top);
					LineTo(hDC,iCurPosRight-CORNER_LENGTH,rc.top);
					SelectObject(hDC,hPenBlack);
					LineTo(hDC,iCurPosRight,rc.top+CORNER_LENGTH);
					LineTo(hDC,iCurPosRight,rc.bottom);
					//DrawText
					hFont = GetStockObject(FONT_12_12);
					SelectObject(hDC,hFont);
					SetRect(&rcText,iCurPosLeft,rc.top,iCurPosRight,rc.bottom);
					DrawText(hDC,pTabControl->pCurItem->lpszCaption,
						strlen(pTabControl->pCurItem->lpszCaption),&rcText,DT_CENTER|DT_VCENTER);
					DeleteObject(hFont);
					iFlag = 1;
				}	
				else{
					if(iFlag){
						iCurPosLeft = rc.right - (iTotalItem-i)*ITEM_HEADER_WIDTH+1;
						iCurPosRight = rc.right - (iTotalItem-i-1)*ITEM_HEADER_WIDTH-1;
					}
					else{
						iCurPosLeft = rc.left + i*ITEM_HEADER_WIDTH+1;
						iCurPosRight = rc.left + (i+1)*ITEM_HEADER_WIDTH-1;
					}
					SelectObject(hDC,hPenBright);
					MoveToEx(hDC,iCurPosLeft,rc.bottom,&point);
					LineTo(hDC,iCurPosLeft,rc.top+2+CORNER_LENGTH);
					LineTo(hDC,iCurPosLeft+CORNER_LENGTH,rc.top+2);
					LineTo(hDC,iCurPosRight-CORNER_LENGTH,rc.top+2);
					SelectObject(hDC,hPenBlack);
					LineTo(hDC,iCurPosRight,rc.top+2+CORNER_LENGTH);
					LineTo(hDC,iCurPosRight,rc.bottom);
					
					//bottom line
					SelectObject(hDC,hPenBlack);
					MoveToEx(hDC,iCurPosLeft-1,rc.bottom,&point);
					LineTo(hDC,iCurPosRight+1,rc.bottom);
				}
				//show bitmap at left 
				OutputBmpObjWithTrColor(hDC,iCurPosLeft+2,rc.bottom-15,
					iWidth,iHeight,(HBITMAP)pItem->pBitmap,RGB(230,230,230));
					
				pItem = pItem->pNext;
			
			}	
			DeleteObject(hPenBlack);
			DeleteObject(hPenGray);
			DeleteObject(hPenBright);
			DeleteObject(hPenDark);

	
			EndPaint(hWnd, &ps);
			break;
		}
		case LMSG_DESTROY:
			ClearTabControlData(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}

static void 
ClearTabControlData(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PTabControl pTabControl;
	PTabControlItem pTabControlItem,pPrev;

	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return;
	pTabControl = (PTabControl)(pWin->dwAddData);
	if(!pTabControl)
		return;
	pTabControlItem = pTabControl->pItemHead;
	pPrev = pTabControlItem;
	while(pPrev){
		pTabControlItem = pTabControlItem->pNext;
		if(!(pPrev->pBitmap->bmBits))
			free(pPrev->pBitmap->bmBits);
		if(!(pPrev->lpszCaption))
			free(pPrev->lpszCaption);
		free(pPrev);
		pPrev = pTabControlItem;
	}
	free(pTabControl);
}



