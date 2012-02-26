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
#include "../include/button.h"
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


static LRESULT 
PushButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


static LRESULT 
CheckBoxCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


static LRESULT 
AutoCheckBoxCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


static LRESULT 
RadioButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


static LRESULT 
AutoRadioButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


static unsigned int 
GetButtonType(
	HWND hWnd
);


static BOOL 
IsChecked(
	HWND hWnd
);


static BOOL 
IsGroup(
	HWND hWnd
);


BOOL 
RegisterButtonControl()
{ 
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)ButtonCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "button";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}


LRESULT ButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	unsigned int uType;
	uType=GetButtonType(hWnd);
	switch(uType){
	case BS_PUSHBUTTON:
		return PushButtonCtrlProc(hWnd, iMsg, wParam, lParam);
	case BS_CHECKBOX:
		return CheckBoxCtrlProc(hWnd, iMsg, wParam, lParam);
	case BS_AUTOCHECKBOX:
		return AutoCheckBoxCtrlProc(hWnd, iMsg, wParam, lParam);
	case BS_RADIOBUTTON:
		return RadioButtonCtrlProc(hWnd, iMsg, wParam, lParam);
	case BS_AUTORADIOBUTTON:
		return AutoRadioButtonCtrlProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}


static LRESULT 
PushButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	POINT point;
	PWindowsTree pWin;
	RECT rc;
	HDC hDC;

	PAINTSTRUCT ps;
	int iWidth,iHeight;
	HPEN hPen;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	char* pString;
	int iLen;
	pWin=(PWindowsTree)hWnd;

	switch(iMsg)
	{
		case LMSG_CREATE:
			break;
		case LMSG_COMMAND:
			break;
		case LMSG_PENDOWN:
			CaptureMouse(hWnd,BYCLIENT);
			if(!IsEnable(hWnd))
				break;
			pWin->dwStyle=pWin->dwStyle | BS_BUTTON_PRESSDOWN;
			winInvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);

			break;

		case LMSG_PENMOVE:
			break;

		case LMSG_PENUP:
			DisCaptureMouse();
			if(!IsEnable(hWnd))
				break;
			pWin->dwStyle=pWin->dwStyle & ~BS_BUTTON_PRESSDOWN;
			winInvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);
			NotifyParent(hWnd, BN_CLICKED);
			break;
		case LMSG_ERASEBKGND:
			//whether it's a focus control
			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;
			if(IsFocus(hWnd))
				((BRUSH*)hBrush)->crBrushColor=RGB(93,158,255);
			else
				((BRUSH*)hBrush)->crBrushColor=RGB(144,144,144);
			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;
		case LMSG_ENABLE:
			if((BOOL)wParam)///Enable
				pWin->dwStyle &=~WS_DISABLE;
			else//Disable
				pWin->dwStyle |=WS_DISABLE;
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
		{
			//no client area drawing 
			//will replace drawing procedure in defwindowproc
			hDC = (HDC)wParam;
			if(!hDC)
				return false;
			GetWindowRect(hWnd,&rc);

			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;

			if(pWin->dwStyle & BS_BUTTON_PRESSDOWN){
	 			if(IsBorder(hWnd)){
					hPen=CreatePen(PS_SOLID,1,RGB(88,87,81));
					SelectObject(hDC,hPen);
					DeleteObject(hPen);
					winMoveToEx(hDC,rc.left,rc.top,&point);
					winLineTo(hDC,rc.right,rc.top);

					winMoveToEx(hDC,rc.left,rc.top,&point);
					winLineTo(hDC,rc.left,rc.bottom);

					hPen=GetStockObject(LTGRAY_PEN);
					SelectObject(hDC,hPen);
					DeleteObject(hPen);

					winMoveToEx(hDC,rc.right,rc.top+1,&point);
					winLineTo(hDC,rc.right,rc.bottom);
					winMoveToEx(hDC,rc.right,rc.bottom,&point);
					winLineTo(hDC,rc.left,rc.bottom);
				}
			}
			else{
				if(IsBorder(hWnd)){
					hPen=GetStockObject(LTGRAY_PEN);
					SelectObject(hDC,hPen);
					DeleteObject(hPen);
					winMoveToEx(hDC,rc.left,rc.top,&point);
					winLineTo(hDC,rc.right,rc.top);

					winMoveToEx(hDC,rc.left,rc.top,&point);
					winLineTo(hDC,rc.left,rc.bottom);

					hPen=CreatePen(PS_SOLID,1,RGB(88,87,81));
					SelectObject(hDC,hPen);
					DeleteObject(hPen);

					winMoveToEx(hDC,rc.right,rc.top+1,&point);
					winLineTo(hDC,rc.right,rc.bottom);
					winMoveToEx(hDC,rc.right,rc.bottom,&point);
					winLineTo(hDC,rc.left,rc.bottom);
				}
			}			

			
			break;
		}
		case LMSG_PAINT:
			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}

			GetWindowRect(hWnd,&rc);

			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;

			if(pWin->dwStyle & BS_BUTTON_PRESSDOWN){
				SetTextColor(hDC,RGB(255,0,0));
			}
			else{
				if(!IsEnable(hWnd))
					SetTextColor(hDC,RGB(180,180,180));
				else
					SetTextColor(hDC,RGB(0,0,255));
			}
			DrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),&rc,DT_CENTER | DT_VCENTER);
			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}

