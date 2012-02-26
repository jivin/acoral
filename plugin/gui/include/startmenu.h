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

#ifndef _DESKMENU_H
#define _DESKMENU_H

#ifdef __cplusplus
extern "C" {
#endif



#define STARTMENU_ID_1		1000
#define STARTMENU_ID_2		1001
#define STARTMENU_ID_3		1002
#define STARTMENU_ID_4		1003



#define DEF_MENU_BUFFER_LEN     16	//max number of items in startmenu

#define DEF_MENU_ITEM_HEIGHT	16	//height of each item
#define DEF_MENU_WIDTH			50	//width of menu




#define SIZE_MI_NAME	16					//max width of menu item name
typedef struct tagMENUITEM{
	DWORD		dwStyle;					//item style
	char		lpszName[SIZE_MI_NAME];		//menu name
	int			id;							//menu item ID
	PBITMAP		pBitmap;					//item bitmap handle
} MENUITEM;

typedef MENUITEM* PMENUITEM;

typedef struct tagMENUDATA
{
    DWORD dwFlags;						// menubox flags

    int itemCount;						// items count

    int itemHilighted;					// current hilighted item
    int itemHeight;						// item height

	MENUITEM items[DEF_MENU_BUFFER_LEN];//items head
} MENUDATA;

typedef MENUDATA* PMENUDATA;



void 
CreateStartMenuControl(
	LPRECT lpRect
);


void 
DestroyStartMenuControl();


void 
ShowStartMenuControl();


void 
HideStartMenuControl();


BOOL
RegisterStartMenuControl();


LRESULT 
StartMenuCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


#ifdef __cplusplus
}
#endif

#endif
