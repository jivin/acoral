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

#ifndef _CARET_H
#define _CARET_H

#ifdef __cplusplus                     
extern "C" {
#endif

typedef struct _CARETINFO {
    int     x;              // position of caret
    int     y;
    void*   pNormal;        // normal bitmap.
    void*   pXored;         // bit-Xored bitmap.
    
    PBITMAP pBitmap;        // user defined caret bitmap.
    int     nWidth;         // size of caret
    int     nHeight;
    int     nBytesNr;       // number of bitmap bytes.
    int     nEffWidth;      // effective width
    int     nEffHeight;     // effective height;

    BOOL    fBlink;         // does blink?
    BOOL    fShow;          // show or hide currently.

    HWND    hOwner;         // the window owns the caret.
    UINT    uTime;          // the blink time.
}CARETINFO;
typedef CARETINFO* PCARETINFO;


BOOL GUIAPI 
CreateCaret (
	HWND hWnd, 
	PBITMAP pBitmap, 
	int nWidth, 
	int nHeight
);

BOOL GUIAPI 
DestroyCaret(
	HWND hWnd
);

UINT GUIAPI 
GetCaretBlinkTime(
	HWND hWnd
);

BOOL GUIAPI 
SetCaretBlinkTime(
	HWND hWnd, 
	UINT uTime
);

BOOL 
BlinkCaret(
	HWND hWnd
);

BOOL GUIAPI 
ShowCaret(
	HWND hWnd
);

BOOL GUIAPI 
HideCaret(
	HWND hWnd
);

void 
GetCaretBitmaps(
	PCARETINFO pCaretInfo
);

BOOL GUIAPI
SetCaretPos(
	HWND hWnd, 
	int x, 
	int y
);

BOOL GUIAPI 
ChangeCaretSize(
	HWND hWnd, 
	int newWidth, 
	int newHeight
);

// Get Caret Position
BOOL GUIAPI 
GetCaretPos(
	HWND hWnd, 
	PPOINT pPt
);

#ifdef __cplusplus
}
#endif 


#endif 





