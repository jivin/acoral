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
#include "../include/sledit.h"
#include "../include/text.h"
#include "../include/keyboard_pro.h"
#include "../include/gtimer.h"


//the field dwStyle of window 
// 1,2 control type 
// 3    control status
// 4    window type
// 5,6 window style
// 7,8 window status

#define EST_FOCUSED     0x00000001L
#define EST_REPLACE     0x00000008L

#define ES_READONLY	0x00100000L	//window status
#define ES_PASSWORD	0x00200000L	//window status



#define MARGIN_EDIT_LEFT       1
#define MARGIN_EDIT_TOP        1
#define MARGIN_EDIT_RIGHT      2
#define MARGIN_EDIT_BOTTOM     1



static int
edtProcessKeyDown(
	HWND hWnd,
	WPARAM wParam,
	LPARAM lParam
);

static int
edtProcessChar(
	HWND hWnd,
	WPARAM wParam,
	LPARAM lParam
);

static BOOL
IsACCharBeforePos(
	char* pBuffer, 
	int iPos
);

static BOOL
IsACCharAtPos(
	char* pBuffer,
	int iLen,
	int iPos
);

//get caret position according to the pendown postion 
static int 
edtGetCaretPos(
	PSLEditData pEditData, 
	int x
);



static int
GetEditBoxDispLen(
	HWND hwnd
);

//In case of moving the cursor to the end of the box
static int 
GetStartPos(
	HWND hWnd,
	PSLEditData pEditData
);








//Register window class 
BOOL 
RegisterSLEditControl()
{
	WNDCLASSEX wcex;

	wcex.cbSize		= sizeof(WNDCLASSEX);
	wcex.style		= 0;
	wcex.lpfnWndProc	= (WNDPROC)SLEditCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon		= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "sledit";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}




//window message process function
LRESULT	
SLEditCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)