//CheckBox
static LRESULT 
CheckBoxCtrlProc(
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
	int xCenter,yCenter;
	RECT rcBox;
	HPEN hPen;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	char* pString;
	int iLen;

	HPEN hPenBlack;
	HPEN hPenGray;
	HPEN hPenBright;
	HPEN hPenDark;

	POINT point;

	pWin=(PWindowsTree)hWnd;



	switch(iMsg)
	{
		case LMSG_CREATE:

			break;
		case LMSG_COMMAND:
			break;
		case LMSG_ERASEBKGND:
			//whether it's a focus control

			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;
			if(IsFocus(hWnd))
				((BRUSH*)hBrush)->crBrushColor=RGB(93,158,255);
			else
				((BRUSH*)hBrush)->crBrushColor=RGB(144,144,144);

			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;

		case LMSG_PENDOWN:
			break;
		case LMSG_PENUP:
			if(!IsChecked(hWnd))
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
			winInvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);

			NotifyParent(hWnd, BN_CLICKED);
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
		case BM_SETCHECK:
			if(wParam)
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
			else
				((PWindowsTree)hWnd)->dwStyle &= ~BS_BUTTON_CHECKED;

			winInvalidateRect(hWnd,NULL,true);
			break;
		case BM_GETCHECK:
			if(IsChecked(hWnd))
				return CHECKED;
			else
				return UNCHECKED;
			break;

		case LMSG_PENMOVE:

			break;

		case LMSG_NCPAINT:
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			hDC = (HDC)wParam;
			if(IsBorder(hWnd)){
				hPen=CreatePen(PS_SOLID,1,RGB(88,87,81));
				SelectObject(hDC,hPen);
				winRectangle(hDC,rc.left,rc.top,rc.right,rc.bottom);
				DeleteObject(hPen);
			}			
			break;

		case LMSG_PAINT:
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;
			if(iHeight<13) break;

			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}

			hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
			hPenGray	=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
			hPenBright	=CreatePen(PS_SOLID,1,RGB_SYSTEMBRIGHT);
			hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);



			SetTextColor(hDC,RGB(0,0,0));

			DrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),&rc,DT_RIGHT | DT_VCENTER);
			yCenter=rc.top + (rc.bottom - rc.top)/2;
			xCenter=7;
			SetRect(&rcBox,xCenter - 6,yCenter - 6,xCenter + 6, yCenter + 6);
			SelectObject(hDC,hPenDark);
			winMoveToEx(hDC,rcBox.right-1,rcBox.top,&point);
			winLineTo(hDC,rcBox.left,rcBox.top);
			winLineTo(hDC,rcBox.left,rcBox.bottom-1);

			SelectObject(hDC,hPenBlack);
			winMoveToEx(hDC,rcBox.right-2,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+1);
			winLineTo(hDC,rcBox.left+1,rcBox.bottom-2);

			SelectObject(hDC,hPenGray);
			winMoveToEx(hDC,rcBox.right,rcBox.top,&point);
			winLineTo(hDC,rcBox.right,rcBox.bottom);
			winLineTo(hDC,rcBox.right,rcBox.bottom);

			SelectObject(hDC,hPenBright);
			winMoveToEx(hDC,rcBox.right-1,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.right-1,rcBox.bottom-1);
			winLineTo(hDC,rcBox.left+1,rcBox.bottom-1);

			if(pWin->dwStyle & BS_BUTTON_CHECKED){

				SelectObject(hDC,hPenBlack);
				winMoveToEx(hDC,rcBox.left+3,rcBox.top+5,&point);
				winLineTo(hDC,rcBox.left+3,rcBox.top+7);

				winMoveToEx(hDC,rcBox.left+4,rcBox.top+6,&point);
				winLineTo(hDC,rcBox.left+4,rcBox.top+8);

				winMoveToEx(hDC,rcBox.left+5,rcBox.top+7,&point);
				winLineTo(hDC,rcBox.left+5,rcBox.top+9);

				winMoveToEx(hDC,rcBox.left+6,rcBox.top+6,&point);
				winLineTo(hDC,rcBox.left+6,rcBox.top+8);

				winMoveToEx(hDC,rcBox.left+7,rcBox.top+5,&point);
				winLineTo(hDC,rcBox.left+7,rcBox.top+7);

				winMoveToEx(hDC,rcBox.left+8,rcBox.top+4,&point);
				winLineTo(hDC,rcBox.left+8,rcBox.top+6);

				winMoveToEx(hDC,rcBox.left+9,rcBox.top+3,&point);
				winLineTo(hDC,rcBox.left+9,rcBox.top+5);

			}

			//destroy the hdc object
			DeleteObject(hPenBlack);
			DeleteObject(hPenGray);
			DeleteObject(hPenBright);
			DeleteObject(hPenDark);

			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}

