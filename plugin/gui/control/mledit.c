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
#include "../include/keyboard_pro.h"
//#include "../include/skbwin.h"
#include "../include/text.h"


//=====================================================================================

static BOOL
edtGetALineFromString(
	char* lpString,
	char* pLine
);


static BOOL
edtInitFixData(
	HWND hWnd
);

//Initial editboxox data
static BOOL
edtInitVarData(
	HWND hWnd,
	char* lpString
);

//destroy the editbox data
static BOOL
edtDestroyData(
	HWND hWnd
);


static BOOL
edtGetText(
	HWND hWnd,
	int iLimitLen,
	char* buffer
);

static BOOL
edtPenDown(
	HWND hWnd,
	int x,
	int y
);

static BOOL
edtReCalTopLine(
	HWND hWnd
);

//Get Line Pointer by display Position
static BOOL
edtGetBoxTopLineInfo(
	PMLEditData pEditData,
	PLineData* ppRetLineData,
	int* pStartPos
);

//Get display string by line number
static BOOL
edtGetDispString(
	PLineData* ppLineData,
	int iLineNumber,
	char* pStrDisp
);

//Draw EditBox Text
static BOOL
edtDrawEditBoxText(
	HWND hWnd,
	HDC hDC
);

static BOOL
edtCalCaretPos(
	HWND hWnd,
	int* px,
	int* py
);

static BOOL
edtSetEditPos(
	HWND hWnd
);

static BOOL
edtGetStrPointerByLineNumber(
	PLineData* ppLineData,
	int iLineNumber,
	char** ppString			//return the string pointer 
);


static BOOL
edtGetValidRect(
	HWND hWnd,
	LPRECT lpRect
);

static BOOL
edtIsACChar(
	char* pString
);

static BOOL
edtIsACCharBeforePos(
	char* pBuffer
);

static BOOL
edtIsCarry(
	char* pString
);

static BOOL
edtIsReadOnly(
	HWND hWnd
);

static BOOL
edtIsFocus(
	HWND hWnd
);


static BOOL
edtIsVScroll(
	HWND hWnd
);


static BOOL
edtIsReplace(
	HWND hWnd
);


//get pLineData->pString by edit position
static BOOL
edtGetCurLineData(
	PMLEditData pEditData,
	PLineData* ppLineData
);

//whether caret at begin position of pLineData->pString
static BOOL 
edtIsCaretAtLineBegin(
	PMLEditData pEditData
);

//whether caret at end position of pLineData->pString
static BOOL
edtIsCaretAtLineEnd(
	PMLEditData pEditData
);

static BOOL
edtReIndexDataAfterInsLine(
	PMLEditData pEditData,
	PLineData pCurLineData
);


static BOOL
edtInsLineDataAfter(
	PMLEditData pEditData,
	PLineData pCurLineData
);

static BOOL
edtInsLineDataBefore(
	PMLEditData pEditData,
	PLineData pCurLineData
);

static BOOL
edtReIndexDataAfterBreak(
	PMLEditData pEditData,
	PLineData pCurLineData
);

static BOOL
edtBreakLineAtCurPos(
	PMLEditData pEditData,
	PLineData pCurLineData
);



//////////////////////////////////////////
//Key Down
/////////////////////////////////////////

//====================  ENTER ========================
static BOOL
edtScanCodeEnter(
	HWND hWnd
);


//====================  HOME ========================
static BOOL
edtScanCodeHome(
	HWND hWnd
);


//====================  END ========================
static BOOL
edtScanCodeEnd(
	HWND hWnd
);

//====================  LEFT ========================
static BOOL
edtScanCodeLeft(
	HWND hWnd
);

//====================  RIGHT========================
static BOOL
edtScanCodeRight(
	HWND hWnd
);


static BOOL
edtReCalCaretXPos(
	PMLEditData pEditData,
	int iPrevCaretYPos,
	int *px
);

static BOOL
edtReCalCaretXPosNoRef(
	PMLEditData pEditData,
	int *px
);

//====================  UP ========================
static BOOL
edtScanCodeUp(
	HWND hWnd
);


//====================  Down ========================
static BOOL
edtScanCodeDown(
	HWND hWnd
);



//====================  Ins ========================

static BOOL
edtScanCodeIns(
	HWND hWnd
);


static BOOL
edtReIndexDataAfterDelChar(
	PMLEditData pEditData, 
	PLineData pCurLineData
);


static BOOL
edtReIndexDataAfterDelWrapLine(
	PMLEditData pEditData,
	PLineData pCurLineData
);

static BOOL
edtReIndexDataAfterDelLine(
	PMLEditData pEditData,
	PLineData pCurLineData
);


static BOOL
edtCombineAdjLine(
	PMLEditData pEditData,
	PLineData pLineData
);

//====================  Del ========================
static BOOL
edtScanCodeDel(
	HWND hWnd
);


//====================  BackSpace ========================

static BOOL
edtScanCodeBackSpace(
	HWND hWnd
);

//====================  PageUp ========================

static BOOL
edtScanCodePageUp(
	HWND hWnd
);


//====================  PageDown ========================

static BOOL
edtScanCodePageDown(
	HWND hWnd
);


//====================  CHAR ========================


static BOOL
edtProChar(
	HWND hWnd,
	char* charBuffer,
	int iChars
);

static BOOL
edtProCharInsert(
	HWND hWnd,
	char* charBuffer,
	int iChars
);

static BOOL
edtReplaceChar(
	char* pString,
	char* pChars,
	int iChars,
	int* pReplaced
);
static BOOL
edtProCharReplace(
	HWND hWnd,
	char* charBuffer,
	int iChars
);


//======== scroll ==========
static void 
edtSetVScrollInfo (
	HWND hWnd, 
	PMLEditData  pEditData, 
	BOOL fRedraw
);

PMLEditData pEditData;

//=======================================================================================

//注册窗口类
BOOL RegisterMLEditControl()
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)MLEditCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "mledit";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}






