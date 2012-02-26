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

#ifndef _GDIBMP_H_
#define _GDIBMP_H_

#ifdef __cplusplus                     
extern "C" {
#endif

BOOL 
winShowBitmapWithTrColor(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
);


BOOL GUIAPI 
ShowBitmapWithTrColor(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
);


BOOL 
ShowBitmapWithTrColorWin(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
);


BOOL 
ShowBitmapWithTrColorMem(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
);


BOOL 
winShowBitmap(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile
);


BOOL GUIAPI 
ShowBitmap(
	HDC hDC,
	int xPos, 
	int yPos, 
	char* pFile
);

BOOL 
ShowBitmapWin(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile
);

BOOL 
ShowBitmapMem(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile
);


BOOL 
OutputBmpObj(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight, 
	HBITMAP hBitmap
);


BOOL 
OutputBmpObjWin(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight, 
	HBITMAP hBitmap
);


BOOL 
OutputBmpObjMem(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight, 
	HBITMAP hBitmap
);


BOOL 
OutputBmpObjWithTrColor(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight, 
	HBITMAP hBitmap,
	COLORREF crColor
);


BOOL 
OutputBmpObjWinWithTrColor(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight,
	HBITMAP hBitmap,
	COLORREF crColor
);


BOOL 
OutputBmpObjMemWithTrColor(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight,
	HBITMAP hBitmap,
	COLORREF crColor
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
