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

#ifndef _REGCLASS_H_
#define _REGCLASS_H_

#ifdef __cplusplus
extern "C" {
#endif



typedef struct tagWNDCLASSEX {
	UINT					cbSize;				//size of this structure
	UINT					style;				//style of application window
	WNDPROC					lpfnWndProc;		//callback function pointer
	HINSTANCE				hInstance;			//
	HICON					hIcon;				//
	HICON					hIconSm;			//
	HCURSOR					hCursor;			//
	HBRUSH					hbrBackground;		//
	char*					lpszMenuName;		//
	char*					lpszClassName;		//
	DWORD					cbClsExtra;			//reserved
	DWORD					cbWndExtra;			//reserved
	struct tagWNDCLASSEX*	pNext;
} WNDCLASSEX;
typedef	WNDCLASSEX* PWNDCLASSEX;

 

BOOL GUIAPI
RegisterClass(
	WNDCLASSEX* lpwcx
);


//get register class by class name
PWNDCLASSEX GUIAPI
GetRegClass(
	char* lpszClassName
);

void 
DestroyRegWndTable();

WNDPROC
GetWndProc(
	HWND hWnd
);

void 
RegisterServerControls();

void 
RegisterClientControls();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 