//CheckBox
static LRESULT 
AutoCheckBoxCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	PWindowsTree pWin;
	RECT rc,rcBox;
	HDC hDC;

	PAINTSTRUCT ps;
	int iWidth,iHeight;
	int xCenter,yCenter;
	HPEN hPen;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	char* pString;
	int iLen;

	HPEN hPenBlack;
	HPEN hPenGray;
	HPEN hPenBright;
	HPEN hPenDark;
	POINT point;
	pWin=(PWindowsTree)hWnd;


	switch(iMsg)
	{
		case LMSG_CREATE:

			break;
		case LMSG_COMMAND:
			break;
		case LMSG_ERASEBKGND:
			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;
			if(IsFocus(hWnd))
				((BRUSH*)hBrush)->crBrushColor=RGB(93,158,255);
			else
				((BRUSH*)hBrush)->crBrushColor=RGB(144,144,144);

			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;

		case LMSG_PENDOWN:

			break;
		case LMSG_PENUP:
			if(IsChecked(hWnd))
				((PWindowsTree)hWnd)->dwStyle &= ~BS_BUTTON_CHECKED;
			else
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
			winInvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);

			NotifyParent(hWnd, BN_CLICKED);
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
		case BM_SETCHECK:
			if(wParam)
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
			else
				((PWindowsTree)hWnd)->dwStyle &= ~BS_BUTTON_CHECKED;

			winInvalidateRect(hWnd,NULL,true);
			break;
		case BM_GETCHECK:
			if(IsChecked(hWnd))
				return CHECKED;
			else
				return UNCHECKED;
			break;
		case LMSG_PENMOVE:
			break;
		case LMSG_NCPAINT:
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			if(IsBorder(hWnd)){
				hPen=CreatePen(PS_SOLID,1,RGB(88,87,81));
				SelectObject(hDC,hPen);
				winRectangle(hDC,rc.left,rc.top,rc.right,rc.bottom);
				DeleteObject(hPen);
			}
			
			break;
		case LMSG_PAINT:

			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;
			if(iHeight<13) break;

			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}

			hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
			hPenGray	=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
			hPenBright	=CreatePen(PS_SOLID,1,RGB_SYSTEMBRIGHT);
			hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);


			SetTextColor(hDC,RGB(0,0,0));

			DrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),&rc,DT_RIGHT | DT_VCENTER);
			//CENTER BLANK winRectangle
			yCenter=rc.top + (rc.bottom - rc.top)/2;
			xCenter=7;
			SetRect(&rcBox,xCenter - 6,yCenter - 6,xCenter + 6, yCenter + 6);
			//Draw bound
			//left top
			SelectObject(hDC,hPenDark);
			winMoveToEx(hDC,rcBox.right-1,rcBox.top,&point);
			winLineTo(hDC,rcBox.left,rcBox.top);
			winLineTo(hDC,rcBox.left,rcBox.bottom-1);
			//left top black bound
			SelectObject(hDC,hPenBlack);
			winMoveToEx(hDC,rcBox.right-2,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+1);
			winLineTo(hDC,rcBox.left+1,rcBox.bottom-2);
			//right bottom white bound
			SelectObject(hDC,hPenGray);
			winMoveToEx(hDC,rcBox.right,rcBox.top,&point);
			winLineTo(hDC,rcBox.right,rcBox.bottom);
			winLineTo(hDC,rcBox.right,rcBox.bottom);
			//right bottom bright bound
			SelectObject(hDC,hPenBright);
			winMoveToEx(hDC,rcBox.right-1,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.right-1,rcBox.bottom-1);
			winLineTo(hDC,rcBox.left+1,rcBox.bottom-1);

			if(pWin->dwStyle & BS_BUTTON_CHECKED){
				//Draw Check
				SelectObject(hDC,hPenBlack);
				winMoveToEx(hDC,rcBox.left+3,rcBox.top+5,&point);
				winLineTo(hDC,rcBox.left+3,rcBox.top+7);

				winMoveToEx(hDC,rcBox.left+4,rcBox.top+6,&point);
				winLineTo(hDC,rcBox.left+4,rcBox.top+8);

				winMoveToEx(hDC,rcBox.left+5,rcBox.top+7,&point);
				winLineTo(hDC,rcBox.left+5,rcBox.top+9);

				winMoveToEx(hDC,rcBox.left+6,rcBox.top+6,&point);
				winLineTo(hDC,rcBox.left+6,rcBox.top+8);

				winMoveToEx(hDC,rcBox.left+7,rcBox.top+5,&point);
				winLineTo(hDC,rcBox.left+7,rcBox.top+7);

				winMoveToEx(hDC,rcBox.left+8,rcBox.top+4,&point);
				winLineTo(hDC,rcBox.left+8,rcBox.top+6);

				winMoveToEx(hDC,rcBox.left+9,rcBox.top+3,&point);
				winLineTo(hDC,rcBox.left+9,rcBox.top+5);

			}

			//destroy the hdc object
			DeleteObject(hPenBlack);
			DeleteObject(hPenGray);
			DeleteObject(hPenBright);
			DeleteObject(hPenDark);

			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}



