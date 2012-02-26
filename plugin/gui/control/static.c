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
#include "../include/static.h"
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
#include "../include/scrollbar.h"


BOOL 
RegisterStaticControl()
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)StaticCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "static";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}


LRESULT 
StaticCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	RECT rc;
	HDC hDC,hDCSource;
	int iHeight,iWidth;

	PAINTSTRUCT ps;
	HPEN hPen;
	POINT point;
	int iBorder;

	char* buffer;
	int len;

	PWindowsTree pWin;
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
			if(IsTrans(hWnd)){
				GetWindowRect(hWnd,&rc);
				iWidth = rc.right - rc.left + 1;
				iHeight = rc.bottom - rc.top + 1;
				hDC = (HDC)wParam;
				hDCSource = (HDC)(pWin->pParent->dwAddData);
				if(!hDCSource)
					break;
				BitBlt(hDC,0,0,iWidth,iHeight,hDCSource,rc.left,rc.top,SRCCOPY);
			}
			break;
		case LMSG_PAINT:
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);

			if(IsBorder(hWnd)){
				hPen = CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
				SelectObject(hDC,hPen);
				DeleteObject(hPen);
				winMoveToEx(hDC,rc.left,rc.bottom,&point);
				winLineTo(hDC,rc.left,rc.top);
				winLineTo(hDC,rc.right,rc.top);
				hPen=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
				SelectObject(hDC,hPen);
				DeleteObject(hPen);
				winMoveToEx(hDC,rc.left,rc.bottom,&point);
				winLineTo(hDC,rc.right,rc.bottom);
				winLineTo(hDC,rc.right,rc.top);
			}
			iBorder = wndGetBorder(hWnd);
			DrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),&rc,DT_CENTER);

			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			break;
        case LMSG_GETTEXT:
			buffer = (char*)lParam;

            len = min ((int)wParam, strlen(pWin->lpszCaption));

            memcpy (buffer, pWin->lpszCaption, len);
            buffer [len] = '\0';
            return len;

        case LMSG_SETTEXT:
            len = strlen ((char*)lParam);
            len = min (len, MAXLEN_WINCAPTION);

            memcpy (pWin->lpszCaption, (char*)lParam, len);

            InvalidateRect (hWnd, NULL, TRUE);
			break;

		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}



