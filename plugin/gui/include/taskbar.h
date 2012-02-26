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

#ifndef _TASKBAR_H
#define _TASKBAR_H

#ifdef __cplusplus                     
extern "C" {
#endif


//number of items
#define TASKBAR_ITEMS_NUMBER					5

#define TASKBAR_BUTTONS_HEIGHT				18	//height of button


#define TASKBAR_OBJID_STARTBUTTON			0
#define TASKBAR_STARTBUTTONBMP_NUMBER		2
#define TASKBAR_STARTBUTTONBMP_NOPRESSED	"/pics/sys/taskbar/nopres.bmp"
#define TASKBAR_STARTBUTTONBMP_PRESSED		"/pics/sys/taskbar/pres.bmp"
#define TASKBAR_STARTBUTTON_LEFT				1
#define TASKBAR_STARTBUTTON_WIDTH			48


#define TASKBAR_OBJID_SKB						1
#define TASKBAR_SKBBMP_NUMBER				2
#define TASKBAR_SKBBMP_PRESSED				"/pics/sys/taskbar/k_pres.bmp"
#define TASKBAR_SKBBMP_NOPRESSED				"/pics/sys/taskbar/k_npres.bmp"
#define TASKBAR_SKBSWITCH_LEFT				144
#define TASKBAR_SKBSWITCH_WIDTH				18



#define TASKBAR_OBJID_DBC						2
#define TASKBAR_DBCBMP_NUMBER				2	
#define TASKBAR_DBCBMP_NOPRESSED				"/pics/sys/taskbar/ime_sbc.bmp"
#define TASKBAR_DBCBMP_PRESSED				"/pics/sys/taskbar/ime_dbc.bmp"
#define TASKBAR_DBCSWITCH_LEFT				162
#define TASKBAR_DBCSWITCH_WIDTH				18


#define TASKBAR_OBJID_IME						3
#define TASKBAR_IMEBMP_NUMBER				2	
#define TASKBAR_IMEBMP_ENG					"/pics/sys/taskbar/ime_eng.bmp"
#define TASKBAR_IMEBMP_HANDWRITE				"/pics/sys/taskbar/hdwr.bmp"
#define TASKBAR_IMESWITCH_LEFT				180
#define TASKBAR_IMESWITCH_WIDTH				18


#define TASKBAR_OBJID_CLOCK					4
#define TASKBAR_CLOCKBMP_NUMBER				1	
#define TASKBAR_CLOCKBMP						"/pics/sys/taskbar/clkbk.bmp"
#define TASKBAR_CLOCKBOX_LEFT					198
#define TASKBAR_CLOCKBOX_WIDTH				40


typedef struct tagTaskBarItem{
	RECT rect;
	BOOL bShow;
	int id;
	int iTotalPictureNumber;
	int iCurValue;
	void* pData;
	struct tagTaskBarItem* pNext;
} TaskBarItem;
typedef TaskBarItem* PTaskBarItem;

typedef struct tagTaskBar{
	int iTotalItem;
	PTaskBarItem pItemHead;
	PTaskBarItem pItemIme;	//value of ime item can be changed by ime install program
} TaskBar;
typedef TaskBar* PTaskBar;


BOOL 
RegisterTaskBarControl();


LRESULT 
TaskBarControlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);

#ifdef __cplusplus
}
#endif 


#endif 


