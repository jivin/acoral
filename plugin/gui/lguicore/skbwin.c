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
#include "../include/keyboard_pro.h"
#include "../include/skbwin.h"
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
#include "../include/imewin.h"



extern int Gal_iBytesDataType;

static BOOL 
InitKeyBoardData();


static void 
DestroyKeyBoardData(
	HWND hWnd
);


static BOOL 
GetClickedKeyValue(
	int x, 
	int y,
	int* iScanCode,
	int* iSubValue
);


static void 
SkbProcessKeyDown(
	HWND hWnd,
	int x, 
	int y
);


static void* 
GetLayoutStartAddress(
	void* pData, 
	int iLayout
);


static void 
InvertSetupArea(
	HDC hDC
);


static void 
InvertSKBRect(
	HDC hDC,
	LPRECT lpRect
);


static void 
SkbProcessKeyUp(
	HWND hWnd, 
	int x, 
	int y
);


PWindowsTree _lGUI_pSkbWindow;
extern PWindowsTree _lGUI_pImeWindow;


void 
CreateSkbWindow(
	LPRECT lpRect
)
{
	if(_lGUI_pSkbWindow)
		DestroySkbWindow();

	CreateWindow("skbwindow",NULL,WS_SKBWIN ,lpRect->left,lpRect->top,
		(lpRect->right - lpRect->left + 1),(lpRect->bottom - lpRect->top + 1),
		_lGUI_pWindowsTree,NULL,NULL,NULL);

}


void 
DestroySkbWindow()
{
	DestroyWindow((HWND)_lGUI_pSkbWindow);
}


void 
ShowSkbWindow()
{
	if(IsVisible((HWND)_lGUI_pSkbWindow))
		return;
	if(IsVisible((HWND)_lGUI_pImeWindow)){
		MoveUpImeWindow();
	}
	ShowWindow((HWND)_lGUI_pSkbWindow,true);
}


void 
HideSkbWindow()
{
	if(!IsVisible((HWND)_lGUI_pSkbWindow))
		return;
	if(IsVisible((HWND)_lGUI_pImeWindow)){
		MoveDownImeWindow();
	}
	ShowWindow((HWND)_lGUI_pSkbWindow,false);
}

BOOL 
RegisterSkbWindow()
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)SkbWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor		= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "skbwindow";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}


LRESULT 
SkbWindowProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	HDC hDC;
	PAINTSTRUCT ps;
	PWindowsTree pWin;

	void* pDestData;
	void* pSrcData;

	int iLayout;

	int iSize;
	POINT point;
	int x,y;


	pWin=(PWindowsTree)hWnd;

	switch(iMsg)
	{
		case LMSG_CREATE:

			InitKeyBoardData(hWnd);
			break;
		case LMSG_PENDOWN:
			CaptureMouse(hWnd,BYCLIENT);
			x = (int)wParam;
			y = (int)lParam;
			ScreenToClient(hWnd,&x, &y);

			SkbProcessKeyDown(hWnd,x,y);
			break;
		case LMSG_PENUP:
			DisCaptureMouse();
			x = (int)wParam;
			y = (int)lParam;
			ScreenToClient(hWnd,&x, &y);
			SkbProcessKeyUp(hWnd,x,y);
			//InvalidateRect(hWnd,&rc,true);

			break;
		case LMSG_PENMOVE:
			point.x = (int)wParam;
			point.y = (int)lParam;
			ScreenToClient(hWnd,&point.x, &point.y);

			break;
		case LMSG_PAINT:

			ps.bPaintDirect=false;
			hDC=BeginPaint(hWnd, &ps);
			if(!hDC){
				return true;
			}


			iLayout = GetSkbLayout();

			pSrcData = ((PLGUIKeyBoard)(pWin->dwAddData))->pData;
			pDestData = hDC->pData;

			iSize = SKBWIN_WIDTH * SKBWIN_HEIGHT * Gal_iBytesDataType;


			pSrcData = GetLayoutStartAddress(pSrcData,iLayout);
			memcpy(pDestData,pSrcData,iSize);
			
			if(GetSkbLayout() != SKB_LAYOUT_HANDWRITE){

				InvertSetupArea(hDC);
			}
		

			EndPaint(hWnd, &ps);
			break;
		case LMSG_DESTROY:
			DestroyKeyBoardData(hWnd);
			break;
		case LMSG_COMMAND:
			break;
		default:
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}


void 
SetSkbLayout(
	int iLayout
)
{
	PLGUIKeyBoard pKeyBoard;
	pKeyBoard = (PLGUIKeyBoard)(_lGUI_pSkbWindow->dwAddData);
	if(pKeyBoard->iCurLayout == iLayout)
		return;
	pKeyBoard->iCurLayout = iLayout;
	if((iLayout == SKB_LAYOUT_STANDARD_EXTEND) ||
	   (iLayout == SKB_LAYOUT_SHIFTED_EXTEND))
		pKeyBoard->bExtend = true;
	else
		pKeyBoard->bExtend = false;
		
	if((iLayout == SKB_LAYOUT_SHIFTED) ||
	   (iLayout == SKB_LAYOUT_SHIFTED_EXTEND))
	   	SetLGUIKBShift(true);
	else
		SetLGUIKBShift(false);
		

	InvalidateRect((HWND)_lGUI_pSkbWindow,NULL,true);
}