LRESULT 
MLEditCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
//	PLineData pLineData;
	PMLEditData pEditData;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;


	switch(iMsg){
	case LMSG_CREATE:
	{
		int x,y;
	
		pEditData = (PMLEditData)malloc(sizeof(MLEditData));
		if(!pEditData)
			return false;
		memset(pEditData,0,sizeof(MLEditData));
		pWin->dwData = (DWORD)pEditData;
		
		edtInitFixData(hWnd);

		edtInitVarData(hWnd,pWin->lpszCaption);
		//init edit position
		edtSetEditPos(hWnd);

		CreateCaret(hWnd,NULL,1,pEditData->iLineHeight);

		edtCalCaretPos(hWnd,&x,&y);
		SetCaretPos(hWnd,x,y);
		edtSetVScrollInfo(hWnd, pEditData, false);
		//SetTimer(hWnd,1,50,NULL);
		break;
	}
	case LMSG_TIMER:
	{
		BlinkCaret(hWnd);
		break;
	}
	case LMSG_ERASEBKGND:
	{
		PWNDCLASSEX pWndClass;
		HBRUSH hBrush;
		COLORREF crColor;
		LRESULT res;
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
	}
	case LMSG_NCPAINT:
	{
		PWindowsTree pWin;
		BOOL bVScroll;
		RECT rcVBar;
		LPSCROLLCURSTATE pCurState;
		
		RECT rect;
		HDC hDC;
		HPEN hPen;
		POINT point;
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
		
		pWin=(PWindowsTree)hWnd;
		if(!pWin)
			return false;
		if(pWin->dwStyle & WS_VSCROLL)
			bVScroll = true;
		else
			bVScroll = false;
		//draw bound
		//wndDrawNCArea(hWnd, hDC);
		 //draw vert scroll
		 if(bVScroll){
			pCurState = pWin->pVCurState;
			wndGetVScrollBarRect(hWnd,&rcVBar);
			ScreenToWindowRect(hWnd,&rcVBar);
			wndDrawVScrollBar(&rcVBar,hDC,pWin->pVScroll->dwStatus,pCurState);
		}		
		break;
	}
	case LMSG_PAINT:
	{
		HDC hDC;
		PAINTSTRUCT ps;
			
		ps.bPaintDirect=false;
		hDC=BeginPaint(hWnd, &ps);
		if(!hDC){
			return true;
		}
		if(IsFocus(hWnd)){
			HideCaret(hWnd);
		}

		edtDrawEditBoxText(hWnd,hDC);
		
		EndPaint(hWnd, &ps);
		if(IsFocus(hWnd)){
			ShowCaret(hWnd);
			SetTimer(hWnd,1,50);
		}

		break;
	}
	case LMSG_DESTROY:
		DestroyCaret(hWnd);
		KillTimer(hWnd,1);
		edtDestroyData(hWnd);
		break;
	case LMSG_KILLFOCUS:
	{

		if(!edtIsFocus(hWnd))
			break;
		pEditData = (PMLEditData)(pWin->dwData);
		pEditData->dwStatus &= ~ EST_FOCUSED;

		HideCaret(hWnd);
		NotifyParent(hWnd, EN_KILLFOCUS);

		break;
	}
	case LMSG_SETFOCUS:
	{
		int x, y;

		if(edtIsReadOnly(hWnd))
			break;
		if(edtIsFocus(hWnd))
			break;
		pEditData = (PMLEditData)(pWin->dwData);
		if(!pEditData)
			return false;
		pEditData->dwStatus |= EST_FOCUSED;
		edtCalCaretPos(hWnd,&x,&y);

		SetCaretPos(hWnd,x,y);
		ShowCaret(hWnd);
		NotifyParent(hWnd, EN_SETFOCUS);

		break;
	}
	case LMSG_GETTEXTLENGTH:
	{
		pEditData = (PMLEditData)(pWin->dwData);
		if(!pEditData)
			return false;
		return pEditData->iTotalLen;
	}
	case LMSG_GETTEXT:
	{
		edtGetText(hWnd,(int)wParam,(char*)lParam);
		break;
	}
	case LMSG_SETTEXT:
	{
		int x, y;
		if(edtIsReadOnly(hWnd))
			break;
		pEditData = (PMLEditData)(pWin->dwData);
		if(!pEditData)
			return false;
		edtInitVarData(hWnd,(char*)lParam);
		pEditData->iDispPos = 0;
		pEditData->iCaretXPos = 0;
		pEditData->iCaretYPos = 0;
		edtSetEditPos(hWnd);		//set edit position 
		edtCalCaretPos(hWnd,&x,&y);
		SetCaretPos(hWnd,x,y);
		InvalidateRect(hWnd,NULL,true);
		break;
	}
	case EM_SETREADONLY:
	{
		pEditData = (PMLEditData)(pWin->dwData);
		if(wParam){
			pEditData->dwStatus |= EST_READONLY;
		}
		else{
			pEditData->dwStatus &= ~EST_READONLY;
		}

		break;
	}
	case LMSG_PENDOWN:
	{
		int x,y;
		x = (int)wParam;
		y = (int)lParam;
		edtPenDown(hWnd,x,y);
		break;
	}
	case LMSG_PENUP:
		break;
	case LMSG_PENMOVE:
		break;
	case LMSG_KEYDOWN:
	{
		switch LOWORD(wParam){
		case SCANCODE_ENTER:
			edtScanCodeEnter(hWnd);
			break;
		case SCANCODE_HOME:
			edtScanCodeHome(hWnd);
			break;
		case SCANCODE_END:
			edtScanCodeEnd(hWnd);
			break;
		case SCANCODE_LEFT:
			edtScanCodeLeft(hWnd);
			break;
		case SCANCODE_RIGHT:
			edtScanCodeRight(hWnd);
			break;
		case SCANCODE_UP:
			edtScanCodeUp(hWnd);
			break;
		case SCANCODE_DOWN:
			edtScanCodeDown(hWnd);
			break;
		case SCANCODE_INS:
			edtScanCodeIns(hWnd);
			break;
		case SCANCODE_DEL:
			edtScanCodeDel(hWnd);
			break;
		case SCANCODE_BACKSPACE:
			edtScanCodeBackSpace(hWnd);
			break;
		case SCANCODE_PAGEUP:
			edtScanCodePageUp(hWnd);
			break;
		case SCANCODE_PAGEDOWN:
			edtScanCodePageDown(hWnd);
			break;
		default:
			break;
		}
		return 0;
	}
	case LMSG_CHAR:
	{
		int chars;
		char charBuffer[3];
		if(edtIsReadOnly(hWnd))
			return true;
        if (HIBYTE (wParam)) {
            charBuffer [0] = LOBYTE (wParam);
            charBuffer [1] = HIBYTE (wParam);
 
            chars = 2;
        }
        else{
            charBuffer [0] = LOBYTE (wParam);
            chars = 1;
        }

        if (chars == 1) {
            switch (charBuffer [0])
            {
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
		edtProChar(hWnd, charBuffer, chars);
		break;
	}
	//scroll event
	case LMSG_VSCROLL:
	{
		int x,y;
		PMLEditData pEditData;
		
		pEditData = (PMLEditData)(pWin->dwData);
		switch((int)wParam){
		case SB_LINEUP:
			pEditData->iDispPos -= 1;
			break;
		case SB_LINEDOWN:
			pEditData->iDispPos += 1;
			break;
		case SB_THUMBTRACK:
			pEditData->iDispPos = pWin->pVScroll->nPos;
			break;
		}
		edtReCalTopLine(hWnd);
		//caret position
#if 0		
		if(!edtGetBoxTopLineInfo(pEditData, &pLineData, &iStartPos))
			return false;
		iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
		if(!edtGetDispString(&pLineData,iLineNumber, pDispString))
			return false;
		pEditData->iCaretXPos = strlen(pDispString);
#endif
		if(!edtReCalCaretXPos(pEditData, pEditData->iCaretYPos, &x))
			return false;
		pEditData->iCaretXPos = x;
		edtSetEditPos(hWnd);
		edtCalCaretPos(hWnd,&x,&y);
		SetCaretPos(hWnd,x,y);
		
		winInvalidateRect(hWnd,NULL,true);
		break;
	}
	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return 0;
}

static BOOL
edtGetBoxTopLineInfo(
	PMLEditData pEditData,
	PLineData* ppRetLineData,
	int* pStartPos
);

//Get display string by line number
static BOOL
edtGetDispString(
	PLineData* ppLineData,
	int iLineNumber,
	char* pStrDisp
);







static BOOL
edtInitFixData(
	HWND hWnd
)
{
	PMLEditData pEditData;
	RECT rect;


	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)(pWin->dwData);

	pEditData->iBufferLimit = LEN_MLEDIT_LINEBUFFER;

	pEditData->iLeftMargin = MLEDIT_LEFTMARGIN;
	pEditData->iTopMargin = MLEDIT_TOPMARGIN;
	pEditData->iRightMargin = MLEDIT_RIGHTMARGIN;
	pEditData->iBottomMargin = MLEDIT_BOTTOMMARGIN;

	pEditData->iCharSpace = MLEDIT_CHARSPACE;
	pEditData->iLineSpace = MLEDIT_LINESPACE;

	pEditData->iLineHeight = GetSysCharHeight() + pEditData->iLineSpace;
	pEditData->iCharWidth = GetSysCharWidth() + pEditData->iCharSpace; 

	//Get lines and chars per line which can be displayed in the current box.
	edtGetValidRect(hWnd,&rect);
	pEditData->iDispLines = (rect.bottom - rect.top + 1)/pEditData->iLineHeight;
	pEditData->iDispChars = (rect.right - rect.left + 1)/pEditData->iCharWidth;
	return  true;
}


//called by init box data function to get a line from init string
static BOOL
edtGetALineFromString(
	char* lpString,
	char* pLine
)
{
	int iLen;
	char* pString;
	pString = lpString;
	while(*pString){
		if(edtIsCarry(pString))
			break;
		pString ++ ;
	}
	iLen = (int)pString - (int)lpString;
	iLen = min(iLen, LEN_MLEDIT_LINEBUFFER);
	strncpy(pLine,lpString,iLen);
	pLine[iLen] = '\0';
	return true;
}


//set init string to the editbox
static BOOL
edtInitVarData(
	HWND hWnd,
	char* lpString
)
{
	int iLineNumber,iChars,iWrapLines,iPos;
	PLineData pLineData;
	PMLEditData pEditData;
	char *pString,*pLine;

	int iTotalLen,iTotalLines;

	char strLine[LEN_MLEDIT_LINEBUFFER];


	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)(pWin->dwData);
	
	// initialize an empty multiline edit box.
	if(!lpString){
		pLineData = (PLineData)malloc(sizeof(LineData));
		if(!pLineData)
			return false;
		memset(pLineData,0,sizeof(LineData));
		pEditData->pHead = pEditData->pTail = pLineData;
		pEditData->iTotalLines = 1;
		pEditData->pDispPointer = pLineData;
		pEditData->pEditPos = pLineData->pString;
		return true;
	}
	
	pString = lpString;

	iLineNumber = 0;
	iWrapLines = -1;
	iTotalLen = iTotalLines = 0;
	while(*pString){
		edtGetALineFromString(pString, strLine);
		pLineData = (PLineData)malloc(sizeof(LineData));
		if(!pLineData)
			return false;
		memset(pLineData,0,sizeof(LineData));
		strcpy(pLineData->pString,strLine);
		iLineNumber += iWrapLines + 1;
		pLineData->iLineNumber = iLineNumber;

		pLine = strLine;
		iChars = 0;
		iWrapLines = 0;
		iPos = 0;
		while(*pLine){
			if(edtIsACChar(pLine)){
				iChars += 2;

				if(iChars > pEditData->iDispChars){
					pLineData->WrapStartPos[iWrapLines] = iPos;
					iWrapLines ++;
					iChars = 2;
				}
				iPos += 2;
				pLine += 2;	
			}
			else {
				iChars += 1;
				if(iChars > pEditData->iDispChars){
					pLineData->WrapStartPos[iWrapLines] = iPos;
					iWrapLines ++;
					iChars = 1;
				}
				iPos += 1;
				pLine += 1;
			}

		}

		pLineData->iWrapLineNumber = iWrapLines;

		iTotalLines += iWrapLines + 1;
		iTotalLen += strlen(strLine);

		//insert into pEditData structure
		if(pEditData->pHead){
			pEditData->pTail->pNext = pLineData;
			pLineData->pPrev = pEditData->pTail;
			pEditData->pTail = pLineData;
		}
		else
			pEditData->pHead = pEditData->pTail = pLineData;

		pString += strlen(strLine) + 1; //skip last carry character
	}
	pEditData->iTotalLen = iTotalLen;
	pEditData->iTotalLines = iTotalLines;

	pEditData->pDispPointer = pEditData->pHead;

	return true;
}


