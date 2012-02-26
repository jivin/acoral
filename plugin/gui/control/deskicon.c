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
#include "../include/deskicon.h"
#include "../include/regclass.h"
#include "../include/hdc.h"
#include "../include/hdcbitblt.h"
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

  

BOOL 
RegisterDeskIconControl()
{ 
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)DeskIconCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "deskicon";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}


LRESULT 
DeskIconCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	POINT point;
	PWindowsTree pWin;
	RECT rc;
	HDC hDC,hDCSource;

	PAINTSTRUCT ps;
	int iWidth,iHeight;
	HPEN hPen;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	char* pString;
	int iLen;
	pWin=(PWindowsTree)hWnd;

	switch(iMsg)
	{
		case LMSG_CREATE:
			if(wParam){
				iLen=strlen((char*)wParam);
				if(iLen){
					pString=(char*)malloc(iLen + 1);
					strcpy(pString,(char*)wParam);
					pWin->dwData=(DWORD)pString;
				}
			}
			if(lParam){
			/*	iLen=strlen((char*)lParam);
				if(iLen){
					pString=(char*)malloc(iLen + 1);
					strcpy(pString,(char*)lParam);
					pWin->dwAddData=(DWORD)pString;
				}*/
				pWin->dwAddData=(DWORD)lParam;
			}
			break;
		case LMSG_COMMAND:
			break;
		case LMSG_PENDOWN:
			CaptureMouse(hWnd,BYCLIENT);
			if(!IsEnable(hWnd))
				break;
			pWin->dwStyle=pWin->dwStyle | BS_DESKICON_PRESSDOWN;
			winInvalidateRect(hWnd,NULL,true);
			break;
		case LMSG_PENMOVE:
			break;
		case LMSG_PENUP:
			DisCaptureMouse();
			if(!IsEnable(hWnd))
				break;
			pWin->dwStyle=pWin->dwStyle & ~BS_DESKICON_PRESSDOWN;
			winInvalidateRect(hWnd,NULL,true);
			NotifyParent(hWnd, BN_CLICKED);
			break;
		case LMSG_ERASEBKGND:
			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
   			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;
			
			if(IsTrans(hWnd)){
				GetWindowRect(hWnd,&rc);
				iWidth = rc.right - rc.left + 1;
				iHeight = rc.bottom - rc.top + 1;
				hDC = (HDC)wParam;
				hDCSource = (HDC)(pWin->pParent->dwAddData);
				if(!hDCSource)
					break;

				winBitBlt(hDC,0,0,iWidth,iHeight,hDCSource,rc.left,rc.top,SRCCOPY);
			}
			break;
		case LMSG_GETTEXT:
			pString=(char*)lParam;
			if(!pString)
				return 0;
			else{
				iLen=(int)wParam;
				if(!iLen){
					strcpy(pString,pWin->lpszCaption);
					return strlen(pString);
				}
				else{
					strncpy(pString,pWin->lpszCaption,(size_t)iLen);
					return strlen(pString);
				}
			}
			break;
		case LMSG_SETTEXT:
			pString=(char*)lParam;
			strcpy(pWin->lpszCaption,pString);
			if(IsVisible(hWnd))
				winInvalidateRect(hWnd,NULL,true);
			break;

		case LMSG_NCPAINT:
			break;

		case LMSG_PAINT:
		{
			HFONT hFont;
			int iLeft,iTop;
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;
			if(pWin->dwData){
				iLeft = (DESKICON_WIDTH - DESKICON_BMP_WIDTH)/2;
				iTop = 0;
				ShowBitmapWithTrColor(hDC,iLeft,iTop,(char*)(pWin->dwData),RGB_BLACK);
			}
			if(pWin->dwStyle & BS_DESKICON_PRESSDOWN){
				if(IsBorder(hWnd)){
					hPen=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
					SelectObject(hDC,hPen);
					DeleteObject(hPen);
					winMoveToEx(hDC,rc.left,rc.bottom,&point);
					winLineTo(hDC,rc.left,rc.top);
					winLineTo(hDC,rc.right,rc.top);

					hPen=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
					SelectObject(hDC,hPen);
					DeleteObject(hPen);

					winMoveToEx(hDC,rc.right,rc.top,&point);
					winLineTo(hDC,rc.right,rc.bottom);
					winLineTo(hDC,rc.left,rc.bottom);
				}
			}
			else{
				if(IsBorder(hWnd)){
					hPen=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
					SelectObject(hDC,hPen);
					DeleteObject(hPen);
					winMoveToEx(hDC,rc.left,rc.bottom,&point);
					winLineTo(hDC,rc.left,rc.top);
					winLineTo(hDC,rc.right,rc.top);

					hPen=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
					SelectObject(hDC,hPen);
					DeleteObject(hPen);

					winMoveToEx(hDC,rc.right,rc.top,&point);
					winLineTo(hDC,rc.right,rc.bottom);
					winLineTo(hDC,rc.left,rc.bottom);
				}
			}
			//draw text
			SetRect(&rc,0,DESKICON_BMP_HEIGHT,
				DESKICON_WIDTH-1,DESKICON_HEIGHT-1);			
			if(IsFocus(hWnd)){//focus
				FillRectangle(hDC,
					rc.left,rc.top,rc.right,rc.bottom,
					RGB(0,50,200),RGB(0,50,200));
				SetTextColor(hDC,RGB(255,255,255));
			}
			else{
				SetTextBkTrans(hDC);
				SetTextColor(hDC,RGB(0,0,0));
			}
			hFont = GetStockObject(FONT_12_12);
			SelectObject(hDC,hFont);
			DrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),
					&rc,DT_CENTER);
			DeleteObject(hFont);


			EndPaint(hWnd, &ps);
			break;
		}
		case LMSG_DESTROY:
			if(pWin->dwData)
				free((char*)(pWin->dwData));
			if(pWin->dwAddData)
				free((char*)(pWin->dwAddData));
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}


