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

#include "../include/common.h"
#include "../include/mouse.h"
#include "../include/rect.h"
#include "../include/gal.h"

extern struct GalDev s3c2440GalDev;
extern struct GalDev X86EmuGalDev;
extern struct GalDev DummyGalDev;
extern struct GalDev QvfbGalDev;
struct GalDev *CurGalDev;
struct GalOps *CurGalOps;
int Gal_iBytesDataType;
acoral_u32 SCREEN_HEIGHT,SCREEN_WIDTH;
BOOL 
InitGal(){

	CurGalDev=&DummyGalDev;
#ifdef CFG_ARM_S3C2440
	CurGalDev=&s3c2440GalDev;
#endif
#ifdef CFG_X86_EMU_SINGLE
	CurGalDev=&QvfbGalDev;
#endif
#ifdef CFG_X86_EMU_CMP
	CurGalDev=&QvfbGalDev;
#endif

	CurGalOps=CurGalDev->ops;
	if(CurGalOps!=NULL&&CurGalOps->Init!=NULL&&CurGalOps->Init()){
			SCREEN_HEIGHT=CurGalDev->ScreenHeight;
			SCREEN_WIDTH=CurGalDev->ScreenWidth;
			Gal_iBytesDataType=CurGalDev->iBytesDataType;
			return true;
	}
	return false;
}

void 
UnInitGal(){
	CurGalOps->UnInit();
}

void /*inline*/ 
Gal_PutPixelRaw(
	int x,
	int y, 
	COLORREF color
)
{
	CurGalOps->PutPixel(x,y,color);
}

void /*inline*/ 
Gal_PutPixel(
	int x,
	int y, 
	COLORREF color
)
{
	RECT rc;
	SetRect(&rc,x,y,x,y);
	RequestPaint(&rc);
	Gal_PutPixelRaw(x,y,color);
	CompletePaint();
}

COLORREF /*inline*/ 
Gal_GetPixel(
	int x,
	int y
)
{
	COLORREF crColor;
	RECT rc;
	SetRect(&rc,x,y,x,y);
	RequestPaint(&rc);
	crColor=Gal_GetPixelRaw(x,y);
	CompletePaint();
	return crColor;
}

COLORREF /*inline*/ 
Gal_GetPixelRaw(
	int x,
	int y
)
{
	return CurGalOps->GetPixel(x,y);
}

void Gal_PutRectRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
){
	CurGalOps->PutRect(lpOutRc,lpOrgRc,pData);
}

void 
Gal_PutRect(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
)
{

	RequestPaint(lpOutRc);
	Gal_PutRectRaw(lpOutRc,lpOrgRc,pData);
	CompletePaint();

}

void 
Gal_PutRectWithTrCorRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
)
{
	CurGalOps->PutRectWithTrCor(lpOutRc,lpOrgRc,pData,crTrColor);
}


void 
Gal_PutRectWithTrCor(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
)
{
	RequestPaint(lpOutRc);
	Gal_PutRectWithTrCorRaw(lpOutRc,lpOrgRc,pData,crTrColor);
	CompletePaint();
}



void 
Gal_PutLine(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
)
{

	RECT rcOut;
	SetRect(&rcOut,xPos,yPos, xPos+iLen,yPos);
	RequestPaint(&rcOut);
	Gal_PutLineRaw(xPos,yPos,iLen,pData);
	CompletePaint();

}


void /*inline*/ 
Gal_PutLineRaw(
	int xPos,
	int yPos, 
	int iLen, 
	void* pData
)
{
	CurGalOps->PutLine(xPos,yPos,iLen,pData);
}


void 
Gal_GetLine(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
)
{

	RECT rcOut;
	SetRect(&rcOut,xPos,yPos, xPos+iLen,yPos);
	RequestPaint(&rcOut);
    Gal_GetLineRaw(xPos,yPos,iLen,pData);
	CompletePaint();

}

void 
Gal_GetLineRaw(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
)
{
	CurGalOps->GetLine(xPos,yPos,iLen,pData);
}

void 
Gal_EraseRectRaw(
	PRECT lpRc,
	COLORREF color
)
{
	CurGalOps->EraseRect(lpRc,color);
}

void 
Gal_EraseRect(
	PRECT lpRc,
	COLORREF color
)
{
	RequestPaint(lpRc);
	Gal_EraseRectRaw(lpRc,color);
	CompletePaint();
}



/*inline*/ void 
Gal_GetBox(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
	RECT rc;
	SetRect(&rc, x, y, x+nWidth, y+nHeight);
	RequestPaint(&rc);
	Gal_GetBoxRaw(x,y,nWidth,nHeight,pData);
	CompletePaint();
}

/*inline*/ void 
Gal_GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
	CurGalOps->GetBox(x,y,nWidth,nHeight,pData);
}

/*inline*/ void 
Gal_PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
	CurGalOps->PutBox(x,y,nWidth,nHeight,pData);
}

/*inline*/ void 
Gal_PutBoxWithTrCorRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
)
{
	CurGalOps->PutBoxWithTrCor(x,y,nWidth,nHeight,pData,crColor);
}