//destroy the editbox data
static BOOL
edtDestroyData(
	HWND hWnd
)
{
	PMLEditData pEditData;
	PLineData pLineData,pNext;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	if(!pWin)
		return false;
	pEditData = (PMLEditData)pWin->dwData;
	pLineData = pEditData->pHead;
	while(pLineData){
		pNext = pLineData->pNext;
		free(pLineData);
		pLineData = pNext;
	}
	free(pEditData);
	return true;
}


static BOOL
edtGetText(
	HWND hWnd,
	int iLimitLen,
	char* buffer
)
{
	PWindowsTree pWin;
	PMLEditData pEditData;
	PLineData pLineData;
	char *pointer,*pCurLine;
	int iCurTotalLen,iStrLen,iCharWidth;
	pWin = (PWindowsTree)hWnd;
	
	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData){
		*buffer = '\0';
		return false;
	}

	pointer = buffer;
	pLineData = pEditData->pHead;
	iCurTotalLen = 0;
	while(pLineData){
		iStrLen = strlen(pLineData->pString);
		if((iCurTotalLen + iStrLen) < iLimitLen){
			memcpy(pointer,pLineData->pString,iStrLen);
		}
		else{
			pCurLine = pLineData->pString;
			while(pCurLine){
				if(edtIsACChar(pCurLine))
					iCharWidth = 2;
				else
					iCharWidth = 1;
				if((iCurTotalLen + iCharWidth) > iLimitLen){
					//buffer[iCurTotalLen] = '\0';
					break;
				}
				memcpy(pointer,pCurLine,iCharWidth);
				pointer += iCharWidth;
				pCurLine += iCharWidth;
				iCurTotalLen += iCharWidth;
			}
			break;
		}
		
		iCurTotalLen += iStrLen;
		if(pLineData->pNext){
			buffer[iCurTotalLen] = '\n';
			iCurTotalLen += 1;
			pointer += iStrLen + 1;
		}
		pLineData = pLineData->pNext;
	}
	buffer[iCurTotalLen] = '\0';
	return true;
}


static BOOL
edtPenDown(
	HWND hWnd,
	int xPos,
	int yPos
)
{
	PMLEditData pEditData;
	PLineData pLineData;
	char pDispString[LEN_MLEDIT_LINEBUFFER];
	int x, y, iLineHeight, iStartPos, iStrLen, iTotalLen;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	if(!pWin)
		return false;
	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	//Set caret and edit position according to the pen down position
	ScreenToClient(hWnd,&xPos,&yPos);
	iLineHeight = pEditData->iLineHeight;
	for(y=0;y<pEditData->iDispLines;y++){
		if((y+1)* iLineHeight > yPos)
			break;
	}

	if(y == pEditData->iDispLines)
		return true;
	//then y should be the caret Y position
	//get the string of this line	
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
		return false;
	if(!edtGetDispString(&pLineData,pEditData->iDispPos + y, pDispString))
		return false;
	//calculate the x position
	iStrLen = strlen(pDispString);
	iTotalLen = 0;
	x = 0;
	while(pDispString[x]){
		if(iTotalLen >= xPos)
			break;
		if(edtIsACChar(pDispString+x)){
			iTotalLen += GetSysCCharWidth();
			x += 2;
		}
		else{
			iTotalLen += GetSysCharWidth();
			x++;
		}

	}
	//then x should be the caret x position
	pEditData->iCaretXPos = x;
	pEditData->iCaretYPos = y;

	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);
	
	//set edit position according caret position
	edtSetEditPos(hWnd);
	

	return true;
}


//Draw EditBox Text
static BOOL
edtDrawEditBoxText(
	HWND hWnd,
	HDC hDC
)
{
	char pDispString[LEN_MLEDIT_LINEBUFFER];
	RECT rtBox,rtLine;
	PMLEditData pEditData;
	PLineData pLineData;
	int i;
	int iWinHeight, iWinWidth;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;
	
	edtGetValidRect(hWnd,&rtBox);
	iWinHeight = rtBox.bottom - rtBox.top + 1;
	iWinWidth = rtBox.right - rtBox.left + 1;
	rtBox.top = 0;
	rtBox.left = 0;
	rtBox.right = iWinWidth - 1;
	rtBox.bottom = iWinHeight - 1;
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&i))//i represent inline position, not used
		return false;
	


	for(i = 0; i < pEditData->iDispLines; i++){
		//for pLineData may pointer to its subsequence, so pass a pointer to function
		if(!edtGetDispString(&pLineData,pEditData->iDispPos + i, pDispString))
			return false;
		//Draw the String
		rtLine.left = rtBox.left;
		rtLine.right = rtBox.right;
		rtLine.top = rtBox.top + i * pEditData->iLineHeight;
		rtLine.bottom = rtLine.top + pEditData->iLineHeight;
		//DrawText(hDC,pDispString,strlen(pDispString),&rtLine,DT_LEFT|DT_TOP);
		DrawText(hDC,pDispString,strlen(pDispString),&rtLine,DT_LEFT|DT_TOP);
	}
	return true;
}



////////////////////////////////////////////////////////////////////////
//BASE FUNCTION
//////////////////////////////////////////////////////////////////////


//recalculate the top line pointer after scroll
static BOOL
edtReCalTopLine(
	HWND hWnd
)
{
	PMLEditData pEditData;
	PLineData pLineData;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	pLineData = pEditData->pDispPointer;
	if(pEditData->iDispPos > pLineData->iLineNumber){//after scroll down
		while(pLineData){
			if(pLineData->iLineNumber + pLineData->iWrapLineNumber >= pEditData->iDispPos)
				break;
			pLineData = pLineData->pNext;
		}
	}
	else{//after scroll up 
		while(pLineData){
			if((pLineData->iLineNumber + pLineData->iWrapLineNumber >= pEditData->iDispPos)
				&& (pLineData->iLineNumber <= pEditData->iDispPos))
				break;
			pLineData = pLineData->pPrev;
		}
	}

	pEditData->pDispPointer = pLineData;
	return true;
}

//get line data pointer and start position 
static BOOL
edtGetBoxTopLineInfo(
	PMLEditData pEditData,
	PLineData* ppRetLineData,
	int* pStartPos
)
{
	int iDispPos;
	PLineData pLineData;
	if(!pEditData)
		return false;
	pLineData = pEditData->pDispPointer;

	if(!pLineData)
		return false;

	iDispPos = pEditData->iDispPos;
	
	if(pLineData->iLineNumber == iDispPos)
		*pStartPos = 0;
	else
		*pStartPos = pLineData->WrapStartPos[iDispPos - pLineData->iLineNumber];

	*ppRetLineData = pLineData;


	return true;
}


//Get display string by line number
static BOOL
edtGetDispString(
	PLineData* ppLineData,
	int iLineNumber,
	char* pStrDisp
)
{
	int iStrLen;
	int iSubValue;
	int iStrSub;
	PLineData pLineData;

	pLineData = *ppLineData;
	while(iLineNumber > pLineData->iLineNumber + pLineData->iWrapLineNumber){
		pLineData = pLineData->pNext;
		if(!pLineData)
			return false;
	}
	iSubValue = iLineNumber - pLineData->iLineNumber;
	if(iSubValue == 0){//Should begin from start position of this string
		if(pLineData->iWrapLineNumber == 0)
			iStrLen = strlen(pLineData->pString);
		else
			iStrLen = pLineData->WrapStartPos[0];

		strncpy(pStrDisp, pLineData->pString, iStrLen);
		pStrDisp[iStrLen] = '\0';
	}
	else if(iSubValue == pLineData->iWrapLineNumber){
		iStrSub = pLineData->WrapStartPos[iSubValue-1];
		iStrLen = strlen(pLineData->pString) - pLineData->WrapStartPos[iSubValue-1];
		strncpy(pStrDisp,&pLineData->pString[iStrSub],iStrLen);
		pStrDisp[iStrLen] = '\0';
	}
	else{
		iStrLen = pLineData->WrapStartPos[iSubValue]
			- pLineData->WrapStartPos[iSubValue - 1];

		iStrSub = pLineData->WrapStartPos[iSubValue-1];

		strncpy(pStrDisp,&pLineData->pString[iStrSub],iStrLen);

		pStrDisp[iStrLen] = '\0';
	}
	*ppLineData = pLineData;	
	return true;
}




//Get Client Position of caret
static BOOL
edtCalCaretPos(
	HWND hWnd,
	int* px,
	int* py
)
{
	int x,y;
	PWindowsTree pWin;
	PMLEditData pEditData;
	
	pWin = (PWindowsTree)hWnd;

	
	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	x = pEditData->iCaretXPos;
	y = pEditData->iCaretYPos;

	*px = x * pEditData->iCharWidth + pEditData->iLeftMargin;
	*py = y * pEditData->iLineHeight + pEditData->iTopMargin;

	return true;

}

