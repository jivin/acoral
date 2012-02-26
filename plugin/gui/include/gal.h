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

#ifndef _GAL_H_
#define _GAL_H_

#ifdef __cplusplus                     
extern "C" {
#endif
struct GalOps{
	BOOL (*Init)();
	void (*UnInit)();
	void (*PutPixel)(int x,int y,COLORREF color);
	COLORREF (*GetPixel)(int x,int y);
	void (*PutLine)(int xPos,int yPos,int iLen, void* pData);
	void (*GetLine)(int xPos,int yPos,int iLen, void* pData);
	void (*PutRect)(PRECT lpOutRc,PRECT lpOrgRc,void* pData);
	void (*GetRect)(PRECT lpOutRc,PRECT lpOrgRc,void* pData);
	void (*PutRectWithTrCor)(PRECT lpOutRc,PRECT lpOrgRc,void* pData, COLORREF crTrColor);
	void (*PutBox)(int x, int y,int width,int height,PCOLORREF pData);
	void (*GetBox)(int x, int y,int width,int height,PCOLORREF pData);
	void (*PutBoxWithTrCor)(int x, int y,int width,int height,PCOLORREF pData,COLORREF color);
    void (*EraseRect)(PRECT lpRc,COLORREF color);
};

struct GalDev{
	struct GalOps *ops;
	int ScreenWidth;
	int ScreenHeight;
	int iBytesDataType;
};

void 
UnInitGal();

BOOL 
InitGal();

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
Gal_PutLineRaw(
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
Gal_GetLineRaw(
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
Gal_PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

/*inline*/ void 
Gal_PutBoxWithTrCorRaw(
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


