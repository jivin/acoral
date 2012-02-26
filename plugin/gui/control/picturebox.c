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
#include "../include/picturebox.h"
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


BOOL 
RegisterPictureBoxControl()
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)PictureBoxCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "picturebox";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}


LRESULT 
PictureBoxCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	PWindowsTree pWin;
	RECT rc;
	HDC hDC;
	PAINTSTRUCT ps;
	int iWidth,iHeight;
	char* pFileName;
	pWin=(PWindowsTree)hWnd;

	switch(iMsg)
	{
		case LMSG_CREATE:

			pFileName = (char*)malloc(strlen((char*)lParam)+1);
			strcpy(pFileName,(char*)lParam);
			pWin->dwAddData = (DWORD)pFileName;
			break;
		case LMSG_COMMAND:
			break;
		case LMSG_PAINT:
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}
			GetWindowRect(hWnd,&rc);

			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;
			if(pWin->dwAddData)
				ShowBitmap(hDC,1,1,(char*)(pWin->dwAddData));
			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			if(pWin->dwAddData)
				free((char*)(pWin->dwAddData));
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}