//Calculate edit position according caret positon
static BOOL
edtSetEditPos(
	HWND hWnd
)
{
	char* pEditString;
	PMLEditData pEditData;
	PLineData pLineData;
	int iStartPos;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
		return false;
	if(!pLineData)
		return false;

	if(!edtGetStrPointerByLineNumber(&pLineData,
		pEditData->iDispPos + pEditData->iCaretYPos,
		&pEditString))
		return false;

	pEditData->pEditPos = pEditString + pEditData->iCaretXPos;
	//pEditString pointer to the begin of edit string
	return true;
}

static BOOL
edtGetStrPointerByLineNumber(
	PLineData* ppLineData,
	int iLineNumber,
	char** ppString			//return the string pointer 
)
{
	int iSubValue;
	PLineData pLineData;

	pLineData = *ppLineData;
	while(iLineNumber > pLineData->iLineNumber + pLineData->iWrapLineNumber){
		pLineData = pLineData->pNext;
		if(!pLineData)
			return false;
	}
	iSubValue = iLineNumber - pLineData->iLineNumber;
	if(iSubValue == 0){//Should begin from start position of this string
		*ppString = pLineData->pString;
	}
	else
		*ppString = pLineData->pString + pLineData->WrapStartPos[iSubValue - 1];

	return true;	
}
	

static BOOL
edtGetValidRect(
	HWND hWnd,
	LPRECT lpRect
)
{

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;

	GetClientRect(hWnd,lpRect);
	lpRect->left += MLEDIT_LEFTMARGIN;
	lpRect->right -= MLEDIT_RIGHTMARGIN;
	lpRect->top += MLEDIT_TOPMARGIN;
	lpRect->bottom -= MLEDIT_BOTTOMMARGIN;
	return true;
}


static BOOL
edtIsACChar(
	char* pString
)
{
	if(!*pString)
		return false;
	if(!*(pString+1))
		return false;
	if(((unsigned char)*pString > 0xA0) && ((unsigned char)*(pString+1) > 0xA0))
		return true;
	return false;
	
}

static BOOL
edtIsACCharBeforePos(
	char* pBuffer
)
{
	//if(!*pBuffer)
	//	return false;
	if(((unsigned char)*(pBuffer-1) > 0xA0) &&
		((unsigned char)*(pBuffer-2) > 0xA0))
		return true;
	return false;
}

static BOOL
edtIsCarry(
	char* pString
)
{
	if(((unsigned char)*pString == '\n') || ((unsigned char)*pString == '\r'))
		return true;
	else
		return false;
}

static BOOL
edtIsFocus(
	HWND hWnd
)
{
	PMLEditData pEditData;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	if(pEditData->dwStatus & EST_FOCUSED)
		return true;
	return false;
}

static BOOL
edtIsVScroll(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(pWin->dwStyle & WS_VSCROLL)
		return true;
	else
		return  false;
}
static BOOL
edtIsReadOnly(
	HWND hWnd
)
{
	PMLEditData pEditData;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	if(pEditData->dwStatus & EST_READONLY)
		return true;
	return false;
}


static BOOL
edtIsReplace(
	HWND hWnd
)
{
	PMLEditData pEditData;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	if(pEditData->dwStatus & EST_REPLACE)
		return true;
	return false;
}


//get pLineData by edit position
static BOOL
edtGetCurLineData(
	PMLEditData pEditData,
	PLineData* ppLineData
)
{
	PLineData pLineData;

	if(!pEditData)
		return false;

	pLineData = pEditData->pDispPointer;
	while(pLineData){
		if(pLineData->iLineNumber + pLineData->iWrapLineNumber >= 
			pEditData->iDispPos + pEditData->iCaretYPos)
			break;
		pLineData = pLineData->pNext;
	}
	*ppLineData = pLineData;
	return true;

}

//whether caret at begin position of pLineData->pString
static BOOL 
edtIsCaretAtLineBegin(
	PMLEditData pEditData
)
{
	PLineData pLineData;

	pLineData = pEditData->pDispPointer;
	while(pLineData){
		if(pLineData->iLineNumber + pLineData->iWrapLineNumber >= 
			pEditData->iDispPos + pEditData->iCaretYPos)
			break;
		pLineData = pLineData->pNext;
	}
	if(pEditData->pEditPos == pLineData->pString)
		return true;
	return false;
}

//whether caret at end position of pLineData->pString
static BOOL
edtIsCaretAtLineEnd(
	PMLEditData pEditData
)
{
	PLineData pLineData;
	pLineData = pEditData->pDispPointer;
	
	while(pLineData){
		if(pLineData->iLineNumber + pLineData->iWrapLineNumber >= 
			pEditData->iDispPos + pEditData->iCaretYPos)
			break;
		pLineData = pLineData->pNext;
	}

	if(pEditData->pEditPos == pLineData->pString + strlen(pLineData->pString))
		return true;

	return false;
}




//recalculate pCurLineData line wrap position
//and recode the line number
static BOOL
edtReIndexDataAfterInsLine(
	PMLEditData pEditData,
	PLineData pCurLineData
)
{
	PLineData pLineData;
	int iChars, iWrapLines, iPos, iLineNumber;
	char* pLine;
	int iIncrement;

	//get previous line data
	pLineData = pCurLineData->pPrev;
	if(!pLineData){
		iLineNumber = 0;
		pEditData->pDispPointer = pCurLineData;
	}
	else{
		iLineNumber = pLineData->iLineNumber + pLineData->iWrapLineNumber + 1;
	}
	
	pCurLineData->iLineNumber = iLineNumber;
	//recalculate pCurLinedata line wrap position
	pLine = pCurLineData->pString;
	iChars = 0;
	iWrapLines = 0;
	iPos = 0;
	while(*pLine){
		if(edtIsACChar(pLine)){
			iChars += 2;
			if(iChars > pEditData->iDispChars){
				pLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 2;
			}
			iPos += 2;
			pLine += 2;	
		}
		else {
			iChars += 1;
			if(iChars > pEditData->iDispChars){
				pLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 1;
			}
			iPos += 1;
			pLine += 1;
		}
	}
	
	pCurLineData->iWrapLineNumber = iWrapLines;
	iIncrement = iWrapLines + 1;
	//recode following pLineData line number
	pLineData = pCurLineData->pNext;
	while(pLineData){
		pLineData->iLineNumber += iIncrement;
		pLineData = pLineData->pNext;
	}


	pEditData->iTotalLines += iIncrement;
	pEditData->iTotalLen += strlen(pCurLineData->pString);

	return true;
}


static BOOL
edtInsLineDataAfter(
	PMLEditData pEditData,
	PLineData pCurLineData
)
{
	PLineData pLineData;
	if(!pEditData)
		return false;
	if(!pCurLineData)
		return false;

	pLineData = (PLineData)malloc(sizeof(LineData));
	if(!pLineData)
		return false;
	memset(pLineData,0,sizeof(LineData));

	if(pCurLineData->pNext){
		pCurLineData->pNext->pPrev = pLineData;
		pLineData->pPrev = pCurLineData;
		pLineData->pNext = pCurLineData->pNext;
		pCurLineData->pNext = pLineData;
	}
	else{
		pCurLineData->pNext = pLineData;
		pLineData->pPrev = pCurLineData;
		pEditData->pTail = pLineData;
	}
	edtReIndexDataAfterInsLine(pEditData, pLineData);
	return true;
}

static BOOL
edtInsLineDataBefore(
	PMLEditData pEditData,
	PLineData pCurLineData
)
{
	PLineData pLineData;
	if(!pEditData)
		return false;
	if(!pCurLineData)
		return false;

	pLineData = (PLineData)malloc(sizeof(LineData));
	if(!pLineData)
		return false;
	memset(pLineData,0,sizeof(LineData));

	if(pCurLineData->pPrev){
		pCurLineData->pPrev->pNext = pLineData;
		pLineData->pPrev = pCurLineData->pPrev;
		pCurLineData->pPrev = pLineData;
		pLineData->pNext = pCurLineData;
	}
	else{//first line data of pEditData
		pLineData->pNext = pCurLineData;
		pCurLineData->pPrev = pLineData;
		pEditData->pHead = pLineData;

	}
	edtReIndexDataAfterInsLine(pEditData, pLineData);
	return true;

}


//recalculate pCurLineData line wrap position
//and recode the line number
static BOOL
edtReIndexDataAfterBreak(
	PMLEditData pEditData,
	PLineData pCurLineData
)
{
	PLineData pLineData;
	int iChars, iWrapLines, iPos;
	char* pLine;
	int iIncrement;
	int i;
	
	//recalculate pCurLinedata line wrap position
	iIncrement = (-1) * (pCurLineData->iWrapLineNumber + 1); //this field not updated when this function called
	for(i=0; i<2; i++){
		pLine = pCurLineData->pString;
		iChars = 0;
		iWrapLines = 0;
		iPos = 0;
		while(*pLine){
			if(edtIsACChar(pLine)){
				iChars += 2;
				if(iChars > pEditData->iDispChars){
					pCurLineData->WrapStartPos[iWrapLines] = iPos;
					iWrapLines ++;
					iChars = 2;
				}
				iPos += 2;
				pLine += 2;	
			}
			else {
				iChars += 1;
				if(iChars > pEditData->iDispChars){
					pCurLineData->WrapStartPos[iWrapLines] = iPos;
					iWrapLines ++;
					iChars = 1;
				}
				iPos += 1;
				pLine += 1;
			}
		}
		pCurLineData->iWrapLineNumber = iWrapLines;
		iIncrement += iWrapLines + 1;
		if(i == 1){
			pCurLineData->iLineNumber = 
				pCurLineData->pPrev->iLineNumber + pCurLineData->pPrev->iWrapLineNumber + 1;
			break;
		}
		pCurLineData = pCurLineData->pNext;
	}


	//recode following pLineData line number
	pLineData = pCurLineData->pNext;
	while(pLineData){
		pLineData->iLineNumber += iIncrement;
		pLineData = pLineData->pNext;
	}


	pEditData->iTotalLines += iIncrement;

	return true;
}