static LRESULT 
RadioButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	PWindowsTree pWin;
	RECT rc;
	RECT rcBox;
	HDC hDC;


	PAINTSTRUCT ps;
	int iWidth,iHeight;
	int xCenter,yCenter;
	HPEN hPen;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	char* pString;
	int iLen;
	HPEN hPenBlack;
	HPEN hPenGray;
	HPEN hPenBright;
	HPEN hPenDark;

	POINT point;
	pWin=(PWindowsTree)hWnd;



	switch(iMsg)
	{
		case LMSG_CREATE:

			break;
		case LMSG_COMMAND:
			break;
		case LMSG_ERASEBKGND:

			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;
			if(IsFocus(hWnd))
				((BRUSH*)hBrush)->crBrushColor=RGB(93,158,255);
			else
				((BRUSH*)hBrush)->crBrushColor=RGB(144,144,144);

			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;

		case LMSG_PENDOWN:

			break;
		case LMSG_PENUP:
			if(!IsChecked(hWnd))
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
			winInvalidateRect(hWnd,NULL,true);
			UpdateWindow(hWnd);

			NotifyParent(hWnd, BN_CLICKED);
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
		case BM_SETCHECK:
			if(wParam)
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
			else
				((PWindowsTree)hWnd)->dwStyle &= ~BS_BUTTON_CHECKED;

			winInvalidateRect(hWnd,NULL,true);
			break;
		case BM_GETCHECK:
			if(IsChecked(hWnd))
				return CHECKED;
			else
				return UNCHECKED;
			break;

		case LMSG_PENMOVE:
			break;
		case LMSG_NCPAINT:
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			if(IsBorder(hWnd)){
				hPen=CreatePen(PS_SOLID,1,RGB(88,87,81));
				SelectObject(hDC,hPen);
				DeleteObject(hPen);
				winRectangle(hDC,rc.left,rc.top,rc.right,rc.bottom);
			}			
			break;
		case LMSG_PAINT:
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;
			if(iHeight<13) break;

			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}


			SetTextColor(hDC,RGB(0,0,0));

			DrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),&rc,DT_RIGHT | DT_VCENTER);
			//CENTER BLANK winRectangle
			yCenter=rc.top + (rc.bottom - rc.top)/2;
			xCenter=7;

			SetRect(&rcBox,xCenter-6,yCenter-6,xCenter+5,yCenter+5);

			hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
			hPenGray	=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
			hPenBright	=CreatePen(PS_SOLID,1,RGB_SYSTEMBRIGHT);
			hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);

			SelectObject(hDC,hPenDark);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+1);
			winMoveToEx(hDC,rcBox.left+1,rcBox.top+3,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+4);
			winMoveToEx(hDC,rcBox.left,rcBox.top+4,&point);
			winLineTo(hDC,rcBox.left,rcBox.top+7);
			winMoveToEx(hDC,rcBox.left+1,rcBox.top+8,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+9);

			SelectObject(hDC,hPenBlack);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top+1);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+2,&point);
			winLineTo(hDC,rcBox.left+3,rcBox.top+2);
			winMoveToEx(hDC,rcBox.left+8,rcBox.top+2,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+2);
			winSetPixel(hDC,rcBox.left+2,rcBox.top+3,RGB_SYSTEMBLACK);
			winMoveToEx(hDC,rcBox.left+1,rcBox.top+4,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+7);
			winSetPixel(hDC,rcBox.left+2,rcBox.top+8,RGB_SYSTEMBLACK);

			SelectObject(hDC,hPenBright);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+9,&point);
			winLineTo(hDC,rcBox.left+3,rcBox.top+9);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top+10,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top+10);
			winMoveToEx(hDC,rcBox.left+8,rcBox.top+9,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+9);
			winSetPixel(hDC,rcBox.left+9,rcBox.top+8,RGB_SYSTEMBRIGHT);
			winMoveToEx(hDC,rcBox.left+10,rcBox.top+7,&point);
			winLineTo(hDC,rcBox.left+10,rcBox.top+4);
			winSetPixel(hDC,rcBox.left+9,rcBox.top+3,RGB_SYSTEMBRIGHT);

			SelectObject(hDC,hPenGray);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+10,&point);
			winLineTo(hDC,rcBox.left+3,rcBox.top+10);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top+11,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top+11);
			winMoveToEx(hDC,rcBox.left+8,rcBox.top+10,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+10);
			winMoveToEx(hDC,rcBox.left+10,rcBox.top+9,&point);
			winLineTo(hDC,rcBox.left+10,rcBox.top+8);
			winMoveToEx(hDC,rcBox.left+11,rcBox.top+7,&point);
			winLineTo(hDC,rcBox.left+11,rcBox.top+4);
			winMoveToEx(hDC,rcBox.left+10,rcBox.top+3,&point);
			winLineTo(hDC,rcBox.left+10,rcBox.top+2);
			//Draw check
			if(pWin->dwStyle & BS_BUTTON_CHECKED){
				SelectObject(hDC,hPenBlack);
				winMoveToEx(hDC,rcBox.left+4,rcBox.top+5,&point);
				winLineTo(hDC,rcBox.left+7,rcBox.top+5);
				winMoveToEx(hDC,rcBox.left+4,rcBox.top+6,&point);
				winLineTo(hDC,rcBox.left+7,rcBox.top+6);

				winMoveToEx(hDC,rcBox.left+5,rcBox.top+4,&point);
				winLineTo(hDC,rcBox.left+5,rcBox.top+7);
				winMoveToEx(hDC,rcBox.left+6,rcBox.top+4,&point);
				winLineTo(hDC,rcBox.left+6,rcBox.top+7);
			}

			//destroy the hdc object
			DeleteObject(hPenBlack);
			DeleteObject(hPenGray);
			DeleteObject(hPenBright);
			DeleteObject(hPenDark);


			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}


