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
#include "../include/listbox.h"
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
#include "../include/scrollbar.h"



//==================================
static BOOL
lstIsNofity(
	HWND hWnd
);


static BOOL 
lstInitListBoxData(
	LISTBOXDATA* pData, 
	int len
);


static void 
lstListBoxCleanUp(
	LISTBOXDATA* pData
);


static void 
lstResetListBoxContent(
	PLISTBOXDATA pData
);


static PLISTBOXITEM 
lstAllocItem (
	PLISTBOXDATA pData
);


static void 
lstFreeItem(
	PLISTBOXDATA pData, 
	PLISTBOXITEM plbi
);


static int 
lstAddNewItem(
	DWORD dwStyle,
	PLISTBOXDATA pData, 
	PLISTBOXITEM newItem, 
	int pos
);


static PLISTBOXITEM 
lstRemoveItem(
	PLISTBOXDATA pData, 
	int* pos
);


static PLISTBOXITEM 
lstGetItem(
	PLISTBOXDATA pData, 
	int pos
);


static int 
lstFindItem(
	PLISTBOXDATA pData, 
	int start, 
	char* key, 
	BOOL bExact
);


static BOOL 
lstInvalidateUnderItem(
	HWND hwnd, 
	PLISTBOXDATA pData, 
	int pos
);


static void 
lstGetItemsRect(
	PLISTBOXDATA pData,
	int start, 
	int end, 
	RECT* prc
);


static void 
lstSetVScrollInfo(
	HWND hwnd, 
	PLISTBOXDATA pData, 
	BOOL fRedraw
);

//==============================================================



BOOL 
RegisterListBoxControl()
{ 
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)ListBoxCtrlProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;
	wcex.hIcon			= 0;
	wcex.hCursor			= 0;
	wcex.hbrBackground	= CreateSolidBrush(RGB(198,239,229));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "listbox";
	wcex.hIconSm		= 0;

	RegisterClass(&wcex);
	return true;
}


