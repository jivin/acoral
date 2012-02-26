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

#ifndef _LGUI_H_
#define _LGUI_H_
#ifdef __cplusplus
extern "C" {
#endif


#define MAX_VALUE_SIZE	256
#define DESKICON_LINES_PER_PAGE 5
#define DESKICON_NUMBER_PER_LINE 3


typedef struct tagDeskIconNode {
	char lpszComment[MAX_VALUE_SIZE];		//comment
	char lpszName[MAX_VALUE_SIZE];			//name
	char lpszIconName[MAX_VALUE_SIZE];		//icon file name 
	char lpszBinName[MAX_VALUE_SIZE];		//bin file name
	struct tagDeskIconNode* pNext;
	void (*entry)(void *args);
} DeskIconNode;
typedef DeskIconNode* PDeskIconNode;

//application class description
typedef struct tagDeskIconClass{
	int id;//class id
	char lpszClassName[MAX_VALUE_SIZE];	//class name
	char lpszIconName[MAX_VALUE_SIZE];	//class name on the tab item
	char lpszPath[MAX_VALUE_SIZE];		//path
	
	int iScrollPos;
	int iScrollMax;
	int iScrollMin;
	int iScrollPage;

	int iTotalIcon;
	PDeskIconNode pHead;
	PDeskIconNode pTail;
	HWND hWndHead;
	HWND hWndTail;
	struct tagDeskIconClass* pNext;
} DeskIconClass;
typedef DeskIconClass* PDeskIconClass;


typedef struct tagDeskIconInfo{
	PDeskIconClass pHead;
	PDeskIconClass pTail;
	int iTotal;
} DeskIconInfo;
typedef DeskIconInfo* PDeskIconInfo;




//MouseDown
BOOL 
srvRawMouseDown(
	WPARAM wParam,
	LPARAM lParam
);


//MouseUp
BOOL 
srvRawMouseUp(
	WPARAM wParam,
	LPARAM lParam
);


//MouseMove
BOOL 
srvRawMouseMove(
	WPARAM wParam,
	LPARAM lParam
);


BOOL 
LaunchApp(
	void(*)(void *args)
);


LRESULT 
DefWndProcDesktop(
	HWND hWnd, 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam
);




BOOL 
GetDeskIconInfo();


void 
GetDeskScrollInfo(
	int* iMax, 
	int* iPage, 
	int iTotalIcon
);


BOOL 
IsDeskIconInfoFile(
	char* pString
);


void 
DestroyDeskIconInfo();


BOOL 
AddNodeToDeskIconClass(
	PDeskIconNode pNode,
	PDeskIconClass pClass
);


void 
DeleteNodeFromDeskIconClass(
	char* lpszName,
	PDeskIconClass pClass
);


BOOL 
CreateDeskIcon();


void 
ProDeskIconScroll(
	HWND hWndScrollBar,
	HWND hWndTab
);


void 
HideClassDeskIcon(
	PDeskIconClass pClass
);


void 
ShowClassDeskIcon(
	PDeskIconClass pClass
);


PDeskIconClass 
GetCurrentClass(
	HWND hWndTab
);


int 
GenDeskIconID();


void DeskIconClicked(
	WORD id
);


HWND 
GetDeskIconHandle(
	int id
);


BOOL 
AddStartMenuItems();


void 
ProcessStartMenuClicked();


void 
ProcessStartMenuItemClicked(
	WPARAM wParam, 
	LPARAM lParam
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