{
	PWindowsTree pWin;
	char* buffer;
	POINT point;
	PSLEditData pEditData;
	HPEN hPen;
	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;
	char*	passwdchar;
	int	dispLen,len,iBorder;
	char*	dispBuffer;
	HDC hDC;
	RECT rect;
	PAINTSTRUCT ps;
		
	pWin = (PWindowsTree)hWnd;
	
	switch(iMsg){
	case LMSG_CREATE:
		pWin = (PWindowsTree)hWnd;
		pEditData = (void*)malloc(sizeof(SLEditData));
		if(!pEditData)
			return false;
		if (!CreateCaret (hWnd, NULL, 1, GetSysCharHeight())){
			printerror ("EDIT: Create Caret error!\n");
			free (pEditData);
			return -1;
		}
		memset(pEditData,0,sizeof(SLEditData));	
		pEditData->iBufferLen      = LEN_EDITBOX_BUFFER;
		pEditData->passwdChar     = '*';
		pEditData->iLeftMargin     = MARGIN_EDIT_LEFT;
		pEditData->iTopMargin      = MARGIN_EDIT_TOP;
		pEditData->iRightMargin    = MARGIN_EDIT_RIGHT;
		pEditData->iBottomMargin   = MARGIN_EDIT_BOTTOM;
//		memcpy (pEditData->buffer,	pWin->lpszCaption,min (pEditData->iBufferLen, strlen(pWin->lpszCaption)+1);
		strncpy (pEditData->buffer,	pWin->lpszCaption,pEditData->iBufferLen-1);//modify
		pWin->rect.bottom = pWin->rect.top + GetSysCharHeight()
			+ MARGIN_EDIT_TOP + MARGIN_EDIT_BOTTOM + 2 * wndGetBorder(hWnd); 	
			
		pWin->dwData = (DWORD)pEditData;
		SetTimer(hWnd,1,50);
		break;
	case LMSG_TIMER:
		BlinkCaret(hWnd);
		break;
	case LMSG_DESTROY:
		free((void*)(pWin->dwData));
		DestroyCaret(hWnd);
		KillTimer(hWnd,1);
		break;		
	case LMSG_ERASEBKGND:
		pWndClass=GetRegClass(pWin->lpszClassName);
		if(!pWndClass)
			return (LRESULT)NULL;
		//is Enable ?
		hBrush=pWndClass->hbrBackground;
		crColor=((BRUSH*)hBrush)->crBrushColor;
		if(!IsEnable(hWnd))
			((BRUSH*)hBrush)->crBrushColor=RGB_SYSTEMLTGRAY;
		else
			((BRUSH*)hBrush)->crBrushColor=RGB_SYSTEMWHITE;
		res=DefWindowProc(hWnd, iMsg, wParam, lParam);
		((BRUSH*)hBrush)->crBrushColor=crColor;
		return res;
	case LMSG_KILLFOCUS:
		pEditData = (PSLEditData) (pWin->dwData);
		pEditData->dwStatus &= ~EST_FOCUSED;
		HideCaret (hWnd);
		NotifyParent (hWnd, EN_KILLFOCUS);
		break;
	case LMSG_SETFOCUS:
		pEditData = (PSLEditData) (pWin->dwData);
		if (pEditData->dwStatus & EST_FOCUSED)
			return 0;
		
		pEditData->dwStatus |= EST_FOCUSED;
		
		SetCaretPos (hWnd,
			pEditData->iCaretPos * GetSysCharWidth () + pEditData->iLeftMargin,
			pEditData->iTopMargin);
		
		ShowCaret (hWnd);
		
		NotifyParent (hWnd, EN_SETFOCUS);
		break;	
	case LMSG_GETTEXTLENGTH:
		pEditData = (PSLEditData) (pWin->dwData);
		return pEditData->iDataLen;
	case LMSG_GETTEXT:
		buffer = (char*)lParam;
		pEditData = (PSLEditData) (pWin->dwData);
		
		len = min ((int)wParam, pEditData->iDataLen);
		
		memcpy (buffer, pEditData->buffer, len);
		buffer [len] = '\0';
		return len;
	case LMSG_SETTEXT:
		pEditData = (PSLEditData)(pWin->dwData);
		if(pWin->dwStyle & ES_READONLY)
			return 0;
		pEditData = (PSLEditData) (pWin->dwData);
		len = min (strlen ((char*)lParam), pEditData->iBufferLen);
		
		pEditData->iDataLen = len;
		memcpy (pEditData->buffer, (char*)lParam, len);
		pEditData->iEditPos        = 0;
		pEditData->iCaretPos       = 0;
		pEditData->iStartPos       = 0;
		if (pEditData->dwStatus & EST_FOCUSED)
			SetCaretPos (hWnd, pEditData->iLeftMargin, pEditData->iTopMargin);
		InvalidateRect (hWnd, NULL, TRUE);
		break;
	case EM_SETREADONLY:
		if (wParam)
			pWin->dwStyle |= ES_READONLY;
		else
			pWin->dwStyle &= ~ES_READONLY;
		break;
	case EM_SETPASSWORDCHAR:
		pEditData = (PSLEditData) (pWin->dwData);
		
		if (pEditData->passwdChar != (char)wParam) {
			if (pEditData->dwStatus & ES_PASSWORD) {
			    pEditData->passwdChar = (char)wParam;
			    InvalidateRect (hWnd, NULL, TRUE);
			}
		}
		break;
	case EM_GETPASSWORDCHAR:
		pEditData = (PSLEditData) (pWin->dwData);
		passwdchar = (char*) lParam;
		
		*passwdchar = pEditData->passwdChar;
		break;
	case LMSG_PENDOWN:
	{
		int x,y;
		int iRet;
		CaptureMouse(hWnd,BYCLIENT);
		pEditData = (PSLEditData) (pWin->dwData);
		x = (int)wParam;
		y = (int)lParam;
		ScreenToClient(hWnd, &x,&y);
		iRet = edtGetCaretPos (pEditData, x);
		//printf("%d\n",x);
		
		if (iRet != pEditData->iCaretPos) {
			pEditData->iEditPos += iRet - pEditData->iCaretPos;
			pEditData->iCaretPos = iRet;
			SetCaretPos (hWnd,
				pEditData->iCaretPos * GetSysCharWidth ()
				    + pEditData->iLeftMargin,
				pEditData->iTopMargin);
		}
		NotifyParent(hWnd,EN_CLICKED);
		break;
	}
	case LMSG_PENUP:
		DisCaptureMouse();
		break;
	
	case LMSG_ENABLE:
		if((BOOL)wParam)///Enable
			pWin->dwStyle &=~WS_DISABLE;
		else//Disable
			pWin->dwStyle |=WS_DISABLE;
		break;
	case LMSG_NCPAINT:
		GetWindowRect(hWnd,&rect);
		SetRect(&rect,0,0,rect.right-rect.left,rect.bottom-rect.top);
		if(IsBorder(hWnd)){
			hDC = (HDC)wParam;
			if(!hDC)
				return false;
			hPen=CreatePen(PS_SOLID,1,RGB(88,87,81));
			SelectObject(hDC,hPen);
			DeleteObject(hPen);

			winMoveToEx(hDC,rect.left,rect.top,&point);
			winLineTo(hDC,rect.right,rect.top);

			winMoveToEx(hDC,rect.left,rect.top,&point);
			winLineTo(hDC,rect.left,rect.bottom);

			hPen=GetStockObject(LTGRAY_PEN);
			SelectObject(hDC,hPen);
			DeleteObject(hPen);

			winMoveToEx(hDC,rect.right,rect.top+1,&point);
			winLineTo(hDC,rect.right,rect.bottom);
			winMoveToEx(hDC,rect.right,rect.bottom,&point);
			winLineTo(hDC,rect.left,rect.bottom);
		}	
		break;	
	case LMSG_PAINT:
		ps.bPaintDirect=false;
		hDC=BeginPaint(hWnd, &ps);
		if(!hDC){
			return true;
		}
		if(IsFocus(hWnd)){
			HideCaret(hWnd);
		}
		GetWindowRect(hWnd,&rect);
		SetRect(&rect,0,0,rect.right-rect.left,rect.bottom-rect.top);

    		pEditData = (PSLEditData) (pWin->dwData);
		iBorder = wndGetBorder(hWnd);
		InflateRect(&rect,(-1)*iBorder,(-1)*iBorder);

		dispLen = GetEditBoxDispLen (hWnd);
		
		if (dispLen == 0) {
			EndPaint(hWnd, &ps);
			break;
		}
		
		dispBuffer = malloc (dispLen + 1);
		
		if (pWin->dwStyle & ES_PASSWORD)
			memset (dispBuffer, '*', dispLen);
		else
			memcpy (dispBuffer,
				pEditData->buffer + pEditData->iStartPos,
				dispLen);
		dispBuffer [dispLen] = '\0';
		
		rect.left += pEditData->iLeftMargin;
		rect.top += pEditData->iTopMargin;
		rect.right -= pEditData->iRightMargin;
		rect.bottom -= pEditData->iBottomMargin;

		DrawText(hDC,dispBuffer,strlen(dispBuffer),&rect,DT_LEFT);
		free(dispBuffer);//modify,this is added
		EndPaint(hWnd, &ps);
		if(IsFocus(hWnd)){
			ShowCaret(hWnd);
			SetTimer(hWnd,1,50);
		}
		break;	
	//keydown process
	case LMSG_KEYDOWN:
		edtProcessKeyDown(hWnd,wParam,lParam);
		break;	
	case LMSG_CHAR:
		edtProcessChar(hWnd,wParam,lParam);
		break;
		
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam);		
	}
	return 0;
}




