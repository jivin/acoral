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
#include "../include/text.h"
#include "../include/system.h"
#include "../include/scrollbar.h"
#include "../include/gtimer.h"


BOOL 
RegisterScrollBarControl()
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)ScrollBarCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "scrollbar";
	wcex.hIconSm		= 0;
	RegisterClass(&wcex);
	return true;
}


LRESULT 
ScrollBarCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	PWNDCLASSEX pWndClass;
	PWindowsTree pWin;
	LRESULT res;
	COLORREF crColor;
	HDC hDC;

	PAINTSTRUCT ps;
	HBRUSH hBrush;

	pWin=(PWindowsTree)hWnd;
	switch(iMsg)
	{
		case LMSG_CREATE:
			break;
		case LMSG_COMMAND:
			break;

		case LMSG_PENDOWN:
			break;
		case LMSG_PENUP:
			break;
		case LMSG_PENMOVE:
		 	break;
		case LMSG_ERASEBKGND:
			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;

			((BRUSH*)hBrush)->crBrushColor=RGB(93,158,255);

			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;
		case LMSG_ENABLE:
			if((BOOL)wParam)
				pWin->dwStyle &=~WS_DISABLE;
			else
				pWin->dwStyle |=WS_DISABLE;
			break;
		case LMSG_PAINT:
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}


			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			break;
		case LMSG_VSCROLL:
			SendNotifyMessage((HWND)(pWin->pParent), LMSG_VSCROLL,wParam,lParam);
			break;
		case LMSG_HSCROLL:
			SendNotifyMessage((HWND)(pWin->pParent), LMSG_VSCROLL,wParam,lParam);
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}


