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

#ifndef _TABCONTROL_H
#define _TABCONTROL_H

#ifdef __cplusplus                     
extern "C" {
#endif


#define ITEM_HEADER_HEIGHT	20
#define ITEM_HEADER_WIDTH 	20
#define CORNER_LENGTH		2

typedef struct tagTabControlItem{
	int id;				
	char* lpszCaption;
	PBITMAP	pBitmap;
	struct tagTabControlItem* pNext;
} TabControlItem;
typedef TabControlItem* PTabControlItem;


typedef struct tagTabControl{
	int iTotalItem;
	int iCurItem;
	PTabControlItem pItemHead;
	PTabControlItem pCurItem;
} TabControl;
typedef TabControl* PTabControl;



BOOL 
RegisterTabControl();


LRESULT 
TabControlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam,
	LPARAM lParam
);

#ifdef __cplusplus
}
#endif 


#endif 