static BOOL
edtBreakLineAtCurPos(
	PMLEditData pEditData,
	PLineData pCurLineData
)
{
	PLineData pLineData;
	if(!pEditData)
		return false;
	pLineData = (PLineData)malloc(sizeof(LineData));
	if(!pLineData)
		return false;
	memset(pLineData, 0, sizeof(LineData));
	strcpy(pLineData->pString, pEditData->pEditPos);
	*(pEditData->pEditPos) = '\0';
	
	if(pCurLineData->pNext){
		pCurLineData->pNext->pPrev = pLineData;
		pLineData->pPrev = pCurLineData;
		pLineData->pNext = pCurLineData->pNext;
		pCurLineData->pNext = pLineData;
	}
	else{
		pLineData->pNext = NULL;
		pLineData->pPrev = pCurLineData;
		pCurLineData->pNext = pLineData;
	}

	edtReIndexDataAfterBreak(pEditData,pCurLineData);
	
	return true;
}

////////////////////////////////////////////////////////////////////////////
//Key Down
///////////////////////////////////////////////////////////////////////////


//====================  ENTER ========================
static BOOL
edtScanCodeEnter(
	HWND hWnd
)
{
	int iWinHeight, iWinWidth;
	RECT rtInvalid;
	PMLEditData pEditData;
	PLineData pCurLineData;
	int x, y;
	BOOL bScroll = false;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;



	if(edtIsReadOnly(hWnd))
		return true;
	

	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;

	//in replace mode	
	if(edtIsReplace(hWnd)){
		if(pEditData->iCaretYPos + 1 == pEditData->iDispLines){
			//caret at the bottom of the edit box
			if(pEditData->iDispPos + pEditData->iDispLines ==
				pEditData->iTotalLines)
				return true;
			else{
				pEditData->iCaretXPos = 0;
				pEditData->iDispPos += 1;
				bScroll = true;
			}
		}
		else{//caret not at the bottom of the edit box
			//move caret to the begin of next line
			pEditData->iCaretXPos = 0;
			pEditData->iCaretYPos ++;
		}
		if(bScroll){
			edtReCalTopLine(hWnd);
			edtSetVScrollInfo(hWnd, pEditData, true);
			InvalidateRect(hWnd,NULL,true);
		}

		edtSetEditPos(hWnd);
		edtCalCaretPos(hWnd,&x,&y);
		SetCaretPos(hWnd,x,y);
		return true;
	}

	//in insert mode 

	//Get current line data  
	if(!edtGetCurLineData(pEditData, &pCurLineData))
		return false;

	//caret position is an important reference value 
	if(edtIsCaretAtLineEnd(pEditData)){
		//insert a new line after current position
		edtInsLineDataAfter(pEditData, pCurLineData);
	}
	else if(edtIsCaretAtLineBegin(pEditData)){
		//insert a new line before current position
		edtInsLineDataBefore(pEditData, pCurLineData);	
	}
	else{
		//break the current line into two lines at current position
		edtBreakLineAtCurPos(pEditData,pCurLineData);
	}
	edtSetVScrollInfo(hWnd, pEditData, true);
//	if(pEditData->iCaretYPos == 0){
//		pEditData->pDispPointer == pCurLineData;
	if(pEditData->iCaretYPos + 1 == pEditData->iDispLines){
		pEditData->iCaretXPos = 0;
		pEditData->iDispPos += 1;
		//bScroll = true;
		edtReCalTopLine(hWnd);
		InvalidateRect(hWnd,NULL,true);
	}
	else{//caret not at the bottom of the edit box
		//move caret to the begin of next line
		GetWindowRect(hWnd,&rtInvalid);
		iWinHeight = rtInvalid.bottom - rtInvalid.top + 1;
		iWinWidth = rtInvalid.right - rtInvalid.left + 1;
		rtInvalid.top = 0;
		rtInvalid.left = 0;
		rtInvalid.right = iWinWidth - 1;
		rtInvalid.bottom = iWinHeight - 1;
		rtInvalid.top += (pEditData->iCaretYPos * pEditData->iLineHeight);
		pEditData->iCaretXPos = 0;
		pEditData->iCaretYPos ++;
		InvalidateRect(hWnd,&rtInvalid,true);
	}

	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);



	NotifyParent(hWnd,EN_CHANGE);



	return true;
}


//====================  HOME ========================
static BOOL
edtScanCodeHome(
	HWND hWnd
)
{
	PMLEditData pEditData;
	int x, y;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	//move caret and edit position to the begin of this line
	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;

	pEditData->iCaretXPos = 0;
	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);

	return true;
}


//====================  END ========================
static BOOL
edtScanCodeEnd(
	HWND hWnd
)
{
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	PMLEditData pEditData;
	PLineData pLineData;
	int iLineNumber;
	int x, y;
	int iStartPos;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	pEditData = (PMLEditData)pWin->dwData;

	iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
		return false;
	if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
		return false;

	pEditData->iCaretXPos = strlen(pStrDisp);

	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);


	return true;
}
//====================  LEFT ========================
static BOOL
edtScanCodeLeft(
	HWND hWnd
)
{
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	int x, y;
	PMLEditData pEditData;
	PLineData pLineData;
	int iStartPos, iLineNumber;
	BOOL bScroll;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;


	bScroll = false;

	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;

	//get string
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData, &iStartPos))
		return false;
	iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
	if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
		return false;

	if(pEditData->iCaretXPos != 0){
		if(edtIsACCharBeforePos(pStrDisp + pEditData->iCaretXPos))
			pEditData->iCaretXPos -= 2;
		else
			pEditData->iCaretXPos -= 1;
	}
	else{//have to move caret to the previous line
		if(iLineNumber == pEditData->iDispPos){//caret at top line of the editbox
			if(pEditData->iDispPos == 0)
				return true;
			else{
				pEditData->iDispPos -- ;
				if(!edtGetBoxTopLineInfo(pEditData, &pLineData, &iStartPos))
					return false;
				iLineNumber = pEditData->iDispPos;
				if(!edtGetDispString(&pLineData, iLineNumber, pStrDisp))
					return false;
				pEditData->iCaretXPos = strlen(pStrDisp);
				bScroll = true;
			}
		}
		else{//caret not at top line of the editbox
			pEditData->iCaretYPos -- ;
			//if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
			//	return false;
			iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
			if(!edtGetDispString(&pLineData, iLineNumber, pStrDisp))
				return false;
			pEditData->iCaretXPos = strlen(pStrDisp);

		}
	}
	if(bScroll){
		edtReCalTopLine(hWnd);
		edtSetVScrollInfo(hWnd, pEditData, true);
		InvalidateRect(hWnd,NULL,true);
	}
	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);
	return true;
}

//====================  RIGHT========================
static BOOL
edtScanCodeRight(
	HWND hWnd
)
{
	int x,y;
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	PMLEditData pEditData;
	PLineData pLineData;
	int iStartPos;
	int iLineNumber;
	BOOL bScroll;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;



	bScroll = false;

	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;
	//get string
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData, &iStartPos))
		return false;
	iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
	if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
		return false;
	
	if(pEditData->iCaretXPos == (int)strlen(pStrDisp)){//move to next line
		if(pEditData->iDispPos + pEditData->iCaretYPos + 1 >= pEditData->iTotalLines)
			return true;//already at the end
		else{
			pEditData->iCaretXPos = 0;
			if(pEditData->iCaretYPos + 2 > pEditData->iDispLines){
				//pEditData->iCaretYPos += 1; //don't increase the value of caret y pos 
				pEditData->iDispPos += 1;
				bScroll = true;
			}
			else
				pEditData->iCaretYPos += 1; 
		}
	}
	else{
		if(edtIsACChar(pEditData->pEditPos))
			pEditData->iCaretXPos += 2;
		else
			pEditData->iCaretXPos += 1;
	}
	if(bScroll){
		edtReCalTopLine(hWnd);
		edtSetVScrollInfo(hWnd, pEditData, true);
		InvalidateRect(hWnd,NULL,true);
	}

	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);


	return true;
}