int  GetSkbLayout()
{
	return ((PLGUIKeyBoard)(_lGUI_pSkbWindow->dwAddData))->iCurLayout;
}


static BOOL 
GetClickedKeyValue(
	int x, 
	int y,
	int* iScanCode,
	int* iSubValue
)
{
	int iCounter, iTotal;
	PLGUIKeyBoard pKeyBoard;
	pKeyBoard = (PLGUIKeyBoard)(_lGUI_pSkbWindow->dwAddData);
	if(pKeyBoard->bExtend){
		iTotal = NR_KEYNUM;
		for(iCounter = 0; iCounter<iTotal;iCounter++){
			if(PtInRect(&(pKeyBoard->ExtendKeyDefTab[iCounter].rc),x,y)){
				*iScanCode = pKeyBoard->ExtendKeyDefTab[iCounter].iScanCode;
				*iSubValue = iCounter;
				return true;
			}
		}
	}
	else{
		iTotal = NR_KEYNUM - 10;
		for(iCounter = 0; iCounter<iTotal;iCounter++){
			if(PtInRect(&(pKeyBoard->KeyDefTab[iCounter].rc),x,y)){
				*iScanCode = pKeyBoard->ExtendKeyDefTab[iCounter].iScanCode;
				*iSubValue = iCounter;
				return true;
			}
		}
	}
	return false;
}


static void 
SkbProcessKeyDown(
	HWND hWnd,
	int x, 
	int y
)
{

	BOOL bValue;
	int iSubValue;
	int iScanCode;
	PWindowsTree pWin;
	PLGUIKeyBoard pKeyBoard;
	pWin = (PWindowsTree)hWnd;
	pKeyBoard = (PLGUIKeyBoard)(pWin->dwAddData);

	if(pKeyBoard->iCurLayout == SKB_LAYOUT_HANDWRITE)
		return;
	if(!GetClickedKeyValue(x,y,&iScanCode, &iSubValue))
		return;

	if(iSubValue == LEFT_SHIFT_SUB || iSubValue == RIGHT_SHIFT_SUB){
		bValue = IsLGUIKBShift();
		SetLGUIKBShift(!bValue);


		switch(pKeyBoard->iCurLayout){
		case SKB_LAYOUT_STANDARD:
			pKeyBoard->iCurLayout = SKB_LAYOUT_SHIFTED;
			break;
		case SKB_LAYOUT_SHIFTED:
			pKeyBoard->iCurLayout = SKB_LAYOUT_STANDARD;
			break;
		case SKB_LAYOUT_STANDARD_EXTEND:
			pKeyBoard->iCurLayout = SKB_LAYOUT_SHIFTED_EXTEND;
			break;
		case SKB_LAYOUT_SHIFTED_EXTEND:
			pKeyBoard->iCurLayout = SKB_LAYOUT_STANDARD_EXTEND;
			break;
		}

		SetLGUIKBCtrl(false);
		SetLGUIKBAlt(false);
		SetLGUIKBCapsLock(false);

		InvalidateRect(hWnd,NULL,true);
		return;
	}

	if(iSubValue == EXTEND_SUB){
		//layout
		switch(pKeyBoard->iCurLayout){
		case SKB_LAYOUT_STANDARD:
			pKeyBoard->iCurLayout = SKB_LAYOUT_STANDARD_EXTEND;
			break;
		case SKB_LAYOUT_SHIFTED:
			pKeyBoard->iCurLayout = SKB_LAYOUT_SHIFTED_EXTEND;
			break;
		case SKB_LAYOUT_STANDARD_EXTEND:
			pKeyBoard->iCurLayout = SKB_LAYOUT_STANDARD;
			break;
		case SKB_LAYOUT_SHIFTED_EXTEND:
			pKeyBoard->iCurLayout = SKB_LAYOUT_SHIFTED;
			break;
		}
		pKeyBoard->bExtend = !pKeyBoard->bExtend;
		//reset status key value
		
		SetLGUIKBCtrl(false);
		SetLGUIKBAlt(false);
		SetLGUIKBCapsLock(false);
		
		InvalidateRect(hWnd,NULL,true);		
		return;
	}
	//ctrl
	if(iSubValue == LEFT_CTRL_SUB || iSubValue == RIGHT_CTRL_SUB){
		bValue = IsLGUIKBCtrl();
		SetLGUIKBCtrl(!bValue);

		if(pKeyBoard->bExtend){
			InvalidateRect(hWnd,
				&(pKeyBoard->ExtendKeyDefTab[LEFT_CTRL_SUB ].rc),true);
			InvalidateRect(hWnd,
				&(pKeyBoard->ExtendKeyDefTab[RIGHT_CTRL_SUB ].rc),true);
		}
		else{
			InvalidateRect(hWnd,&(pKeyBoard->KeyDefTab[LEFT_CTRL_SUB].rc),true);
			InvalidateRect(hWnd,&(pKeyBoard->KeyDefTab[RIGHT_CTRL_SUB].rc),true);
		}
		return;
	}
	//ALT
	if(iSubValue == LEFT_ALT_SUB || iSubValue == RIGHT_ALT_SUB){
		bValue = IsLGUIKBAlt();
		SetLGUIKBAlt(!bValue);

		if(pKeyBoard->bExtend){
			InvalidateRect(hWnd,
				&(pKeyBoard->ExtendKeyDefTab[LEFT_ALT_SUB].rc),true);
			InvalidateRect(hWnd,
				&(pKeyBoard->ExtendKeyDefTab[RIGHT_ALT_SUB].rc),true);
		}
		else{
			InvalidateRect(hWnd,&(pKeyBoard->KeyDefTab[LEFT_ALT_SUB].rc),true);
			InvalidateRect(hWnd,&(pKeyBoard->KeyDefTab[RIGHT_ALT_SUB].rc),true);			
		}
		return;
	}
	//CAPSLOCK
	if(iSubValue == CAPSLOCK_SUB ){
		bValue = IsLGUIKBCapsLock();
		SetLGUIKBCapsLock(!bValue);
		
		if(pKeyBoard->bExtend)
			InvalidateRect(hWnd,&(pKeyBoard->ExtendKeyDefTab[iSubValue].rc),true);
		else
			InvalidateRect(hWnd,&(pKeyBoard->KeyDefTab[iSubValue].rc),true);
		return;
		
	}

		
	pKeyBoard->iPressed = iSubValue;

	if(pKeyBoard->bExtend)
		InvalidateRect(hWnd,&(pKeyBoard->ExtendKeyDefTab[iSubValue].rc),true);
	else
		InvalidateRect(hWnd,&(pKeyBoard->KeyDefTab[iSubValue].rc),true);

	//Call keyboard_pro procedure to send message
	SendKBMessage(iScanCode);

}


