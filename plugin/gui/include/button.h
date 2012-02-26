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

#ifndef _BUTTON_H
#define _BUTTON_H

#ifdef __cplusplus                     
extern "C" {
#endif

//  |----1---||----2---||----3---||----4---||---5----||---6----||---7----||---8----|
//	|....control type .||status  ||.wintype||....winstyle......||...winstatus......|



#define BS_ANIMATIONBUTTON  	0x80000000L
#define BS_TEXT         			0x40000000L// PushButton
#define BS_BITMAP      			0x20000000L// Specifies that the button displays a bitmap.
#define BS_GROUP				0x10000000L// RadioBox group

#define BS_TYPEMASK         		0x0f000000L
#define BS_PUSHBUTTON       		0x00000000L
#define BS_CHECKBOX         		0x01000000L
#define BS_AUTOCHECKBOX		0x02000000L//check state automatically toggles between checked and unchecked
#define BS_RADIOBUTTON      		0x03000000L
#define BS_AUTORADIOBUTTON  	0x04000000L//automatically sets the button's check state to checked and sets the check state for all other buttons in the same group to unchecked.

#define BS_STATUSMASK			0x00f00000L
#define BS_BUTTON_PRESSDOWN	0x00100000L
#define BS_BUTTON_CHECKED		0x00100000L


#define CHECKED					0X00000000L
#define UNCHECKED				0x00000001L



BOOL 
RegisterButtonControl();


LRESULT 
ButtonCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);
#ifdef __cplusplus
}
#endif 


#endif 