static BOOL
edtReCalCaretXPos(
	PMLEditData pEditData,
	int iPrevCaretYPos,
	int *px
)
{
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	int iLineNumber, iStartPos;
	PLineData pLineData;
	int iCharNumber, iByteNumber;
	int x;
	//get string
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData, &iStartPos))
		return false;
	iLineNumber = pEditData->iDispPos + iPrevCaretYPos;
	if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
		return false;
	//Get character number 
	iCharNumber = iByteNumber = 0;
	while(iByteNumber < pEditData->iCaretXPos){
		if(edtIsACChar(pStrDisp + iCharNumber))
			iByteNumber += 2;
		else
			iByteNumber += 1;
		iCharNumber += 1;
	}
	iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
	if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
		return false;
	
	x = iByteNumber = 0;
	while(x < iCharNumber){
		if(!*(pStrDisp + iByteNumber))
			break;
		if(edtIsACChar(pStrDisp + iByteNumber))
			iByteNumber += 2;
		else
			iByteNumber += 1;
		x += 1;
	}
	*px = iByteNumber;
	return true;
}


static BOOL
edtReCalCaretXPosNoRef(
	PMLEditData pEditData,
	int *px
)
{
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	int iLineNumber, iStartPos;
	PLineData pLineData;
	int x;
	//get string
	if(!edtGetBoxTopLineInfo(pEditData,&pLineData, &iStartPos))
		return false;
	iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
	if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
		return false;
	x = 0;
	while(x < pEditData->iCaretXPos){
		if(!*(pStrDisp + x))
			break;
		if(edtIsACChar(pStrDisp + x))
			x += 2;
		else
			x += 1;
	}
	*px = x;
	return true;
}
//====================  UP ========================
static BOOL
edtScanCodeUp(
	HWND hWnd
)
{
	int x,y;
	PMLEditData pEditData;

	BOOL bScroll;


	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;


	bScroll = false;

	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;

	if(pEditData->iCaretYPos == 0){//at top line of edit box
		if(pEditData->iDispPos == 0)
			return true;
		else{
			pEditData->iDispPos -- ;
			bScroll = true;
		}
	}
	else{
		pEditData->iCaretYPos -- ;
	}

	//recalculate caret x position 
	if(!edtReCalCaretXPos(pEditData, pEditData->iCaretYPos + 1, &x))
		return false;
	pEditData->iCaretXPos = x;
	

	if(bScroll){
		edtReCalTopLine(hWnd);
		edtSetVScrollInfo(hWnd, pEditData, true);
		InvalidateRect(hWnd,NULL,true);
	}

	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);

	return true;
}


//====================  Down ========================
static BOOL
edtScanCodeDown(
	HWND hWnd
)
{
	int x,y;
	PMLEditData pEditData;

	BOOL bScroll;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;



	bScroll = false;

	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;

	if(pEditData->iCaretYPos + 2 > pEditData->iDispLines){//caret at bottom of the edit box

		if(pEditData->iDispPos + pEditData->iCaretYPos + 1 >= pEditData->iTotalLines)
			return true;//already at the end
		else{
			pEditData->iDispPos += 1;
			bScroll = true;
		}
	}
	else{//caret not at bottom of the edit box
		pEditData->iCaretYPos += 1;
	}

	//recalculate caret x position 
	if(!edtReCalCaretXPos(pEditData, pEditData->iCaretYPos - 1, &x))
		return false;
	pEditData->iCaretXPos = x;

	

	if(bScroll){
		edtReCalTopLine(hWnd);
		edtSetVScrollInfo(hWnd, pEditData, true);
		InvalidateRect(hWnd,NULL,true);
	}

	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);

	return true;
}



//====================  Ins ========================
static BOOL
edtScanCodeIns(
	HWND hWnd
)
{
	PMLEditData pEditData;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;


	pEditData = (PMLEditData)pWin->dwData;
	if(!pEditData)
		return false;
	pEditData->dwStatus ^= EST_REPLACE;
	return true;
}





static BOOL
edtReIndexDataAfterDelWrapLine(
	PMLEditData pEditData,
	PLineData pCurLineData
)
{

	pCurLineData->iWrapLineNumber -= 1;
	pCurLineData->WrapStartPos[pCurLineData->iWrapLineNumber] = 0;

	pEditData->iTotalLines -= 1;
	pCurLineData = pCurLineData->pNext;
	while(pCurLineData){
		pCurLineData->iLineNumber -= 1;
		pCurLineData = pCurLineData->pNext;
	}

	return true;
}

static BOOL
edtReIndexDataAfterDelChar(
	PMLEditData pEditData, 
	PLineData pCurLineData
)
{
	PLineData pLineData;
	int iWrapLines;
	char* pLine;
	int iChars, iPos;
	int iDecrease;

	pLine = pCurLineData->pString;
	iChars = 0;
	iWrapLines = 0;
	iPos = 0;
	while(*pLine){
		if(edtIsACChar(pLine)){
			iChars += 2;
			if(iChars > pEditData->iDispChars){
				pCurLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 2;
			}
			iPos += 2;
			pLine += 2;	
		}
		else {
			iChars += 1;
			if(iChars > pEditData->iDispChars){
				pCurLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 1;
			}
			iPos += 1;
			pLine += 1;
		}
	}
	if(iWrapLines != pCurLineData->iWrapLineNumber){
		iDecrease = pCurLineData->iWrapLineNumber - iWrapLines;
		pCurLineData->iWrapLineNumber = iWrapLines;
		pLineData = pCurLineData->pNext;
		while(pLineData){
			pLineData->iLineNumber -= iDecrease;
			pLineData = pLineData->pNext;
		}
		pEditData->iTotalLines -= iDecrease;
	}
	
	return true;
}

//recalculate pCurLineData line wrap position
//and recode the line number
static BOOL
edtReIndexDataAfterDelLine(
	PMLEditData pEditData,
	PLineData pCurLineData
)
{
	PLineData pLineData;
	int iChars, iWrapLines, iPos;
	char* pLine;
	int iDecrease;

	//recalculate pCurLinedata line wrap position
	pLine = pCurLineData->pString;
	iChars = 0;
	iWrapLines = 0;
	iPos = 0;
	while(*pLine){
		if(edtIsACChar(pLine)){
			iChars += 2;
			if(iChars > pEditData->iDispChars){
				pCurLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 2;
			}
			iPos += 2;
			pLine += 2;	
		}
		else {
			iChars += 1;
			if(iChars > pEditData->iDispChars){
				pCurLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 1;
			}
			iPos += 1;
			pLine += 1;
		}
	}
	pCurLineData->iWrapLineNumber = iWrapLines;

	pLineData = pCurLineData->pNext;
	if(!pLineData){
		pEditData->iTotalLines = pCurLineData->iLineNumber 
			+ pCurLineData->iWrapLineNumber + 1;
		return true;
	}

	iDecrease = pLineData->iLineNumber - 
		(pCurLineData->iLineNumber + pCurLineData->iWrapLineNumber + 1);
	if(!iDecrease)
		return true;

	while(pLineData){
		pLineData->iLineNumber -= iDecrease;
		pLineData = pLineData->pNext;
	}
	
	pEditData->iTotalLines -= iDecrease;

	return true;
}



static BOOL
edtCombineAdjLine(
	PMLEditData pEditData,
	PLineData pLineData
)
{
	PLineData pNextLineData;
	
	pNextLineData = pLineData->pNext;
	if(!pNextLineData)
		return false;
	if(pNextLineData->pNext){//pNextLineData is not the end of the link table
		pLineData->pNext = pNextLineData->pNext;
		pNextLineData->pNext->pPrev = pLineData;
	}
	else{//pNextLineData is at the end of the link table
		pLineData->pNext = NULL;
	}
	strcat(pLineData->pString, pNextLineData->pString);
	free(pNextLineData);
	
	edtReIndexDataAfterDelLine(pEditData,pLineData);
		
	return true;
}