static void 
SkbProcessKeyUp(
	HWND hWnd, 
	int x, 
	int y
)
{
	int iValue;

	PLGUIKeyBoard pKeyBoard;

	pKeyBoard = (PLGUIKeyBoard)(_lGUI_pSkbWindow->dwAddData);

	if(pKeyBoard->iCurLayout == SKB_LAYOUT_HANDWRITE)
		return;

	iValue = pKeyBoard->iPressed;
	

	if(iValue!=-1){
		pKeyBoard->iPressed = -1;
		if(pKeyBoard->bExtend){
			InvalidateRect(hWnd,
				&(pKeyBoard->ExtendKeyDefTab[iValue].rc),true);
		}
		else{
			InvalidateRect(hWnd,
				&(pKeyBoard->KeyDefTab[iValue].rc),true);
		}
	}
}



static void* 
GetLayoutStartAddress(
	void* pData, 
	int iLayout
)
{
	switch(iLayout){
	case SKB_LAYOUT_STANDARD:
		return pData;
	case SKB_LAYOUT_SHIFTED:
		return (void*)((acoral_8 *)pData+SKBWIN_WIDTH*SKBWIN_HEIGHT*Gal_iBytesDataType);
	case SKB_LAYOUT_STANDARD_EXTEND:
		return (void*)((acoral_8*)pData+SKBWIN_WIDTH*SKBWIN_HEIGHT*Gal_iBytesDataType * 2);
	case SKB_LAYOUT_SHIFTED_EXTEND:
		return (void*)((acoral_8*)pData+SKBWIN_WIDTH*SKBWIN_HEIGHT*Gal_iBytesDataType * 3);
	case SKB_LAYOUT_HANDWRITE:
		return (void*)((acoral_8*)pData+SKBWIN_WIDTH*SKBWIN_HEIGHT*Gal_iBytesDataType * 4);
	}
		return NULL;
}



static void 
InvertSetupArea(
	HDC hDC
)
{
	PLGUIKeyBoard pKeyBoard;

	pKeyBoard = (PLGUIKeyBoard)(_lGUI_pSkbWindow->dwAddData);

	if(IsLGUIKBCapsLock()){
		if(pKeyBoard->bExtend)
			InvertSKBRect(hDC,&(pKeyBoard->ExtendKeyDefTab[CAPSLOCK_SUB].rc));
		else
			InvertSKBRect(hDC,&(pKeyBoard->KeyDefTab[CAPSLOCK_SUB].rc));
	}

	if(IsLGUIKBAlt()){
		if(pKeyBoard->bExtend){
			InvertSKBRect(hDC,&(pKeyBoard->ExtendKeyDefTab[LEFT_ALT_SUB].rc));
			InvertSKBRect(hDC,&(pKeyBoard->ExtendKeyDefTab[RIGHT_ALT_SUB].rc));
		}
		else{
			InvertSKBRect(hDC,&(pKeyBoard->KeyDefTab[LEFT_ALT_SUB].rc));
			InvertSKBRect(hDC,&(pKeyBoard->KeyDefTab[RIGHT_ALT_SUB].rc));
		}
	}

	if(IsLGUIKBCtrl()){
		if(pKeyBoard->bExtend){
			InvertSKBRect(hDC,&(pKeyBoard->ExtendKeyDefTab[LEFT_CTRL_SUB].rc));
			InvertSKBRect(hDC,&(pKeyBoard->ExtendKeyDefTab[RIGHT_CTRL_SUB].rc));
		}
		else{
			InvertSKBRect(hDC,&(pKeyBoard->KeyDefTab[LEFT_CTRL_SUB].rc));
			InvertSKBRect(hDC,&(pKeyBoard->KeyDefTab[RIGHT_CTRL_SUB].rc));
		}
	}


	if(pKeyBoard->iPressed!=-1){
		if(pKeyBoard->bExtend){
			InvertSKBRect(hDC,&(pKeyBoard->ExtendKeyDefTab[pKeyBoard->iPressed].rc));
		}
		else{
			InvertSKBRect(hDC,&(pKeyBoard->KeyDefTab[pKeyBoard->iPressed].rc));
		}
	}
}



