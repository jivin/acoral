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

#ifndef _LISTBOX_H
#define _LISTBOX_H

#ifdef __cplusplus
extern "C" {
#endif


//the field dwStyle of window 
// 1,2 control type 
// 3    control status
// 4    window type
// 5,6 window style
// 7,8 window status


#define LB_ERR -1


#define LBS_TYPEMASK        	0xf0000000L
#define LBS_NOTIFY			0x10000000L
#define LBS_SORT			0x20000000L
#define LBS_VSCROLL			0x40000000L
#define LBS_STANDARD		0x70000000L


#define LBIF_NORMAL         	0x00100000L
#define LBIF_SELECTED       	0x00200000L
#define LBIF_CHECKED        	0x00300000L


#define DEF_LB_BUFFER_LEN       16
#define DEF_LB_ITEM_HEIGHT		28	//default list box item height


#define ITEM_BOTTOM(x)  (x->itemTop + x->itemVisibles - 1)

typedef struct _LISTBOXITEM
{
    char*   key;					// item sort key
    DWORD   dwFlags;				// item flags
    DWORD   dwData;				// item data
    DWORD   dwAddData;			// item additional data
    struct  _LISTBOXITEM* next;  	// next item
} LISTBOXITEM;
typedef LISTBOXITEM* PLISTBOXITEM;

typedef struct tagLISTBOXDATA
{
	DWORD dwFlags;          // listbox flags

	int itemCount;          	// items count
	int itemTop;            	// start display item
	int itemVisibles;       	// number of visible items

	int itemHilighted;      	// current hilighted item
	int itemHeight;         	// item height

	PLISTBOXITEM head;      // items linked list head

	int buffLen;           		 // buffer length
	PLISTBOXITEM buffStart; // buffer start
	PLISTBOXITEM buffEnd;   // buffer end
	PLISTBOXITEM freeList;  // free list in buffer

} LISTBOXDATA;
typedef LISTBOXDATA* PLISTBOXDATA;


BOOL 
RegisterListBoxControl();


LRESULT 
ListBoxCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


#ifdef __cplusplus
}
#endif


#endif