//====================  Del ========================
static BOOL
edtScanCodeDel(
	HWND hWnd
)
{
	int iStartPos;
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	char* pString;
	int iLineNumber, iLen;
	int x, y;
	int iWinHeight, iWinWidth;

	BOOL bChange;
	BOOL bScroll;
	RECT rtInvalid;
	PMLEditData pEditData;
	PLineData pLineData, pPrevLineData;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;


	bChange = false;
	bScroll = false;

	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;

	if(!edtGetCurLineData(pEditData,&pLineData))
		return false;
	if(edtIsCaretAtLineEnd(pEditData)){
		if(!pLineData->pNext){//at the end of all text
			if(strlen(pLineData->pString)==0){
				//if it's not the last left line, remove it
				pPrevLineData = pLineData->pPrev;
				if(pPrevLineData){
					if(pEditData->pDispPointer == pLineData)
						pEditData->pDispPointer = pPrevLineData;
					pPrevLineData->pNext = NULL;
					free(pLineData);
					pLineData = NULL;
					pEditData->iTotalLines -= 1;
					
				}
				//process caret position
				if(pEditData->iCaretYPos){
					pEditData->iCaretYPos -= 1;
					if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
						return false;
					iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
					if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
						return false;
					pEditData->iCaretXPos = strlen(pStrDisp);
				}
				else{//caret at top line of the box
					if(pEditData->iDispPos){
						pEditData->iDispPos --;
						bScroll = true;
						if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
							return false;
						iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
						if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
							return false;
						pEditData->iCaretXPos = strlen(pStrDisp);
					}
					else{//this is the last left line.
						//no action
					}
				}
			}
		}
		else{//not at the end of all text
			//combine the next line to the current line 
			edtCombineAdjLine(pEditData, pLineData);
		}
	}
	else{//not at line end 
		//delete a character at current position
		pString = pEditData->pEditPos;
		if(edtIsACChar(pString))
			iLen = 2;
		else
			iLen = 1;
		while(1){
			*pString = *(pString+iLen);
			if(!*(pString+iLen))
				break;
			pString ++;
		}

		pEditData->iTotalLen -= iLen;

		iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
		if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
			return false;
		if(strlen(pStrDisp) == 0){
			if((pEditData->iDispPos + pEditData->iCaretYPos + 1	== pEditData->iTotalLines)
				&& (strlen(pLineData->pString) !=0 )){//for caret y pos beyond the text content scale
				//move caret to end of upper line
				//but if current pLineData is empty after del this character,
				//reserver this empty line and do not move caret to the upper line
				if(pEditData->iCaretYPos){
					pEditData->iCaretYPos --;
				}
				else if(pEditData->iDispPos){
					pEditData->iDispPos -= 1;
					bScroll = true;
				}
				iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
				if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
					return false;
				if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
					return false;
				pEditData->iCaretXPos = strlen(pStrDisp);
				edtReIndexDataAfterDelWrapLine(pEditData, pLineData);
			}
//			else{//combine next line
//				if(pLineData->pNext)
//					edtCombineAdjLine(pEditData, pLineData);
//			}
			
		}
		else{
			edtReIndexDataAfterDelChar(pEditData, pLineData);
		}
	}
	edtSetVScrollInfo(hWnd, pEditData, true);
	GetWindowRect(hWnd, &rtInvalid);
	iWinHeight = rtInvalid.bottom - rtInvalid.top - 1;
	iWinWidth = rtInvalid.right - rtInvalid.left - 1;
	rtInvalid.top = 0;
	rtInvalid.left = 0;
	rtInvalid.bottom = iWinHeight - 1;
	rtInvalid.right = iWinWidth - 1;
	rtInvalid.top = pEditData->iCaretYPos * pEditData->iLineHeight;
	InvalidateRect(hWnd,&rtInvalid,true);
	if(bScroll)
		edtReCalTopLine(hWnd);
	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);



	NotifyParent(hWnd,EN_CHANGE);


	return true;
}


//====================  BackSpace ========================

static BOOL
edtScanCodeBackSpace(
	HWND hWnd
)
{
	int iStartPos;
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	char* pString;
	int iLineNumber, iLen;
	int x, y;
	int iWinHeight, iWinWidth;
	RECT rtInvalid;
	
	PMLEditData pEditData;
	PLineData pLineData, pPrevLineData;


	PWindowsTree pWin;
	BOOL bScroll = false;
	pWin = (PWindowsTree)hWnd;


	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;
	if(!edtGetCurLineData(pEditData,&pLineData))
		return false;
	//whether at the begin position of a plinedata line
	if(pEditData->iCaretXPos == 0){
		if(pEditData->pEditPos == pLineData->pString){//at pLineData begin positioin
			//combine with previous line
			pPrevLineData = pLineData->pPrev;
			if(!pPrevLineData)
				return true;
			if(pEditData->iCaretYPos){
				pEditData->iCaretYPos --;
			}
			else if(pEditData->iDispPos){
				pEditData->iDispPos --;
				pEditData->pDispPointer = pPrevLineData;
				bScroll = true;
			}
			iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;

			if(!edtGetDispString(&pPrevLineData,iLineNumber,pStrDisp))
				return false;
			pEditData->iCaretXPos = strlen(pStrDisp);

			if(!edtCombineAdjLine(pEditData,pPrevLineData))
				return false;
		}
		else{

			if(pEditData->iCaretYPos){
				pEditData->iCaretYPos -- ;
			}
			else if(pEditData->iDispPos){
				pEditData->iDispPos --;
				bScroll = true;
			}
			iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;

			if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
				return false;
			
			pString = pEditData->pEditPos;
			if(edtIsACCharBeforePos(pString))
				iLen = 2;
			else
				iLen = 1;

			pString -= iLen;
			while(1){
				*pString = *(pString+iLen);
				if(!*(pString+iLen))
					break;
				pString ++;
			}
			pEditData->iTotalLen -= iLen;		
			edtReIndexDataAfterDelChar(pEditData, pLineData);

			pEditData->iCaretXPos = strlen(pStrDisp) - iLen;
			
		}
	}
	else{//caret not at the begin positon of a line
		//delete a left character 
		pString = pEditData->pEditPos;
		if(edtIsACCharBeforePos(pString))
			iLen = 2;
		else
			iLen = 1;
		pString -= iLen;
		while(1){
			*pString = *(pString+iLen);
			if(!*(pString+iLen))
				break;
			pString ++;
		}
		pEditData->iTotalLen -= iLen;

		iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
		if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
			return false;

		pEditData->iCaretXPos -= iLen;

		if(strlen(pStrDisp) == 0){//the deleted character is an only character of this line 
			if(strlen(pLineData->pString) !=0){
				if(pEditData->iCaretYPos){
					pEditData->iCaretYPos --;
				}
				else if(pEditData->iDispPos){
					pEditData->iDispPos -= 1;
					bScroll = true;
				}
				iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
				if(!edtGetBoxTopLineInfo(pEditData,&pLineData,&iStartPos))
					return false;
				if(!edtGetDispString(&pLineData,iLineNumber,pStrDisp))
					return false;
				pEditData->iCaretXPos = strlen(pStrDisp);
				edtReIndexDataAfterDelWrapLine(pEditData, pLineData);
			}
			else{//combine next line
				if(pLineData->pNext)
					edtCombineAdjLine(pEditData, pLineData);
			}
		}
		else{// strlen(pStrDisp) != 0, 
			edtReIndexDataAfterDelChar(pEditData, pLineData);
		}
	}
	edtSetVScrollInfo(hWnd, pEditData, true);
	GetWindowRect(hWnd, &rtInvalid);
	iWinHeight = rtInvalid.bottom - rtInvalid.top - 1;
	iWinWidth = rtInvalid.right - rtInvalid.left - 1;
	rtInvalid.top = 0;
	rtInvalid.left = 0;
	rtInvalid.bottom = iWinHeight - 1;
	rtInvalid.right = iWinWidth - 1;
	rtInvalid.top = pEditData->iCaretYPos * pEditData->iLineHeight;
	InvalidateRect(hWnd,&rtInvalid,true);
	if(bScroll)
		edtReCalTopLine(hWnd);
	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);



	NotifyParent(hWnd,EN_CHANGE);


	return true;
}

//====================  PageUp ========================

static BOOL
edtScanCodePageUp(
	HWND hWnd
)
{
	PMLEditData pEditData;

	int x, y;
	BOOL bScroll;

	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;


	bScroll = false;

	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;

	if(pEditData->iDispPos == 0)
		return true;

	if(pEditData->iDispPos > pEditData->iDispLines)
		pEditData->iDispPos -= pEditData->iDispLines;
	else
		pEditData->iDispPos = 0;


	//recalculate caret x position 
	if(!edtReCalCaretXPosNoRef(pEditData, &x))
		return false;
	pEditData->iCaretXPos = x;

	edtReCalTopLine(hWnd);
	edtSetVScrollInfo(hWnd, pEditData, true);
	InvalidateRect(hWnd,NULL,true);


	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);


	return true;
}


//====================  PageDown ========================

static BOOL
edtScanCodePageDown(
	HWND hWnd
)
{
	PMLEditData pEditData;

	int x, y;

	BOOL bScroll;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;


	bScroll = false;

	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;

	if(pEditData->iDispPos + pEditData->iDispLines == pEditData->iTotalLines)
		return true;
	
	if(pEditData->iDispPos + 2* pEditData->iDispLines < pEditData->iTotalLines)
		pEditData->iDispPos += pEditData->iDispLines;
	else
		pEditData->iDispPos = pEditData->iTotalLines - pEditData->iDispLines;


	//recalculate caret x position 
	if(!edtReCalCaretXPosNoRef(pEditData, &x))
		return false;
	pEditData->iCaretXPos = x;

	edtReCalTopLine(hWnd);
	edtSetVScrollInfo(hWnd, pEditData, true);
	InvalidateRect(hWnd,NULL,true);


	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);

	return true;

}






static BOOL
edtReIndexDataAfterInsChar(
	PMLEditData pEditData, 
	PLineData pCurLineData
)
{
	PLineData pLineData;
	int iWrapLines;
	char* pLine;
	int iChars, iPos;
	int iIncrease;

	pLine = pCurLineData->pString;
	iChars = 0;
	iWrapLines = 0;
	iPos = 0;
	while(*pLine){
		if(edtIsACChar(pLine)){
			iChars += 2;
			if(iChars > pEditData->iDispChars){
				pCurLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 2;
			}
			iPos += 2;
			pLine += 2;	
		}
		else {
			iChars += 1;
			if(iChars > pEditData->iDispChars){
				pCurLineData->WrapStartPos[iWrapLines] = iPos;
				iWrapLines ++;
				iChars = 1;
			}
			iPos += 1;
			pLine += 1;
		}
	}
	if(iWrapLines != pCurLineData->iWrapLineNumber){
		iIncrease = iWrapLines - pCurLineData->iWrapLineNumber;
		pCurLineData->iWrapLineNumber = iWrapLines;
		pLineData = pCurLineData->pNext;
		while(pLineData){
			pLineData->iLineNumber += iIncrease;
			pLineData = pLineData->pNext;
		}
		pEditData->iTotalLines += iIncrease;
	}
	
	return true;
}