//group radio button control procedure
static LRESULT 
AutoRadioButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	PWindowsTree pWin;
	RECT rc;
	RECT rcBox;
	HDC hDC;

	PAINTSTRUCT ps;
	int iWidth,iHeight;
	int xCenter,yCenter;
	HPEN hPen;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	DWORD dwData;
	PWindowsTree pControl;
	char* pString;
	int iLen;

	HPEN hPenBlack;
	HPEN hPenGray;
	HPEN hPenBright;
	HPEN hPenDark;
	POINT point;
	pWin=(PWindowsTree)hWnd;


	switch(iMsg)
	{
		case LMSG_CREATE:
			if(IsGroup(hWnd))
				pWin->pParent->dwData+=1;
			pWin->dwData=pWin->pParent->dwData;
			break;
		case LMSG_COMMAND:
			break;
		case LMSG_ERASEBKGND:
			pWndClass=GetRegClass(pWin->lpszClassName);
			if(!pWndClass)
				return (LRESULT)NULL;
			hBrush=pWndClass->hbrBackground;
			crColor=((BRUSH*)hBrush)->crBrushColor;
			if(IsFocus(hWnd))
				((BRUSH*)hBrush)->crBrushColor=RGB(93,158,255);
			else
				((BRUSH*)hBrush)->crBrushColor=RGB(144,144,144);

			res=DefWindowProc(hWnd, iMsg, wParam, lParam);
			((BRUSH*)hBrush)->crBrushColor=crColor;
			return res;

		case LMSG_PENDOWN:
			break;
		case LMSG_PENUP:
			if(!IsChecked(hWnd)){
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
				winInvalidateRect(hWnd,NULL,true);
				UpdateWindow(hWnd);
				dwData=pWin->dwData;
				pControl=pWin->pParent->pControlHead;
				while(pControl){
					if(pControl!=pWin){
						if((GetButtonType((HWND)pControl)==BS_AUTORADIOBUTTON) && (pControl->dwData==dwData)){
							SendMessage((HWND)pControl,BM_SETCHECK,(WPARAM)false,(LPARAM)NULL);
						}
					}
					pControl=pControl->pNext;
				}
			}
			NotifyParent(hWnd, BN_CLICKED);
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
		case BM_SETCHECK:
			if(wParam)
				((PWindowsTree)hWnd)->dwStyle |= BS_BUTTON_CHECKED;
			else
				((PWindowsTree)hWnd)->dwStyle &= ~BS_BUTTON_CHECKED;

			winInvalidateRect(hWnd,NULL,true);
			break;
		case BM_GETCHECK:
			if(IsChecked(hWnd))
				return CHECKED;
			else
				return UNCHECKED;
			break;
		case LMSG_PENMOVE:
			break;
		case LMSG_NCPAINT:
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);		
			if(IsBorder(hWnd)){
				hPen=CreatePen(PS_SOLID,1,RGB(88,87,81));
				SelectObject(hDC,hPen);
				DeleteObject(hPen);
				winRectangle(hDC,rc.left,rc.top,rc.right,rc.bottom);
			}
			break;
		case LMSG_PAINT:
			GetWindowRect(hWnd,&rc);
			SetRect(&rc,0,0,rc.right-rc.left,rc.bottom-rc.top);
			iWidth	=rc.right - rc.left + 1;
			iHeight	=rc.bottom - rc.top + 1;
			if(iHeight<13) break;

			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}


			SetTextColor(hDC,RGB(0,0,0));

			DrawText(hDC,pWin->lpszCaption,strlen(pWin->lpszCaption),&rc,DT_RIGHT | DT_VCENTER);
			//CENTER BLANK winRectangle
			yCenter=rc.top + (rc.bottom - rc.top)/2;
			xCenter=7;

			SetRect(&rcBox,xCenter-6,yCenter-6,xCenter+5,yCenter+5);

			hPenBlack	=CreatePen(PS_SOLID,1,RGB_SYSTEMBLACK);
			hPenGray	=CreatePen(PS_SOLID,1,RGB_SYSTEMLTGRAY);
			hPenBright	=CreatePen(PS_SOLID,1,RGB_SYSTEMBRIGHT);
			hPenDark	=CreatePen(PS_SOLID,1,RGB_SYSTEMDARK);

			SelectObject(hDC,hPenDark);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+1);
			winMoveToEx(hDC,rcBox.left+1,rcBox.top+3,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+4);
			winMoveToEx(hDC,rcBox.left,rcBox.top+4,&point);
			winLineTo(hDC,rcBox.left,rcBox.top+7);
			winMoveToEx(hDC,rcBox.left+1,rcBox.top+8,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+9);

			SelectObject(hDC,hPenBlack);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top+1,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top+1);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+2,&point);
			winLineTo(hDC,rcBox.left+3,rcBox.top+2);
			winMoveToEx(hDC,rcBox.left+8,rcBox.top+2,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+2);
			winSetPixel(hDC,rcBox.left+2,rcBox.top+3,RGB_SYSTEMBLACK);
			winMoveToEx(hDC,rcBox.left+1,rcBox.top+4,&point);
			winLineTo(hDC,rcBox.left+1,rcBox.top+7);
			winSetPixel(hDC,rcBox.left+2,rcBox.top+8,RGB_SYSTEMBLACK);

			SelectObject(hDC,hPenBright);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+9,&point);
			winLineTo(hDC,rcBox.left+3,rcBox.top+9);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top+10,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top+10);
			winMoveToEx(hDC,rcBox.left+8,rcBox.top+9,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+9);
			winSetPixel(hDC,rcBox.left+9,rcBox.top+8,RGB_SYSTEMBRIGHT);
			winMoveToEx(hDC,rcBox.left+10,rcBox.top+7,&point);
			winLineTo(hDC,rcBox.left+10,rcBox.top+4);
			winSetPixel(hDC,rcBox.left+9,rcBox.top+3,RGB_SYSTEMBRIGHT);

			SelectObject(hDC,hPenGray);
			winMoveToEx(hDC,rcBox.left+2,rcBox.top+10,&point);
			winLineTo(hDC,rcBox.left+3,rcBox.top+10);
			winMoveToEx(hDC,rcBox.left+4,rcBox.top+11,&point);
			winLineTo(hDC,rcBox.left+7,rcBox.top+11);
			winMoveToEx(hDC,rcBox.left+8,rcBox.top+10,&point);
			winLineTo(hDC,rcBox.left+9,rcBox.top+10);
			winMoveToEx(hDC,rcBox.left+10,rcBox.top+9,&point);
			winLineTo(hDC,rcBox.left+10,rcBox.top+8);
			winMoveToEx(hDC,rcBox.left+11,rcBox.top+7,&point);
			winLineTo(hDC,rcBox.left+11,rcBox.top+4);
			winMoveToEx(hDC,rcBox.left+10,rcBox.top+3,&point);
			winLineTo(hDC,rcBox.left+10,rcBox.top+2);
			//Draw check
			if(pWin->dwStyle & BS_BUTTON_CHECKED){
				SelectObject(hDC,hPenBlack);
				winMoveToEx(hDC,rcBox.left+4,rcBox.top+5,&point);
				winLineTo(hDC,rcBox.left+7,rcBox.top+5);
				winMoveToEx(hDC,rcBox.left+4,rcBox.top+6,&point);
				winLineTo(hDC,rcBox.left+7,rcBox.top+6);

				winMoveToEx(hDC,rcBox.left+5,rcBox.top+4,&point);
				winLineTo(hDC,rcBox.left+5,rcBox.top+7);
				winMoveToEx(hDC,rcBox.left+6,rcBox.top+4,&point);
				winLineTo(hDC,rcBox.left+6,rcBox.top+7);
			}

			//destroy the hdc object
			DeleteObject(hPenBlack);
			DeleteObject(hPenGray);
			DeleteObject(hPenBright);
			DeleteObject(hPenDark);


			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}


static unsigned int 
GetButtonType(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	return pWin->dwStyle & BS_TYPEMASK;
}

static BOOL 
IsChecked(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & BS_BUTTON_CHECKED)==BS_BUTTON_CHECKED)
		return true;
	else
		return false;
}

static BOOL 
IsGroup(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin=(PWindowsTree)hWnd;
	if((pWin->dwStyle & BS_GROUP)==BS_GROUP)
		return true;
	else
		return false;
}