static int
edtProcessKeyDown(
	HWND hWnd,
	WPARAM wParam,
	LPARAM lParam
)
{
	PWindowsTree pWin;
	PSLEditData pEditData;
	BOOL bChange, bScroll;
	int iNewStartPos;
	int scrollStep,moveStep,deleted;
	int iLimit;
	RECT InvRect;
		
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return -1;

	pEditData = (PSLEditData)(pWin->dwData);
	if(!pEditData)
		return -1;
	bChange = false;
	switch(LOWORD(wParam)){
	case SCANCODE_ENTER:
		NotifyParent (hWnd, EN_ENTER);
		break;
	case SCANCODE_HOME:
		if (pEditData->iEditPos == 0)
			break;
		pEditData->iEditPos  = 0;
		pEditData->iCaretPos = 0;
		
		SetCaretPos (hWnd,
			pEditData->iCaretPos * GetSysCharWidth ()
			+ pEditData->iLeftMargin,
			pEditData->iTopMargin);
			
		if (pEditData->iStartPos != 0)
			InvalidateRect (hWnd, NULL, TRUE);
		
		pEditData->iStartPos = 0;
		break;
	case SCANCODE_END:
		if (pEditData->iEditPos == pEditData->iDataLen)
			return 0;
		
		iNewStartPos = GetStartPos (hWnd, pEditData);
		
		pEditData->iEditPos = pEditData->iDataLen;
		pEditData->iCaretPos = pEditData->iEditPos - iNewStartPos;
		
		SetCaretPos (hWnd,
			pEditData->iCaretPos * GetSysCharWidth ()
			+ pEditData->iLeftMargin,
			pEditData->iTopMargin);
		
		if (pEditData->iStartPos != iNewStartPos)
			InvalidateRect (hWnd, NULL, TRUE);
		
		pEditData->iStartPos = iNewStartPos;
		break;	
	case SCANCODE_LEFT:
		//
		bScroll = FALSE;
		if (pEditData->iEditPos == 0)
			return 0;
		
		if (IsACCharBeforePos (pEditData->buffer, pEditData->iEditPos)) {
			scrollStep = 2;
			pEditData->iEditPos -= 2;
		}
		else {
			scrollStep = 1;
			pEditData->iEditPos --;
		}
		
		pEditData->iCaretPos -= scrollStep;
		if (pEditData->iCaretPos == 0 && pEditData->iEditPos != 0) {
			bScroll = TRUE;
			if (IsACCharBeforePos(pEditData->buffer,
				pEditData->iEditPos)) {
				pEditData->iStartPos -= 2;
				pEditData->iCaretPos = 2;
			}
			else{
				pEditData->iStartPos --;
				pEditData->iCaretPos = 1;
			}
		}
		else if (pEditData->iCaretPos < 0) {
			pEditData->iStartPos = 0;
			pEditData->iCaretPos = 0;
		}
		
		SetCaretPos (hWnd,
			pEditData->iCaretPos * GetSysCharWidth ()
			+ pEditData->iLeftMargin,
			pEditData->iTopMargin);
		
		if (bScroll)
			InvalidateRect (hWnd, NULL, TRUE);
		break;	
	case SCANCODE_RIGHT:
		bScroll = FALSE;
		if (pEditData->iEditPos == pEditData->iDataLen)
			return 0;
		
		if (IsACCharAtPos (pEditData->buffer,
				pEditData->iDataLen,
				pEditData->iStartPos)) {
			if (IsACCharAtPos (pEditData->buffer,
					pEditData->iDataLen,
					pEditData->iEditPos)) {
				scrollStep = 2;
				moveStep = 2;
				pEditData->iEditPos  += 2;
			}
			else {
				scrollStep = 2;
				moveStep = 1;
				pEditData->iEditPos ++;
			}
		}
		else {// isn't a chinese character at start position
			if (IsACCharAtPos (pEditData->buffer,
					pEditData->iDataLen,
					pEditData->iEditPos)) {
			
				if (IsACCharAtPos (pEditData->buffer,
						pEditData->iDataLen,
						pEditData->iStartPos + 1))
					scrollStep = 3;
				else
					scrollStep = 2;
				
				moveStep = 2;
				pEditData->iEditPos += 2;
			}
			else {
				scrollStep = 1;
				moveStep = 1;
				pEditData->iEditPos ++;
			}
		}
		iLimit = pWin->rect.right - pWin->rect.left + 1 - 2 * wndGetBorder(hWnd) 
			 - pEditData->iLeftMargin - pEditData->iRightMargin;

		pEditData->iCaretPos += moveStep;
		if (pEditData->iCaretPos * GetSysCharWidth () > iLimit) {
			bScroll = TRUE;
			pEditData->iStartPos += scrollStep;
			
			pEditData->iCaretPos =
					pEditData->iEditPos - pEditData->iStartPos;
		}
		SetCaretPos (hWnd,
				pEditData->iCaretPos * GetSysCharWidth ()
				+ pEditData->iLeftMargin,
				pEditData->iTopMargin);
		
		if (bScroll)
			InvalidateRect (hWnd, NULL, TRUE);
		break;				
	case SCANCODE_INS:
		pEditData = (PSLEditData) (pWin->dwData);
		pEditData->dwStatus ^= EST_REPLACE;
		break;
	case SCANCODE_DEL:
	{
		int i;
		pEditData = (PSLEditData) (pWin->dwData);
		if ((pWin->dwStyle & ES_READONLY)
				|| (pEditData->iEditPos == pEditData->iDataLen)){
			return 0;
		}
		
		if (IsACCharAtPos (pEditData->buffer,
				pEditData->iDataLen,
				pEditData->iEditPos))
			deleted = 2;
		else
			deleted = 1;
	
		for(i=pEditData->iEditPos; i<pEditData->iDataLen; i++)
			pEditData->buffer[i] = pEditData->buffer[i+deleted];	
		//for (i = pEditData->iEditPos;
		//		i < pEditData->iDataLen - deleted; i++)
		//	pEditData->buffer [i] = pEditData->buffer [i + deleted];
		
		pEditData->iDataLen -= deleted;
		bChange = TRUE;
		
		InvRect.left = pEditData->iLeftMargin
				+ pEditData->iCaretPos * GetSysCharWidth ();
		InvRect.top = pEditData->iTopMargin;
		InvRect.right = pWin->rect.right - pWin->rect.left;
		InvRect.bottom = pWin->rect.bottom - pWin->rect.top;
		
		InvalidateRect (hWnd, &InvRect, TRUE);
		break;
	}	
	case SCANCODE_BACKSPACE:
	{
		int i;
		pEditData = (PSLEditData) (pWin->dwData);
		if ((pEditData->dwStatus & ES_READONLY) || (pEditData->iEditPos == 0)) {
			return 0;
		}
		
		if (IsACCharBeforePos (pEditData->buffer, pEditData->iEditPos))
			deleted = 2;
		else
			deleted = 1;
		
		for (i = pEditData->iEditPos - deleted; i < pEditData->iDataLen; i++)
			pEditData->buffer [i] = pEditData->buffer [i + deleted];
		
		pEditData->iDataLen -= deleted;
		pEditData->iEditPos -= deleted;
		bChange = TRUE;
		
		pEditData->iCaretPos -= deleted;
		if (pEditData->iCaretPos == 0 && pEditData->iEditPos != 0) {
			if (IsACCharBeforePos (pEditData->buffer, 
					pEditData->iEditPos)) {
				pEditData->iStartPos -= 2;
				pEditData->iCaretPos = 2;
			}
			else {
				pEditData->iStartPos --;
				pEditData->iCaretPos = 1;
			}
		
			InvRect.left = pEditData->iLeftMargin;
			InvRect.top = pEditData->iTopMargin;
			InvRect.right = pWin->rect.right - pWin->rect.left;
			InvRect.bottom = pWin->rect.bottom - pWin->rect.top;
		}
		else {
			InvRect.left = pEditData->iLeftMargin
					+ pEditData->iCaretPos * GetSysCharWidth ();
			InvRect.top = pEditData->iTopMargin;
			InvRect.right = pWin->rect.right - pWin->rect.left;
			InvRect.bottom = pWin->rect.bottom - pWin->rect.top;
		}
		
		SetCaretPos (hWnd,
				pEditData->iCaretPos * GetSysCharWidth ()
				+ pEditData->iLeftMargin,
				pEditData->iTopMargin);
				
		InvalidateRect (hWnd, &InvRect, TRUE);
		break;
	}
	}
	if (bChange)
		NotifyParent (hWnd, EN_CHANGE);
	return 0;
}



