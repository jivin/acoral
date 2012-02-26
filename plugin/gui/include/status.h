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

#ifndef _STATUS_H
#define _STATUS_H

#ifdef __cplusplus
extern "C" {
#endif



#define IME_LEFT		1
#define IME_RIGHT	19
#define IME_TOP		1
#define IME_BOTTOM	18

#define SKB_LEFT		20
#define SKB_RIGHT	38
#define SKB_TOP		1
#define SKB_BOTTOM	18

#define IMEBMP_STATUS_HANDWRITE	"statusbar/ime_handwrite.bmp"
#define IMEBMP_STATUS_PINYIN		"statusbar/ime_pinyin.bmp"
#define IMEBMP_STATUS_ENGLISH		"statusbar/ime_eng.bmp"
#define IMEBMP_STATUS_CHAR		"statusbar/ime_char.bmp"

#define SKBBMP_STATUS_SHOW		"statusbar/skb_pressed.bmp"
#define SKBBMP_STATUS_HIDE		"statusbar/skb_nopressed.bmp"


BOOL 
RegisterStatusBarControl();

LRESULT 
StatusBarControlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);



#ifdef __cplusplus
}
#endif

#endif