LRESULT 
ListBoxCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
)
{
	RECT rc,rcItem,rcItemHilighted;
	HDC hDC;

	PAINTSTRUCT ps;
	int iWidth,iHeight;

	HBRUSH hBrush;
	COLORREF crColor;
	PWNDCLASSEX pWndClass;
	LRESULT res;

	PLISTBOXDATA pData;
	PLISTBOXITEM removed;

	PWindowsTree pWin;
	int delete;
	DWORD dwStyle;
	char* string = NULL;
	char* pFileName;
	int iCounter,iTotal;

	PLISTBOXITEM plbi = NULL;

	PLISTBOXITEM newItem;
	int pos;
	int x,y;

	pWin=(PWindowsTree)hWnd;
	switch(iMsg)	{
	case LMSG_CREATE:
		pData = (PLISTBOXDATA)malloc(sizeof(LISTBOXDATA));
		if(!pData)
			return false;
		memset(pData,0, sizeof(LISTBOXDATA));
		pWin->dwData = (DWORD)pData;
		//dwAddData save background bmp file path
		if(lParam){
			pFileName = (char*)malloc(strlen((char*)lParam)+1);
			strcpy(pFileName,(char*)lParam);
			pWin->dwAddData = (DWORD)pFileName;
		}
		if(!lstInitListBoxData(pData,DEF_LB_BUFFER_LEN))
			return false;
		break;
	case LMSG_ACTIVEWINDOW:
		break;
	case LMSG_COMMAND:
		break;
	case LMSG_PENDOWN:
	{
		int oldSel,hit;

		pData = (PLISTBOXDATA)(pWin->dwData);
		GetClientRect(hWnd,&rc);
		ScreenToClientRect(hWnd,&rc);

		x = (int)wParam;
		y = (int)lParam;

		ScreenToClient(hWnd,&x,&y);
		hit = y / pData->itemHeight;
		hit += pData->itemTop;

		if (hit >= pData->itemCount)
		    break;
		oldSel = pData->itemHilighted;

		iTotal =pData->itemVisibles;
		for(iCounter=0;iCounter<iTotal;iCounter++){

			plbi=lstGetItem(pData,pData->itemTop + iCounter);
			lstGetItemsRect(pData,pData->itemTop+iCounter,pData->itemTop+iCounter,&rcItem);
			rcItem.left=rc.left;
			rcItem.right =rc.right;
			if(PtInRect(&rcItem,x,y)){
				lstGetItemsRect(pData,pData->itemHilighted,pData->itemHilighted,&rcItemHilighted);
				rcItemHilighted.left=rc.left;
				rcItemHilighted.right =rc.right;
				pData->itemHilighted = pData->itemTop + iCounter;
				InvalidateRect(hWnd,&rcItemHilighted,false);
				InvalidateRect(hWnd,&rcItem,false);
				break;
			}
		}
		if (oldSel != hit && lstIsNofity(hWnd))
		    NotifyParent(hWnd, LBN_SELCHANGE);
		}
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

		((BRUSH*)hBrush)->crBrushColor=RGB_SYSTEMHIBRIGHT;

		res=DefWindowProc(hWnd, iMsg, wParam, lParam);
		((BRUSH*)hBrush)->crBrushColor=crColor;
		return res;

	case LMSG_ENABLE:
		break;
	case LMSG_PAINT:
		ps.bPaintDirect=false;
		hDC=BeginPaint(hWnd, &ps);
		if(!hDC){
			return true;
		}
		//GetClientRect(hWnd,&rc);
		scrGetClientRect(hWnd,&rc);
		ScreenToClientRect(hWnd,&rc);
		if(pWin->dwAddData)
			ShowBitmap(hDC,0,0,(char*)(pWin->dwAddData));
		iWidth	=rc.right - rc.left + 1;
		iHeight	=rc.bottom - rc.top + 1;

		pData = (PLISTBOXDATA)(pWin->dwData);
		if(!pData)
			return false;
		iTotal =pData->itemVisibles;
		//printf("%d\n",pData->itemTop);
		for(iCounter=0;iCounter<iTotal;iCounter++){

			plbi=lstGetItem(pData,pData->itemTop + iCounter);
			lstGetItemsRect(pData,pData->itemTop+iCounter,pData->itemTop+iCounter,&rcItem);
			rcItem.left=rc.left;
			rcItem.right =rc.right;
			if(iCounter + pData->itemTop == pData->itemHilighted)
				FillRectangle(hDC,rcItem.left,rcItem.top,rcItem.right,rcItem.bottom,RGB_SYSTEMBRIGHT,RGB_SYSTEMBRIGHT);
			DrawText(hDC,plbi->key,strlen(plbi->key),&rcItem,DT_CENTER);
		}

		EndPaint(hWnd, &ps);
		break;
	case LMSG_DESTROY:
		pData = (PLISTBOXDATA)(pWin->dwData);
		lstListBoxCleanUp (pData);
		free (pData);
		if(pWin->dwAddData)
			free((char*)(pWin->dwAddData));
		break;
        case LB_RESETCONTENT:
		pData = (PLISTBOXDATA)(pWin->dwData);
		lstResetListBoxContent (pData);
		winInvalidateRect (hWnd, NULL, TRUE);
		break;
        case LB_ADDSTRING:
        case LB_INSERTSTRING:
		string = (char*)lParam;
		if (string == NULL || string [0] == '\0')
			return false;

		pData = (PLISTBOXDATA)(pWin->dwData);
		newItem = lstAllocItem (pData);
		newItem->key = strdup (string);
		if (!newItem->key) {
			lstFreeItem (pData, newItem);
			printerror("alloc memory error!");
			return false;
		}
		newItem->dwFlags = LBIF_NORMAL;
		newItem->dwAddData = 0L;

		if (iMsg == LB_ADDSTRING)
			pos = lstAddNewItem (dwStyle, pData, newItem, -1);
		else
			pos = lstAddNewItem (dwStyle, pData, newItem, (int)wParam);

		//modify visible items
		GetClientRect(hWnd,&rc);
		iHeight	=rc.bottom - rc.top + 1;
		if(pData->itemVisibles < iHeight/pData->itemHeight)
			pData->itemVisibles +=1;

		lstInvalidateUnderItem (hWnd, pData, pos);

		lstSetVScrollInfo (hWnd, pData, TRUE);
		return true;

        case LB_DELETESTRING:
		delete = (int)wParam;
		pData = (PLISTBOXDATA)(pWin->dwData);
		removed = lstRemoveItem (pData, &delete);
		if (removed){
			free (removed->key);
			lstFreeItem (pData, removed);

			pData->itemCount --;	

			if (pData->itemTop != 0
				&& pData->itemCount <= pData->itemVisibles){
				pData->itemTop = 0;
				winInvalidateRect (hWnd, NULL, TRUE);
			}
			else{
				lstInvalidateUnderItem (hWnd, pData, delete);
				if (delete <= pData->itemTop){
					pData->itemTop --;
				if (pData->itemTop < 0)
					pData->itemTop = 0;
				}
			}

			if (pData->itemHilighted >= pData->itemCount){
				pData->itemHilighted = pData->itemCount - 1;
				if (pData->itemHilighted < 0)
					pData->itemHilighted = 0;
			}

			if (pData->itemHilighted < pData->itemTop)
				pData->itemHilighted = pData->itemTop;
			if (pData->itemHilighted > ITEM_BOTTOM (pData))
				pData->itemHilighted = ITEM_BOTTOM (pData);

			lstSetVScrollInfo (hWnd, pData, TRUE);
			return 0;
		}
		else
			return false;
		break;
        case LB_FINDSTRING:
		if( *(char*)lParam == '\0' )
			return false;
		pData = (PLISTBOXDATA)(pWin->dwData);
		return lstFindItem(pData, (int)wParam, (char*)lParam, FALSE);
        case LB_FINDSTRINGEXACT:
		if( *(char*)lParam == '\0' )
			return false;
		pData = (PLISTBOXDATA)(pWin->dwData);
		return lstFindItem(pData, (int)wParam, (char*)lParam, TRUE);
	case LB_GETCOUNT:
		return pData->itemCount;
	case LB_SETCURSEL:
	{
		int new = (int)wParam;
		int old, newTop;

		pData = (PLISTBOXDATA)(pWin->dwData);
		if (new < 0 || new > pData->itemCount - 1)
			return LB_ERR;

		old = pData->itemHilighted;
		if (new >= 0 && new != old){
			if (pData->itemCount - new >= pData->itemVisibles)
				newTop = new;
			else
				newTop = max (pData->itemCount - pData->itemVisibles, 0);
			pData->itemTop = newTop;
			pData->itemHilighted = new;
			lstSetVScrollInfo (hWnd, pData, TRUE);
		}

		InvalidateRect (hWnd, NULL, TRUE);

		return old;
	}

	case LB_GETCURSEL:
	{
		PLISTBOXITEM plbi;
		int index = 0;

		pData = (PLISTBOXDATA)(pWin->dwData);

		plbi = pData->head;
		while(plbi){
			if (plbi->dwFlags & LBIF_SELECTED)
				return index;
			index ++;
			plbi = plbi->next;
		}
		return LB_ERR;
	}
	case LB_GETTEXT:
	{
		PLISTBOXITEM plbi;

		pData = (PLISTBOXDATA)(pWin->dwData);;
		plbi = lstGetItem (pData, (int)wParam);
		if (plbi)
		strcpy ((char*)lParam, plbi->key);
		else
		return LB_ERR;
	}

	case LB_SETTEXT:
	{
		PLISTBOXITEM plbi;
		char* newStr;

		pData = (PLISTBOXDATA)(pWin->dwData);
		plbi = lstGetItem (pData, (int)wParam);
		if (plbi) {
			newStr = strdup ((const char*)lParam);
			if (newStr) {
				free (plbi->key);
				plbi->key = newStr;
				InvalidateRect (hWnd, NULL, TRUE);
			}
			else
				return LB_ERR;
		}
		else
			return LB_ERR;
		
		break;
	}
	case LMSG_VSCROLL:
		pData = (PLISTBOXDATA)(pWin->dwData);
		switch((int)wParam){
		case SB_LINEUP:
			pData->itemTop-=1;
			break;
		case SB_LINEDOWN:
			pData->itemTop+=1;
			break;
		case SB_THUMBTRACK:
			pData->itemTop = pWin->pVScroll->nPos;
			break;
		}
		winInvalidateRect(hWnd,NULL,true);
		break;

	default:
		return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
	return true;
}



static BOOL 
lstInitListBoxData (
	LISTBOXDATA* pData, 
	int len
)
{
	int i;
	PLISTBOXITEM plbi;

	pData->itemHeight = DEF_LB_ITEM_HEIGHT;//default list box item height
	pData->itemHilighted = 0;

	// init item buffer.
	if (!(pData->buffStart = malloc (len * sizeof (LISTBOXITEM))))
		return FALSE;

	pData->buffLen = len;
	pData->buffEnd = pData->buffStart + len;
	pData->freeList = pData->buffStart;

	plbi = pData->freeList;
	//make a free list link table
	for (i = 0; i < len - 1; i++) {
		plbi->next = plbi + 1;
		plbi ++;
	}
	plbi->next = NULL;

	return TRUE;
}


static void
lstListBoxCleanUp(
	LISTBOXDATA* pData
)
{
	PLISTBOXITEM plbi;
	PLISTBOXITEM next;

	plbi = pData->head;
	while (plbi) {
		free (plbi->key);
		next = plbi->next;
		if (plbi < pData->buffStart || plbi > pData->buffEnd)
			free (plbi);
		plbi = next;
	}
	free (pData->buffStart);
}


static void 
lstResetListBoxContent (
	PLISTBOXDATA pData
)
{
	int i;
	PLISTBOXITEM plbi, next;

	pData->itemCount = 0;
	pData->itemTop = 0;
	pData->itemHilighted = 0;

	plbi = pData->head;
	while (plbi) {
		free (plbi->key);
		next = plbi->next;
		if (plbi < pData->buffStart || plbi > pData->buffEnd)
			free (plbi);
		plbi = next;
	}

	pData->head = NULL;
	pData->freeList = pData->buffStart;

	plbi = pData->freeList;
	for (i = 0; i < pData->buffLen - 1; i++) {
		plbi->next = plbi + 1;
		plbi ++;
	}
	plbi->next = NULL;
	return;
}


static PLISTBOXITEM 
lstAllocItem (
	PLISTBOXDATA pData
)
{
	PLISTBOXITEM plbi;
	if (pData->freeList) {
		plbi = pData->freeList;
		pData->freeList = plbi->next;
	}
	else
		plbi = (PLISTBOXITEM) malloc (sizeof (LISTBOXITEM));

	return plbi;
}


static void 
lstFreeItem (
	PLISTBOXDATA pData, 
	PLISTBOXITEM plbi
)
{
	if (plbi < pData->buffStart || plbi > pData->buffEnd)//不在预分配区
		free (plbi);
	else {
		plbi->next = pData->freeList;
		pData->freeList = plbi;
	}
}
//add a new item
//dwStyle: sortable
//pData  : 
//newItem: 
//pos    : position(pos<0 insert to begin of listbox)

static int 
lstAddNewItem (
	DWORD dwStyle,
	PLISTBOXDATA pData, 
	PLISTBOXITEM newItem, 
	int pos
)
{
	PLISTBOXITEM plbi;
	PLISTBOXITEM insPosItem = NULL;
	int insPos = 0;

	newItem->next = NULL;

	if (!pData->head)
		insPosItem = NULL;
	else if (dwStyle & LBS_SORT) {
		plbi = pData->head;
		if (strcmp (newItem->key, plbi->key) < 0) {
			insPosItem = NULL;
			insPos = 0;
		}
		else{
			while (plbi->next) {
				if (strcmp (newItem->key, plbi->next->key) <= 0)
					break;

				plbi = plbi->next;
				insPos ++;
			}
			insPosItem = plbi;
		}
	}
	else {
		plbi = pData->head;
		if (pos < 0) {
			while (plbi->next) {
				plbi = plbi->next;
				insPos ++;
			}
			insPosItem = plbi;
		}
		else if (pos > 0) {
			int index = 0;
			while (plbi->next) {
				if (pos == index)
				break;
				plbi = plbi->next;
				index ++;
				insPos ++;
			}
			insPosItem = plbi;
		}
	}

	if (insPosItem) {
		plbi = insPosItem->next;
		insPosItem->next = newItem;
		newItem->next = plbi;
		insPos ++;
	}
	else {
		plbi = pData->head;
		pData->head = newItem;
		newItem->next = plbi;
	}
	pData->itemCount ++;

	return insPos;
}
static PLISTBOXITEM 
lstRemoveItem (
	PLISTBOXDATA pData, 
	int* pos
)
{
	int index = 0;
	PLISTBOXITEM plbi, prev;

	if (!pData->head)
		return NULL;

	if (*pos < 0) {					//若pos<0,则移除最后一个节点
		prev = pData->head;
		plbi = pData->head;
		while (plbi->next) {
			prev = plbi;
			plbi = plbi->next;
			index ++;
		}

		if (plbi == pData->head) {
			pData->head = pData->head->next;
			*pos = 0;
			return plbi;
		}
		else {
			prev->next = plbi->next;
			*pos = index;
			return plbi;
		}
	}
	else if (*pos == 0) {			//如果Pos==0,则移除最前一个结点
		plbi = pData->head;
		pData->head = plbi->next;
		return plbi;
	}
	else {							//根据实际指定的位置移除
		index = 0;
		prev = pData->head;
		plbi = pData->head;
		while (plbi->next) {
			if (*pos == index)
			break;

			prev = plbi;
			plbi = plbi->next;
			index ++;
		}

		if (plbi == pData->head) {
			pData->head = pData->head->next;
			*pos = 0;
			return plbi;
		}
		else {
			prev->next = plbi->next;
			*pos = index;
			return plbi;
		}
	}

	return NULL;
}

static PLISTBOXITEM 
lstGetItem (
	PLISTBOXDATA pData, 
	int pos
)
{
	int i;
	PLISTBOXITEM plbi;

	plbi = pData->head;
	for (i=0; i < pos && plbi; i++)
		plbi = plbi->next;

	return plbi;
}


static int 
lstFindItem (
	PLISTBOXDATA pData, 
	int start, 
	char* key, 
	BOOL bExact
)
{
	PLISTBOXITEM plbi;
	int keylen = strlen (key);

	if (start >= (pData->itemCount - 1))
		start = 0;
	else if (start < 0)
		start = 0;

	plbi = lstGetItem (pData, start);

	while (plbi){
		if (bExact && (keylen != strlen (plbi->key))) {
			plbi = plbi->next;
			start ++;
			continue;
		}
		if (strncasecmp (key, plbi->key, keylen) == 0)
			return start;
			plbi = plbi->next;
		start ++;
	}

	return LB_ERR;
}


static BOOL
lstInvalidateUnderItem (
	HWND hwnd, 
	PLISTBOXDATA pData, 
	int pos
)
{
	RECT rcInv;

	if (pos > (pData->itemTop + pData->itemVisibles))
		return FALSE;

	if (pos <= pData->itemTop) {
		winInvalidateRect (hwnd, NULL, TRUE);
		return TRUE;
	}
	GetClientRect (hwnd, &rcInv);

	lstGetItemsRect (pData, pos, -1, &rcInv);

	if (rcInv.top < rcInv.bottom)
		winInvalidateRect (hwnd, &rcInv, TRUE);

	return TRUE;
}


static void
lstGetItemsRect (
	PLISTBOXDATA pData,
	int start, 
	int end, 
	RECT* prc
)
{
	if (start < 0)
		start = 0;
	prc->top = (start - pData->itemTop)*pData->itemHeight;

	if (end >= 0)
		prc->bottom = (end - pData->itemTop + 1)*pData->itemHeight - 1;

}


static void 
lstSetVScrollInfo (
	HWND hWnd, 
	PLISTBOXDATA pData, 
	BOOL fRedraw
)
{
	SCROLLINFO si;

	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMax = pData->itemCount - 1;
	si.nMin = 0;
	si.nPage = pData->itemVisibles;
	si.nPos = pData->itemTop;
	SetScrollInfo (hWnd, SB_VERT, &si, fRedraw);
	EnableScrollBar (hWnd, SB_VERT, TRUE);
}


static BOOL
lstIsNofity(
	HWND hWnd
)
{
	PWindowsTree pWin;
	pWin = (PWindowsTree)hWnd;
	if(pWin->dwStyle & LBS_NOTIFY)
		return true;
	else
		return false;
}




