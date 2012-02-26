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

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#ifdef __cplusplus                     
extern "C" {
#endif

void 
UnInitFrameBuffer();

void /*inline*/ 
Gal_PutPixelRaw(
	int x,
	int y, 
	COLORREF color
);

void /*inline*/ 
Gal_PutPixel(
	int x,
	int y, 
	COLORREF color
);

COLORREF /*inline*/ 
Gal_GetPixel(
	int x,
	int y
);

COLORREF /*inline*/ 
Gal_GetPixelRaw(
	int x,
	int y
);


void 
Gal_PutRect(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
);

void 
Gal_PutRectWithTrCor(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
);

void 
Gal_PutLine(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
);

void /*inline*/ 
Gal_PutLineRAW(
	int xPos,
	int yPos, 
	int iLen, 
	void* pData
);

void 
Gal_GetLine(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
);

void 
Gal_GetLineRAW(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
);

void 
Gal_EraseRect(
	PRECT lpRc,
	COLORREF color
);

/*inline*/ void 
Gal_GetBox(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

/*inline*/ void 
Gal_GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

/*inline*/ void 
Gal_PutBoxRawRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

/*inline*/ void 
Gal_PutBoxRawRawWithTrCor(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
);
#ifdef __cplusplus
}
#endif

#endif