static int
edtProcessChar(
	HWND hWnd,
	WPARAM wParam,
	LPARAM lParam
)
{	
	PWindowsTree pWin;
	PSLEditData pEditData;
	char charBuffer[2];
	int chars, inserting, scrollStep, iLimit;
	RECT InvRect;
	int i;
	
	pWin = (PWindowsTree)hWnd;
	pEditData = (PSLEditData) (pWin->dwData);
	
	if (pWin->dwStyle & ES_READONLY) {
		return 0;
	}
	
	if (HIBYTE (wParam)) {
		charBuffer [0] = LOBYTE (wParam);
		charBuffer [1] = HIBYTE (wParam);
		chars = 2;
	}
	else {
		charBuffer [0] = LOBYTE (wParam);
		chars = 1;
	}
	
	if (chars == 1) {
		switch (charBuffer [0]){
		case 0x00:  // NULL
		case 0x07:  // BEL
		case 0x08:  // BS
		case 0x09:  // HT
		case 0x0A:  // LF
		case 0x0B:  // VT
		case 0x0C:  // FF
		case 0x0D:  // CR
		case 0x1B:  // Escape
		return 0;
		}
	}
	
	if (pEditData->dwStatus & EST_REPLACE) {
		if (pEditData->iDataLen == pEditData->iEditPos)
			inserting = chars;
		else if (IsACCharAtPos (pEditData->buffer,
				pEditData->iDataLen,
				pEditData->iEditPos)) {
			if (chars == 2)
				inserting = 0;
			else
				inserting = -1;
		}
		else {
			if (chars == 2)
				inserting = 1;
			else
				inserting = 0;
		}
	}
	else
		inserting = chars;
	
	// check space
	if (pEditData->iDataLen + inserting > pEditData->iBufferLen) {
		NotifyParent(hWnd,EN_MAXTEXT);
		return 0;
	}

	
	if (inserting == -1) {
		for (i = pEditData->iEditPos; i < pEditData->iDataLen-1; i++)
			pEditData->buffer [i] = pEditData->buffer [i + 1];
	}
	else if (inserting > 0) {
		for (i = pEditData->iDataLen + inserting - 1;
				i > pEditData->iEditPos + inserting - 1;i--)
			pEditData->buffer [i] = pEditData->buffer [i - inserting];
	}
	
	for (i = 0; i < chars; i++)
		pEditData->buffer [pEditData->iEditPos + i] = charBuffer [i];
	
	pEditData->iEditPos += chars;
	pEditData->iCaretPos += chars;
	pEditData->iDataLen += inserting;
	
	iLimit = pWin->rect.right - pWin->rect.left + 1 - 2 * wndGetBorder(hWnd) 
		 - pEditData->iLeftMargin - pEditData->iRightMargin;
	
	if (pEditData->iCaretPos * GetSysCharWidth () > iLimit){
		if (IsACCharAtPos (pEditData->buffer,
				pEditData->iDataLen,
				pEditData->iStartPos))
			scrollStep = 2;
		else {
			if (chars == 2) {
				if (IsACCharAtPos (pEditData->buffer,
						pEditData->iDataLen,
						pEditData->iStartPos + 1))
					scrollStep = 3;
				else
					scrollStep = 2;
			}
			else
				scrollStep = 1;
		}
	
		pEditData->iStartPos += scrollStep;
		
		pEditData->iCaretPos =
				pEditData->iEditPos - pEditData->iStartPos;
		
		InvRect.left = pEditData->iLeftMargin;
		InvRect.top = pEditData->iTopMargin;
		InvRect.right = pWin->rect.right - pWin->rect.left;
		InvRect.bottom = pWin->rect.bottom - pWin->rect.top;
	}
	else {
		InvRect.left = pEditData->iLeftMargin
				+ (pEditData->iCaretPos - chars)
				* GetSysCharWidth ();
		InvRect.top = pEditData->iTopMargin;
		InvRect.right = pWin->rect.right - pWin->rect.left;
		InvRect.bottom = pWin->rect.bottom - pWin->rect.top;
	}
	
	SetCaretPos (hWnd,
			pEditData->iCaretPos * GetSysCharWidth ()
			+ pEditData->iLeftMargin,
			pEditData->iTopMargin);
	InvalidateRect (hWnd, &InvRect, TRUE);
	
	NotifyParent (hWnd, EN_CHANGE);
	
	return 0;	
}

