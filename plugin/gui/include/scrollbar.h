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

#ifndef _SCROLLBAR_H
#define _SCROLLBAR_H

#ifdef __cplusplus
extern "C" {
#endif

#define SB_CTL  0// Sets the parameters of a scroll bar control. The hwnd parameter must be the handle to the scroll bar control.
#define SB_HORZ 1// Sets the parameters of the given window's standard horizontal scroll bar.
#define SB_VERT 2//Sets the parameters of the given window's standard vertical scroll bar.

//message sent to parent
#define SB_LINEUP				0x0001
#define SB_LINEDOWN				0x0002
#define SB_PAGEUP				0x0003
#define SB_PAGEDOWN				0x0004
#define SB_THUMBTRACK			0x0005
#define SB_LINELEFT				SB_LINEUP
#define SB_LINERIGHT			SB_LINEDOWN

#define MIN_SLIDERBAR_LENGTH	6
//control status 
#define SS_UPLEFT_PRESSDOWN		0x00100000L
#define SS_DOWNRIGHT_PRESSDOWN	0x00200000L
#define SS_BAR_PRESSDOWN		0x00400000L

#define SIF_ALL			0x000000ff
#define SIF_RANGE		0x00000001
#define SIF_PAGE		0x00000002
#define SIF_POS			0x00000004
#define SIF_TRACKPOS	0x00000008


BOOL 
RegisterScrollBarControl();


LRESULT 
ScrollBarCtrlProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


#ifdef __cplusplus
}
#endif
#endif