static BOOL
edtProChar(
	HWND hWnd,
	char* charBuffer,
	int iChars
)
{
	if(edtIsReplace(hWnd))
		return edtProCharReplace(hWnd,charBuffer,iChars);
	else
		return edtProCharInsert(hWnd,charBuffer,iChars);
}

static BOOL
edtProCharInsert(
	HWND hWnd,
	char* charBuffer,
	int iChars
)
{
	RECT rtInvalid;
	char pString[LEN_MLEDIT_LINEBUFFER];
	char pStrDisp[LEN_MLEDIT_LINEBUFFER];
	int iLineNumber;
	int iLen;
	PMLEditData pEditData;
	PLineData pLineData;
	int x, y;
	int iWinHeight, iWinWidth;
	BOOL bScroll;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;

	bScroll = false;


	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;

	if(!edtGetCurLineData(pEditData, &pLineData))
		return false;
	iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
	
	if(!edtGetDispString(&pLineData, iLineNumber, pStrDisp))
		return false;
	if(strlen(pLineData->pString) + iChars > LEN_MLEDIT_LINEBUFFER){
		NotifyParent(hWnd,EN_MAXTEXT);

		return false;
	}
	GetWindowRect(hWnd, &rtInvalid);
	iWinHeight = rtInvalid.bottom - rtInvalid.top + 1;
	iWinWidth = rtInvalid.right - rtInvalid.left + 1;
	rtInvalid.top = 0;
	rtInvalid.left = 0;
	rtInvalid.bottom = iWinHeight - 1;
	rtInvalid.right = iWinWidth - 1;
	rtInvalid.top = pEditData->iCaretYPos * pEditData->iLineHeight;


	if(pEditData->iCaretXPos + iChars > pEditData->iDispChars){
		//need wrap to next line
		if(pEditData->iCaretYPos == pEditData->iDispLines - 1){
			pEditData->iDispPos ++;
			pEditData->iCaretXPos = iChars;
			bScroll = true;
		}
		else{
			pEditData->iCaretYPos ++;
			pEditData->iCaretXPos = iChars;
		}
	}
	else{
		pEditData->iCaretXPos += iChars;
	}

	//at plinedata end ?
	if(!*pEditData->pEditPos){
		iLen = strlen(pLineData->pString);
		strncat(pLineData->pString, charBuffer, iChars);
		pLineData->pString[iLen + iChars] = '\0';
	}
	else{//insert into current position
		strcpy(pString,pEditData->pEditPos);
		strncpy(pEditData->pEditPos,charBuffer,iChars);
		*(pEditData->pEditPos + iChars) = '\0';
		strcat(pEditData->pEditPos,pString);
	}
	pEditData->pEditPos += iChars;
	pEditData->iTotalLen += iChars;

	if(!edtReIndexDataAfterInsChar(pEditData, pLineData))
		return false;
	
	if(bScroll)
		edtReCalTopLine(hWnd);

	edtSetVScrollInfo(hWnd, pEditData, true);

	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);

	if(bScroll)
		InvalidateRect(hWnd, NULL, true);
	else
		InvalidateRect(hWnd,&rtInvalid,true);

	NotifyParent(hWnd,EN_CHANGE);

	return true;
}


static BOOL
edtReplaceChar(
	char* pString,
	char* pChars,
	int iChars,
	int* pReplaced
)
{
	char pBuffer[LEN_MLEDIT_LINEBUFFER];
	int iLen;
	if(!*pString)
		return false;
	if(edtIsACChar(pString))
		iLen = 2;
	else
		iLen = 1;
	*pReplaced = iLen;

	strcpy(pBuffer,pString+iLen);
	strncpy(pString, pChars, iChars);
	pString[iChars] = '\0';
	strcat(pString, pBuffer);

	pString[iChars + strlen(pBuffer)] = '\0';

	return true;
}

static BOOL
edtProCharReplace(
	HWND hWnd,
	char* charBuffer,
	int iChars
)
{
	RECT rtInvalid;
	int iLineNumber;
	int iLen, iReplaced;
	PMLEditData pEditData;
	PLineData pLineData;
	int x, y;
	int iWinHeight, iWinWidth;
	PWindowsTree pWin;
	BOOL bScroll;
	pWin = (PWindowsTree)hWnd;

	bScroll = false;


	pEditData = (PMLEditData)(pWin->dwData);
	if(!pEditData)
		return false;

	if(!edtGetCurLineData(pEditData, &pLineData))
		return false;
	iLineNumber = pEditData->iDispPos + pEditData->iCaretYPos;
	

	GetWindowRect(hWnd, &rtInvalid);
	iWinHeight = rtInvalid.bottom - rtInvalid.top - 1;
	iWinWidth = rtInvalid.right - rtInvalid.left - 1;
	rtInvalid.top = 0;
	rtInvalid.left = 0;
	rtInvalid.bottom = iWinHeight - 1;
	rtInvalid.right = iWinWidth - 1;
	rtInvalid.top = pEditData->iCaretYPos * pEditData->iLineHeight;


	
	if(edtIsCaretAtLineEnd(pEditData)){
		if(strlen(pLineData->pString) + iChars > LEN_MLEDIT_LINEBUFFER){
			NotifyParent(hWnd,EN_MAXTEXT);
			return false;
		}

		if(!pLineData->pNext){//at the end of total text
			iLen = strlen(pLineData->pString);
			strncat(pLineData->pString, charBuffer, iChars);
			pLineData->pString[iLen + iChars] = '\0';

			if(!edtReIndexDataAfterInsChar(pEditData, pLineData))
				return false;
		}
		else{//not at the end of total text
			//need wrap to next line?
			if(pEditData->iCaretXPos + iChars > pEditData->iDispChars){
				//is next line empty?
				pLineData = pLineData->pNext;
				if(*pLineData->pString){//next line not empty
					//replace this character
					if(!edtReplaceChar(pLineData->pString, charBuffer, iChars, &iReplaced))
						return false;
					pEditData->iTotalLen += (iChars - iReplaced);
					if(iChars > iReplaced){
						if(!edtReIndexDataAfterInsChar(pEditData, pLineData))
							return false;
					}
					else if(iChars < iReplaced){
						if(!edtReIndexDataAfterDelChar(pEditData, pLineData))
							return false;
					}
				}
				else{//next line is empty
					strncpy(pLineData->pString, charBuffer, iChars);
					pLineData->pString[iChars] = '\0';
					pEditData->iTotalLen += iChars;
				}
			}
			else{//doesn't need wrap to next line
				//append to end of this line
				iLen = strlen(pLineData->pString);
				strncat(pLineData->pString, charBuffer, iChars);
				pLineData->pString[iLen + iChars] = '\0';
			}
		}
	}
	else{//caret not at line end
		if(edtIsACChar(pEditData->pEditPos))
			iLen = 2;
		else
			iLen = 1;
		if(strlen(pLineData->pString) + iChars - iLen > LEN_MLEDIT_LINEBUFFER){
			NotifyParent(hWnd,EN_MAXTEXT);
			return false;
		}

		edtReplaceChar(pEditData->pEditPos, charBuffer, iChars, &iReplaced);
		if(iChars > iReplaced){
			if(!edtReIndexDataAfterInsChar(pEditData, pLineData))
				return false;
		}
		else if(iChars < iReplaced){
			if(!edtReIndexDataAfterDelChar(pEditData, pLineData))
				return false;
		}
		pEditData->iTotalLen += (iChars - iReplaced);
	}


	if(pEditData->iCaretXPos + iChars > pEditData->iDispChars){
		//need wrap to next line
		if(pEditData->iCaretYPos == pEditData->iDispLines - 1){
			pEditData->iDispPos ++;
			pEditData->iCaretXPos = iChars;
			bScroll = true;
		}
		else{
			pEditData->iCaretYPos ++;
			pEditData->iCaretXPos = iChars;
		}
	}
	else{
		pEditData->iCaretXPos += iChars;
	}


	if(bScroll)
		edtReCalTopLine(hWnd);
	edtSetVScrollInfo(hWnd, pEditData, true);
	
	edtSetEditPos(hWnd);
	edtCalCaretPos(hWnd,&x,&y);
	SetCaretPos(hWnd,x,y);

	if(bScroll)
		InvalidateRect(hWnd,NULL,true);
	else
		InvalidateRect(hWnd,&rtInvalid,true);


	NotifyParent(hWnd,EN_CHANGE);

	return true;
}




//SCROLL

static void 
edtSetVScrollInfo (
	HWND hWnd, 
	PMLEditData  pEditData, 
	BOOL fRedraw
)
{
	SCROLLINFO si;

	if(!edtIsVScroll(hWnd))
		return;
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMax = pEditData->iTotalLines - 1;
	si.nMin = 0;
	si.nPage = pEditData->iDispLines;
	si.nPos = pEditData->iDispPos;
	SetScrollInfo (hWnd, SB_VERT, &si, fRedraw);
	EnableScrollBar (hWnd, SB_VERT, TRUE);
}