static BOOL
IsACCharBeforePos(
	char* pBuffer, 
	int iPos
)
{
	if(iPos < 2)
		return false;
	if(((unsigned char)*(pBuffer + iPos - 1) > 0xA0) && ((unsigned char)*(pBuffer + iPos - 2) > 0xA0))
		return true;
	return false;
}

static BOOL
IsACCharAtPos(
	char* pBuffer,
	int iLen,
	int iPos
)
{
	if(iPos > iLen - 2)
		return false;
	if(((unsigned char)*(pBuffer + iPos) > 0xA0) && ((unsigned char)*(pBuffer + iPos + 1) > 0xA0))
		return true;
	return false;
	
}

//get caret position according to the pendown postion 

static int 
edtGetCaretPos(
	PSLEditData pEditData, 
	int x
)
{
	char* pBuffer;
	int iRet;
	int iWidth, iSysCharWidth, iSysCCharWidth;
	int iOffset;
	iWidth = 0;
	iRet = 0;
	iSysCharWidth = GetSysCharWidth();
	iSysCCharWidth = GetSysCCharWidth();
	iOffset = x - pEditData->iLeftMargin;
	pBuffer = pEditData->buffer;
	while(*pBuffer){
		if((unsigned char)*pBuffer > 0xA0){
			iWidth += iSysCCharWidth;
			pBuffer += 2; 
			iRet += 2;
		}
		else{
			iWidth += iSysCharWidth;
			pBuffer ++;
			iRet += 1;
		}
		if(iWidth >= iOffset)
			break;
	}
	return iRet;
}



