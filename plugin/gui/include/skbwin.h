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

#ifndef _SKBWIN_H
#define _SKBWIN_H

#ifdef __cplusplus
extern "C" {
#endif


	
#define SKBWIN_LEFT 		0
#define SKBWIN_TOP			SCREEN_HEIGHT-102
#define SKBWIN_WIDTH		239
#define SKBWIN_HEIGHT		84





#define SKB_LAYOUT_NUMBER				5

#define SKB_LAYOUT_STANDARD			0
#define SKB_LAYOUT_SHIFTED				1	
#define SKB_LAYOUT_STANDARD_EXTEND	2
#define SKB_LAYOUT_SHIFTED_EXTEND	3
#define SKB_LAYOUT_HANDWRITE			4



#define SKBBMP_LAYOUT_STANDARD			"/pics/sys/skb/mnkb_0.bmp"
#define SKBBMP_LAYOUT_SHIFTED				"/pics/sys/skb/mnkb_1.bmp"
#define SKBBMP_LAYOUT_STANDARD_EXTEND	"/pics/sys/skb/mnkb_2.bmp"
#define SKBBMP_LAYOUT_SHIFTED_EXTEND		"/pics/sys/skb/mnkb_3.bmp"	
#define SKBBMP_LAYOUT_HANDWRITE			"/pics/sys/skb/mnkb_4.bmp"	

//==================================================================================
// subscript values of keyboard switch keys
#define CAPSLOCK_SUB		29
#define LEFT_SHIFT_SUB		42
#define RIGHT_SHIFT_SUB	53
#define LEFT_CTRL_SUB		54
#define RIGHT_CTRL_SUB		58
#define LEFT_ALT_SUB		55
#define RIGHT_ALT_SUB		57
#define EXTEND_SUB			59


typedef struct tagLGUIKeyDefine{
	RECT rc;
	int iScanCode;
} LGUIKeyDefine;


typedef struct tagLGUIKeyBoard{
	int iCurLayout;	
	int iPressed;	
	BOOL bExtend;	
	char* pData;
	LGUIKeyDefine KeyDefTab[NR_KEYNUM - 10];
	LGUIKeyDefine ExtendKeyDefTab[NR_KEYNUM];

} LGUIKeyBoard;

typedef LGUIKeyBoard* PLGUIKeyBoard;


void 
CreateSkbWindow(
	LPRECT lpRect
);


void 
DestroySkbWindow();


BOOL 
RegisterSkbWindow();


LRESULT 
SkbWindowProc(
	HWND hWnd, 
	int iMsg, 
	WPARAM wParam, 
	LPARAM lParam
);


void 
SetSkbLayout(
	int iLayout
);


int  
GetSkbLayout();


void 
ShowSkbWindow();


void 
HideSkbWindow();


#ifdef __cplusplus
}
#endif


#endif