static void 
InvertSKBRect(
	HDC hDC,
	LPRECT lpRect
)
{
	char *pData;
	int x,y;
	int iWidth, iHeight;
	pData = (char*)(hDC->pData);
	if(!pData)
		return;
	iWidth = lpRect->right - lpRect->left + 1;
	iHeight = lpRect->bottom -lpRect->top + 1;

	pData+=(lpRect->top*SKBWIN_WIDTH + lpRect->left)*Gal_iBytesDataType;

	for(y=0;y<iHeight;y++){
		for(x=0; x<iWidth * Gal_iBytesDataType;x++){
			*(pData+x) = ~(*(pData+x));
		}
		pData += SKBWIN_WIDTH * Gal_iBytesDataType;
	}

}



static BOOL 
InitKeyBoardData(
	HWND hWnd
)
{
	char pString[256];
	char* pData;
	int iSize;
	FILE_P fd;


	PLGUIKeyBoard pKeyBoard;
	PWindowsTree pWin;


	pWin = (PWindowsTree)hWnd;
	if(!pWin)
		return false;
	pKeyBoard = (PLGUIKeyBoard)bmalloc(sizeof(LGUIKeyBoard));
	if(!pKeyBoard)
		return false; 
	
	pWin->dwAddData = (DWORD)pKeyBoard;

	pKeyBoard->iCurLayout = SKB_LAYOUT_STANDARD;
	pKeyBoard->iPressed = -1;
	pKeyBoard->bExtend = false;

	memset((void*)(pKeyBoard->KeyDefTab),0,sizeof(LGUIKeyDefine)*NR_KEYNUM);
	memset((void*)(pKeyBoard->ExtendKeyDefTab),0,sizeof(LGUIKeyDefine)*NR_KEYNUM);

	
	pKeyBoard->KeyDefTab[0].iScanCode = SCANCODE_ESCAPE;
	SetRect(&(pKeyBoard->KeyDefTab[0].rc),0,0,14,15);
	pKeyBoard->KeyDefTab[1].iScanCode = SCANCODE_WAVE;
	SetRect(&(pKeyBoard->KeyDefTab[1].rc),16,0,30,15);
	pKeyBoard->KeyDefTab[2].iScanCode = SCANCODE_1;
	SetRect(&(pKeyBoard->KeyDefTab[2].rc),32,0,46,15);
	pKeyBoard->KeyDefTab[3].iScanCode = SCANCODE_2;
	SetRect(&(pKeyBoard->KeyDefTab[3].rc),48,0,62,15);
	pKeyBoard->KeyDefTab[4].iScanCode = SCANCODE_3;
	SetRect(&(pKeyBoard->KeyDefTab[4].rc),64,0,78,15);
	pKeyBoard->KeyDefTab[5].iScanCode = SCANCODE_4;
	SetRect(&(pKeyBoard->KeyDefTab[5].rc),80,0,94,15);
	pKeyBoard->KeyDefTab[6].iScanCode = SCANCODE_5;
	SetRect(&(pKeyBoard->KeyDefTab[6].rc),96,0,110,15);
	pKeyBoard->KeyDefTab[7].iScanCode = SCANCODE_6;
	SetRect(&(pKeyBoard->KeyDefTab[7].rc),112,0,126,15);
	pKeyBoard->KeyDefTab[8].iScanCode = SCANCODE_7;
	SetRect(&(pKeyBoard->KeyDefTab[8].rc),128,0,142,15);
	pKeyBoard->KeyDefTab[9].iScanCode = SCANCODE_8;
	SetRect(&(pKeyBoard->KeyDefTab[9].rc),144,0,158,15);
	pKeyBoard->KeyDefTab[10].iScanCode = SCANCODE_9;
	SetRect(&(pKeyBoard->KeyDefTab[10].rc),160,0,174,15);
	pKeyBoard->KeyDefTab[11].iScanCode = SCANCODE_0;
	SetRect(&(pKeyBoard->KeyDefTab[11].rc),176,0,190,15);
	pKeyBoard->KeyDefTab[12].iScanCode = SCANCODE_MINUS;
	SetRect(&(pKeyBoard->KeyDefTab[12].rc),192,0,206,15);
	pKeyBoard->KeyDefTab[13].iScanCode = SCANCODE_EQUAL;
	SetRect(&(pKeyBoard->KeyDefTab[13].rc),208,0,222,15);
	pKeyBoard->KeyDefTab[14].iScanCode = SCANCODE_BACKSPACE;
	SetRect(&(pKeyBoard->KeyDefTab[14].rc),224,0,238,15);


	pKeyBoard->KeyDefTab[15].iScanCode = SCANCODE_TAB;
	SetRect(&(pKeyBoard->KeyDefTab[15].rc),0,17,22,32);
	pKeyBoard->KeyDefTab[16].iScanCode = SCANCODE_Q;
	SetRect(&(pKeyBoard->KeyDefTab[16].rc),24,17,38,32);
	pKeyBoard->KeyDefTab[17].iScanCode = SCANCODE_W;
	SetRect(&(pKeyBoard->KeyDefTab[17].rc),40,17,54,32);
	pKeyBoard->KeyDefTab[18].iScanCode = SCANCODE_E;
	SetRect(&(pKeyBoard->KeyDefTab[18].rc),56,17,70,32);
	pKeyBoard->KeyDefTab[19].iScanCode = SCANCODE_R;
	SetRect(&(pKeyBoard->KeyDefTab[19].rc),72,17,86,32);
	pKeyBoard->KeyDefTab[20].iScanCode = SCANCODE_T;
	SetRect(&(pKeyBoard->KeyDefTab[20].rc),88,17,102,32);
	pKeyBoard->KeyDefTab[21].iScanCode = SCANCODE_Y;
	SetRect(&(pKeyBoard->KeyDefTab[21].rc),104,17,118,32);
	pKeyBoard->KeyDefTab[22].iScanCode = SCANCODE_U;
	SetRect(&(pKeyBoard->KeyDefTab[22].rc),120,17,134,32);
	pKeyBoard->KeyDefTab[23].iScanCode = SCANCODE_I;
	SetRect(&(pKeyBoard->KeyDefTab[23].rc),136,17,150,32);
	pKeyBoard->KeyDefTab[24].iScanCode = SCANCODE_O;
	SetRect(&(pKeyBoard->KeyDefTab[24].rc),152,17,166,32);
	pKeyBoard->KeyDefTab[25].iScanCode = SCANCODE_P;
	SetRect(&(pKeyBoard->KeyDefTab[25].rc),168,17,182,32);
	pKeyBoard->KeyDefTab[26].iScanCode = SCANCODE_BRACKET_LEFT;
	SetRect(&(pKeyBoard->KeyDefTab[26].rc),184,17,198,32);
	pKeyBoard->KeyDefTab[27].iScanCode = SCANCODE_BRACKET_RIGHT;
	SetRect(&(pKeyBoard->KeyDefTab[27].rc),200,17,214,32);
	pKeyBoard->KeyDefTab[28].iScanCode = SCANCODE_BACKSLASH;
	SetRect(&(pKeyBoard->KeyDefTab[28].rc),216,17,230,32);

	pKeyBoard->KeyDefTab[59].iScanCode = SCANCODE_EXTPAD;
	SetRect(&(pKeyBoard->KeyDefTab[59].rc),232,17,238,32);

	pKeyBoard->KeyDefTab[29].iScanCode = SCANCODE_CAPSLOCK;
	SetRect(&(pKeyBoard->KeyDefTab[29].rc),0,34,30,49);
	pKeyBoard->KeyDefTab[30].iScanCode = SCANCODE_A;
	SetRect(&(pKeyBoard->KeyDefTab[30].rc),32,34,46,49);
	pKeyBoard->KeyDefTab[31].iScanCode = SCANCODE_S;
	SetRect(&(pKeyBoard->KeyDefTab[31].rc),48,34,62,49);
	pKeyBoard->KeyDefTab[32].iScanCode = SCANCODE_D;
	SetRect(&(pKeyBoard->KeyDefTab[32].rc),64,34,78,49);
	pKeyBoard->KeyDefTab[33].iScanCode = SCANCODE_F;
	SetRect(&(pKeyBoard->KeyDefTab[33].rc),80,34,94,49);
	pKeyBoard->KeyDefTab[34].iScanCode = SCANCODE_G;
	SetRect(&(pKeyBoard->KeyDefTab[34].rc),96,34,110,49);
	pKeyBoard->KeyDefTab[35].iScanCode = SCANCODE_H;
	SetRect(&(pKeyBoard->KeyDefTab[35].rc),112,34,126,49);
	pKeyBoard->KeyDefTab[36].iScanCode = SCANCODE_J;
	SetRect(&(pKeyBoard->KeyDefTab[36].rc),128,34,142,49);
	pKeyBoard->KeyDefTab[37].iScanCode = SCANCODE_K;
	SetRect(&(pKeyBoard->KeyDefTab[37].rc),144,34,158,49);
	pKeyBoard->KeyDefTab[38].iScanCode = SCANCODE_L;
	SetRect(&(pKeyBoard->KeyDefTab[38].rc),160,34,174,49);
	pKeyBoard->KeyDefTab[39].iScanCode = SCANCODE_SEMICOLON;
	SetRect(&(pKeyBoard->KeyDefTab[39].rc),176,34,190,49);
	pKeyBoard->KeyDefTab[40].iScanCode = SCANCODE_APOSTROPHE;
	SetRect(&(pKeyBoard->KeyDefTab[40].rc),192,34,206,49);
	pKeyBoard->KeyDefTab[41].iScanCode = SCANCODE_ENTER;
	SetRect(&(pKeyBoard->KeyDefTab[41].rc),208,34,238,49);


	pKeyBoard->KeyDefTab[42].iScanCode = SCANCODE_LEFTSHIFT;
	SetRect(&(pKeyBoard->KeyDefTab[42].rc),0,51,38,66);
	pKeyBoard->KeyDefTab[43].iScanCode = SCANCODE_Z;
	SetRect(&(pKeyBoard->KeyDefTab[43].rc),40,51,54,66);
	pKeyBoard->KeyDefTab[44].iScanCode = SCANCODE_X;
	SetRect(&(pKeyBoard->KeyDefTab[44].rc),56,51,70,66);
	pKeyBoard->KeyDefTab[45].iScanCode = SCANCODE_C;
	SetRect(&(pKeyBoard->KeyDefTab[45].rc),72,51,86,66);
	pKeyBoard->KeyDefTab[46].iScanCode = SCANCODE_V;
	SetRect(&(pKeyBoard->KeyDefTab[46].rc),88,51,102,66);
	pKeyBoard->KeyDefTab[47].iScanCode = SCANCODE_B;
	SetRect(&(pKeyBoard->KeyDefTab[47].rc),104,51,118,66);
	pKeyBoard->KeyDefTab[48].iScanCode = SCANCODE_N;
	SetRect(&(pKeyBoard->KeyDefTab[48].rc),120,51,134,66);
	pKeyBoard->KeyDefTab[49].iScanCode = SCANCODE_M;
	SetRect(&(pKeyBoard->KeyDefTab[49].rc),136,51,150,66);
	pKeyBoard->KeyDefTab[50].iScanCode = SCANCODE_COMMA;
	SetRect(&(pKeyBoard->KeyDefTab[50].rc),152,51,166,66);
	pKeyBoard->KeyDefTab[51].iScanCode = SCANCODE_PERIOD;
	SetRect(&(pKeyBoard->KeyDefTab[51].rc),168,51,182,66);
	pKeyBoard->KeyDefTab[52].iScanCode = SCANCODE_SLASH;
	SetRect(&(pKeyBoard->KeyDefTab[52].rc),184,51,198,66);
	pKeyBoard->KeyDefTab[53].iScanCode = SCANCODE_RIGHTSHIFT;
	SetRect(&(pKeyBoard->KeyDefTab[53].rc),200,51,238,66);


	pKeyBoard->KeyDefTab[54].iScanCode = SCANCODE_LEFTCONTROL;
	SetRect(&(pKeyBoard->KeyDefTab[54].rc),0,68,22,83);
	pKeyBoard->KeyDefTab[55].iScanCode = SCANCODE_LEFTALT;
	SetRect(&(pKeyBoard->KeyDefTab[55].rc),24,68,46,83);
	pKeyBoard->KeyDefTab[56].iScanCode = SCANCODE_SPACE;
	SetRect(&(pKeyBoard->KeyDefTab[56].rc),48,68,190,83);
	pKeyBoard->KeyDefTab[57].iScanCode = SCANCODE_RIGHTALT;
	SetRect(&(pKeyBoard->KeyDefTab[57].rc),192,68,214,83);
	pKeyBoard->KeyDefTab[58].iScanCode = SCANCODE_RIGHTCONTROL;
	SetRect(&(pKeyBoard->KeyDefTab[58].rc),216,68,238,83);

	

//====================================================================
//=============================Extend Key def tab====================
//====================================================================
	pKeyBoard->ExtendKeyDefTab[0].iScanCode = SCANCODE_ESCAPE;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[0].rc),5,0,15,15);
	pKeyBoard->ExtendKeyDefTab[1].iScanCode = SCANCODE_WAVE;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[1].rc),17,0,27,15);
	pKeyBoard->ExtendKeyDefTab[2].iScanCode = SCANCODE_1;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[2].rc),29,0,39,15);
	pKeyBoard->ExtendKeyDefTab[3].iScanCode = SCANCODE_2;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[3].rc),41,0,51,15);
	pKeyBoard->ExtendKeyDefTab[4].iScanCode = SCANCODE_3;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[4].rc),53,0,63,15);
	pKeyBoard->ExtendKeyDefTab[5].iScanCode = SCANCODE_4;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[5].rc),65,0,75,15);
	pKeyBoard->ExtendKeyDefTab[6].iScanCode = SCANCODE_5;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[6].rc),77,0,87,15);
	pKeyBoard->ExtendKeyDefTab[7].iScanCode = SCANCODE_6;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[7].rc),89,0,99,15);
	pKeyBoard->ExtendKeyDefTab[8].iScanCode = SCANCODE_7;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[8].rc),101,0,111,15);
	pKeyBoard->ExtendKeyDefTab[9].iScanCode = SCANCODE_8;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[9].rc),113,0,123,15);
	pKeyBoard->ExtendKeyDefTab[10].iScanCode = SCANCODE_9;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[10].rc),125,0,135,15);
	pKeyBoard->ExtendKeyDefTab[11].iScanCode = SCANCODE_0;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[11].rc),137,0,147,15);
	pKeyBoard->ExtendKeyDefTab[12].iScanCode = SCANCODE_MINUS;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[12].rc),149,0,159,15);
	pKeyBoard->ExtendKeyDefTab[13].iScanCode = SCANCODE_EQUAL;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[13].rc),161,0,171,15);
	pKeyBoard->ExtendKeyDefTab[14].iScanCode = SCANCODE_BACKSPACE;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[14].rc),173,0,183,15);


	pKeyBoard->ExtendKeyDefTab[15].iScanCode = SCANCODE_TAB;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[15].rc),5,17,21,32);
	pKeyBoard->ExtendKeyDefTab[16].iScanCode = SCANCODE_Q;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[16].rc),23,17,33,32);
	pKeyBoard->ExtendKeyDefTab[17].iScanCode = SCANCODE_W;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[17].rc),35,17,45,32);
	pKeyBoard->ExtendKeyDefTab[18].iScanCode = SCANCODE_E;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[18].rc),47,17,57,32);
	pKeyBoard->ExtendKeyDefTab[19].iScanCode = SCANCODE_R;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[19].rc),59,17,69,32);
	pKeyBoard->ExtendKeyDefTab[20].iScanCode = SCANCODE_T;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[20].rc),71,17,81,32);
	pKeyBoard->ExtendKeyDefTab[21].iScanCode = SCANCODE_Y;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[21].rc),83,17,93,32);
	pKeyBoard->ExtendKeyDefTab[22].iScanCode = SCANCODE_U;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[22].rc),95,17,105,32);
	pKeyBoard->ExtendKeyDefTab[23].iScanCode = SCANCODE_I;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[23].rc),107,17,117,32);
	pKeyBoard->ExtendKeyDefTab[24].iScanCode = SCANCODE_O;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[24].rc),119,17,129,32);
	pKeyBoard->ExtendKeyDefTab[25].iScanCode = SCANCODE_P;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[25].rc),131,17,141,32);
	pKeyBoard->ExtendKeyDefTab[26].iScanCode = SCANCODE_BRACKET_LEFT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[26].rc),143,17,153,32);
	pKeyBoard->ExtendKeyDefTab[27].iScanCode = SCANCODE_BRACKET_RIGHT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[27].rc),155,17,165,32);
	pKeyBoard->ExtendKeyDefTab[28].iScanCode = SCANCODE_BACKSLASH;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[28].rc),167,17,177,32);

	pKeyBoard->ExtendKeyDefTab[59].iScanCode = SCANCODE_EXTPAD;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[59].rc),179,17,183,32);

	pKeyBoard->ExtendKeyDefTab[29].iScanCode = SCANCODE_CAPSLOCK;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[29].rc),5,34,27,49);
	pKeyBoard->ExtendKeyDefTab[30].iScanCode = SCANCODE_A;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[30].rc),29,34,39,49);
	pKeyBoard->ExtendKeyDefTab[31].iScanCode = SCANCODE_S;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[31].rc),41,34,51,49);
	pKeyBoard->ExtendKeyDefTab[32].iScanCode = SCANCODE_D;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[32].rc),53,34,63,49);
	pKeyBoard->ExtendKeyDefTab[33].iScanCode = SCANCODE_F;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[33].rc),65,34,75,49);
	pKeyBoard->ExtendKeyDefTab[34].iScanCode = SCANCODE_G;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[34].rc),77,34,87,49);
	pKeyBoard->ExtendKeyDefTab[35].iScanCode = SCANCODE_H;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[35].rc),89,34,99,49);
	pKeyBoard->ExtendKeyDefTab[36].iScanCode = SCANCODE_J;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[36].rc),101,34,111,49);
	pKeyBoard->ExtendKeyDefTab[37].iScanCode = SCANCODE_K;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[37].rc),113,34,123,49);
	pKeyBoard->ExtendKeyDefTab[38].iScanCode = SCANCODE_L;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[38].rc),125,34,135,49);
	pKeyBoard->ExtendKeyDefTab[39].iScanCode = SCANCODE_SEMICOLON;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[39].rc),137,34,147,49);
	pKeyBoard->ExtendKeyDefTab[40].iScanCode = SCANCODE_APOSTROPHE;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[40].rc),149,34,159,49);
	pKeyBoard->ExtendKeyDefTab[41].iScanCode = SCANCODE_ENTER;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[41].rc),161,34,183,49);


	pKeyBoard->ExtendKeyDefTab[42].iScanCode = SCANCODE_LEFTSHIFT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[42].rc),5,51,33,66);
	pKeyBoard->ExtendKeyDefTab[43].iScanCode = SCANCODE_Z;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[43].rc),35,51,45,66);
	pKeyBoard->ExtendKeyDefTab[44].iScanCode = SCANCODE_X;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[44].rc),47,51,57,66);
	pKeyBoard->ExtendKeyDefTab[45].iScanCode = SCANCODE_C;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[45].rc),59,51,69,66);
	pKeyBoard->ExtendKeyDefTab[46].iScanCode = SCANCODE_V;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[46].rc),71,51,81,66);
	pKeyBoard->ExtendKeyDefTab[47].iScanCode = SCANCODE_B;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[47].rc),83,51,93,66);
	pKeyBoard->ExtendKeyDefTab[48].iScanCode = SCANCODE_N;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[48].rc),95,51,105,66);
	pKeyBoard->ExtendKeyDefTab[49].iScanCode = SCANCODE_M;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[49].rc),107,51,117,66);
	pKeyBoard->ExtendKeyDefTab[50].iScanCode = SCANCODE_COMMA;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[50].rc),119,51,129,66);
	pKeyBoard->ExtendKeyDefTab[51].iScanCode = SCANCODE_PERIOD;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[51].rc),131,51,141,66);
	pKeyBoard->ExtendKeyDefTab[52].iScanCode = SCANCODE_SLASH;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[52].rc),143,51,153,66);
	pKeyBoard->ExtendKeyDefTab[53].iScanCode = SCANCODE_RIGHTSHIFT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[53].rc),155,51,183,66);

	pKeyBoard->ExtendKeyDefTab[54].iScanCode = SCANCODE_LEFTCONTROL;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[54].rc),5,68,21,83);
	pKeyBoard->ExtendKeyDefTab[55].iScanCode = SCANCODE_LEFTALT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[55].rc),23,68,39,83);
	pKeyBoard->ExtendKeyDefTab[56].iScanCode = SCANCODE_SPACE;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[56].rc),41,68,147,83);
	pKeyBoard->ExtendKeyDefTab[57].iScanCode = SCANCODE_RIGHTALT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[57].rc),149,68,165,83);
	pKeyBoard->ExtendKeyDefTab[58].iScanCode = SCANCODE_RIGHTCONTROL;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[58].rc),167,68,183,83);

	
	pKeyBoard->ExtendKeyDefTab[60].iScanCode = SCANCODE_HOME;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[60].rc),209,0,219,15);
	pKeyBoard->ExtendKeyDefTab[61].iScanCode = SCANCODE_END;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[61].rc),209,17,219,32);
	pKeyBoard->ExtendKeyDefTab[62].iScanCode = SCANCODE_INS;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[62].rc),197,0,207,15);
	pKeyBoard->ExtendKeyDefTab[63].iScanCode = SCANCODE_DEL;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[63].rc),197,17,207,32);
	pKeyBoard->ExtendKeyDefTab[64].iScanCode = SCANCODE_PAGEUP;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[64].rc),221,0,231,15);
	pKeyBoard->ExtendKeyDefTab[65].iScanCode = SCANCODE_PAGEDOWN;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[65].rc),221,17,231,32);
	pKeyBoard->ExtendKeyDefTab[66].iScanCode = SCANCODE_LEFT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[66].rc),197,68,207,83);
	pKeyBoard->ExtendKeyDefTab[67].iScanCode = SCANCODE_RIGHT;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[67].rc),221,68,231,83);
	pKeyBoard->ExtendKeyDefTab[68].iScanCode = SCANCODE_UP;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[68].rc),209,51,219,66);
	pKeyBoard->ExtendKeyDefTab[69].iScanCode = SCANCODE_DOWN;
	SetRect(&(pKeyBoard->ExtendKeyDefTab[69].rc),209,68,219,83);




	//read skb bmp

	iSize = SKBWIN_WIDTH*SKBWIN_HEIGHT*Gal_iBytesDataType;
	pData = (char*)bmalloc(SKB_LAYOUT_NUMBER*iSize);
	if(!pData)
		return false;
	pKeyBoard->pData = pData;
	//standard
	strcpy(pString,pEnvStr);
	strcat(pString,SKBBMP_LAYOUT_STANDARD);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,SKBWIN_WIDTH,SKBWIN_HEIGHT,pData);
	CLOSE(fd);
	pData+=iSize;

	//shifted
	strcpy(pString,pEnvStr);
	strcat(pString,SKBBMP_LAYOUT_SHIFTED);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,SKBWIN_WIDTH,SKBWIN_HEIGHT,pData);
	CLOSE(fd);
	pData+=iSize;

	//standard extend
	strcpy(pString,pEnvStr);
	strcat(pString,SKBBMP_LAYOUT_STANDARD_EXTEND);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,SKBWIN_WIDTH,SKBWIN_HEIGHT,pData);
	CLOSE(fd);
	pData+=iSize;

	//shifted extend
	strcpy(pString,pEnvStr);
	strcat(pString,SKBBMP_LAYOUT_SHIFTED_EXTEND);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,SKBWIN_WIDTH,SKBWIN_HEIGHT,pData);
	CLOSE(fd);
	pData+=iSize;

	//handwrite
	strcpy(pString,pEnvStr);
	strcat(pString,SKBBMP_LAYOUT_HANDWRITE);
	fd = OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	ReadBmpToBuf(fd,SKBWIN_WIDTH,SKBWIN_HEIGHT,pData);
	CLOSE(fd);

	return true;

}


static void 
DestroyKeyBoardData(
	HWND hWnd
)
{
	PLGUIKeyBoard pKeyBoard;
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	pKeyBoard = (PLGUIKeyBoard)(pWin->dwAddData);
	if(!pKeyBoard)
		return;
	bfree(pKeyBoard->pData);
	bfree(pKeyBoard);
}