static int
GetEditBoxDispLen(
	HWND hWnd
)
{
	PWindowsTree pWin;
	PSLEditData pEditData;
	int iRet,iCurWidth;
	int iLimit;
	int iSysCharWidth, iSysCCharWidth;
	char* pBuffer;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return -1;
	pEditData = (PSLEditData)(pWin->dwData);
	if(!pEditData)
		return -1;
	iLimit = pWin->rect.right - pWin->rect.left + 1 - 2 * wndGetBorder(hWnd) 
		 - pEditData->iLeftMargin - pEditData->iRightMargin;
	pBuffer = pEditData->buffer;
	iCurWidth = 0;
	iRet = 0;
	iSysCharWidth = GetSysCharWidth();
	iSysCCharWidth = GetSysCCharWidth();
	while(pBuffer){
		if((unsigned char)*pBuffer>0xA0){
			iCurWidth += iSysCCharWidth;
			iRet += 2;
			pBuffer += 2;
		}
		else{
			iCurWidth += iSysCharWidth;
			iRet += 1;
			pBuffer ++;
		}
		if(iCurWidth >= iLimit)
			break;
	}
	return iRet;
	
}

//In case of moving the cursor to the end of the box
static int 
GetStartPos(
	HWND hWnd,
	PSLEditData pEditData
)
{
	PWindowsTree pWin;
	char* pBuffer;
	int iLimit;
	int iPos;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return -1;
	pBuffer = pEditData->buffer;
	if(!pBuffer)
		return -1;
	iLimit = pWin->rect.right - pWin->rect.left + 1 - 2 * wndGetBorder(hWnd) 
		 - pEditData->iLeftMargin - pEditData->iRightMargin;
	iPos = 0;
	while(pBuffer){
		if((unsigned char)*pBuffer > 0xA0){
			iPos +=2;
			pBuffer += 2;
		}
		else{
			iPos ++;
			pBuffer ++;
		}
		if((pEditData->iDataLen - iPos)*GetSysCharWidth() < iLimit)
			return iPos;
	}
	return -1;
		
}


