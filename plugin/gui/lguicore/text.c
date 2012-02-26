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
#include "../include/hdc.h"
#include "../include/rect.h"
#include "../include/gmessage.h"
#include "../include/blockheap.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/shmem.h"
#include "../include/gdi.h"
#include "../include/text.h"
#include "../include/mouse.h"


static PFONT /*inline*/ 
GetFont(
	HDC hDC
);

static void 
GetAscFontData(
	char* pBuffer, 
	FONT* pFont,
	char* pChar
);

static void 
GetChnFontData(
	char* pBuffer, 
	FONT* pFont,
	char* pChar
);

static BOOL 
GetBitValue(
	char* pData,
	int iPos
);

static void 
DrawaAscChar(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar
);

static void 
DrawaChnChar(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar
);

static int /*inline*/ 
CalXPos(
	int iLen, 
	double fDirection
);

static int /*inline*/ 
CalYPos(
	int iLen, 
	double fDirection
);

static void 
DrawaAscCharAngle(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar, 
	double fDirection
);

static void 
DrawaChnCharAngle(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar, 
	double fDirection
);



static PFONT /*inline*/ 
GetFont(
	HDC hDC
)
{
	return (FONT*)(hDC->hFont);
}
 

static void 
GetAscFontData(
	char* pBuffer, 
	FONT* pFont,
	char* pChar
)
{
	int iOffset;
	iOffset=pFont->iOffset + pFont->FontLibHeader.iSize 
		+ pFont->FontLibHeader.iAscOffset;
	iOffset=iOffset+(*pChar) * pFont->FontLibHeader.iAscBytes;
	shm_read(pBuffer, iOffset, pFont->FontLibHeader.iAscBytes);
}

static void 
GetChnFontData(
	char* pBuffer, 
	FONT* pFont,
	char* pChar
)
{
	unsigned char qh,wh;
	int iOffset;
	//qu-wei code of chinese
	qh=*(pChar)-0xa0;
	wh=*(pChar+1)-0xa0;
	if(qh>=0x10){
		iOffset=pFont->iOffset + pFont->FontLibHeader.iSize 
			+ pFont->FontLibHeader.iChnOffset;
		iOffset=iOffset 
			+ ((94*(qh-1)+(wh-1))-0x0582)* pFont->FontLibHeader.iChnBytes;
	}
	else{
		iOffset=pFont->iOffset + pFont->FontLibHeader.iSize 
			+ pFont->FontLibHeader.iChnSymOffset;
		iOffset=iOffset 
			+ (94*(qh-1)+(wh-1)) * pFont->FontLibHeader.iChnBytes;
	}
	shm_read(pBuffer, iOffset, pFont->FontLibHeader.iChnBytes);
}


static BOOL 
GetBitValue(
	char* pData,
	int iPos
)
{
	int iCurByte;
	int iCurBit;
	unsigned char Mask=0x80;
	iCurByte=iPos/8;
	iCurBit=iPos%8;
	Mask=Mask>>iCurBit;
	if(*(pData+iCurByte) & Mask)
		return true;
	else
		return false;
}

//draw a ascii character
//x and y is left top coord
static void 
DrawaAscChar(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar
)
{
	char pBuffer[128],*pCurPointer;
	//int iWinWidth,iWinHeight;
	int iCharWidth,iCharHeight;
	int iCharWidthBytes;
	RECT rc;
	FONT* pFont;
	COLORREF crColor;
	COLORREF crTextBkColor;
	BOOL isTextBkTrans;
	int x,y;



	GetClientRect(hDC->hWnd,&rc);
	//iWinWidth=rc.right-rc.left+1;
	//iWinHeight=rc.bottom-rc.top+1;
	SetRect(&rc,0,0,rc.right - rc.left, rc.bottom - rc.top);

	crColor			=hDC->crTextColor;
	crTextBkColor	=hDC->crTextBkColor;
	isTextBkTrans	=hDC->isTextBkTrans;

	pFont=GetFont(hDC);

	GetAscFontData(pBuffer,pFont,pChar);
	pCurPointer=pBuffer;


	iCharWidth=pFont->FontLibHeader.iAscWidth;
	iCharHeight=pFont->FontLibHeader.iAscHeight;
	if(iCharWidth%8==0)
		iCharWidthBytes=iCharWidth/8;
	else
		iCharWidthBytes=iCharWidth/8+1;

	for(y=0;y<iCharHeight;y++){
		if((y+yPos< rc.top || y+yPos > rc.bottom))
			continue;
		for(x=0;x<iCharWidth;x++){
			if(x+xPos < rc.left || x+xPos>rc.right)
				continue;
			if(GetBitValue(pCurPointer,x))
				winSetPixel(hDC, xPos+x, yPos+y, crColor);
			else{
				if(!isTextBkTrans)
					winSetPixel(hDC,xPos+x,yPos+y,crTextBkColor);
			}
		}
		pCurPointer=pCurPointer + iCharWidthBytes;
	}
}



static void 
DrawaChnChar(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar
)
{
	char pBuffer[128],*pCurPointer;
	//int iWinWidth,iWinHeight;
	int iCharWidth,iCharHeight;
	int iCharWidthBytes;
	RECT rc;
	FONT* pFont;
	COLORREF crColor;
	COLORREF crTextBkColor;
	BOOL isTextBkTrans;
	int x,y;

	GetClientRect(hDC->hWnd,&rc);
	SetRect(&rc,0,0,rc.right - rc.left, rc.bottom - rc.top);

	crColor			=hDC->crTextColor;
	crTextBkColor	=hDC->crTextBkColor;
	isTextBkTrans	=hDC->isTextBkTrans;

	pFont=GetFont(hDC);
	GetChnFontData(pBuffer,pFont,pChar);
	pCurPointer=pBuffer;

	iCharWidth=pFont->FontLibHeader.iChnWidth;
	iCharHeight=pFont->FontLibHeader.iChnHeight;
	if(iCharWidth%8==0)
		iCharWidthBytes=iCharWidth/8;
	else
		iCharWidthBytes=iCharWidth/8+1;

	for(y=0;y<iCharHeight;y++){
		if((y+yPos< rc.top || y+yPos > rc.bottom))
			continue;

		for(x=0;x<iCharWidth;x++){
			if(x+xPos < rc.left || x+xPos>rc.right)
				continue;

			if(GetBitValue(pCurPointer,x))
				winSetPixel(hDC, xPos+x, yPos+y, crColor);
			else{
				if(!isTextBkTrans)
					winSetPixel(hDC,xPos+x,yPos+y,crTextBkColor);
			}
		}
		pCurPointer=pCurPointer + iCharWidthBytes;
	}
}


BOOL GUIAPI 
DrawText(
	HDC hDC,
	char* lpString,
	int nCount,
	LPRECT lpRect,
	UINT uFormat
)
{
	ClientToWindowRect(hDC->hWnd,lpRect);
	return winDrawText(hDC,lpString,nCount,lpRect,uFormat);
}


BOOL 
winDrawText(
	HDC hDC,
	char* lpString,
	int nCount,
	LPRECT lpRect,
	UINT uFormat
)
{
	char* lpStr;
	FONT* pFont;
	int iChnCharWidth,iChnCharHeight;
	int iAscCharWidth;
	int iRectWidth,iRectHeight;
	int iStrWidth;
	int x,y;

	int iCurWidth;

	iRectWidth=lpRect->right-lpRect->left+1;
	iRectHeight=lpRect->bottom-lpRect->top+1;
	pFont=GetFont(hDC);
	iAscCharWidth=pFont->FontLibHeader.iAscWidth;

	iChnCharWidth=pFont->FontLibHeader.iChnWidth;
	iChnCharHeight=pFont->FontLibHeader.iChnHeight;

	iStrWidth=0;
	lpStr=lpString;
	while(*lpStr){
		if((*lpStr) & 0x80){
			iStrWidth+=iChnCharWidth;
			lpStr+=2;
		}
		else{
			iStrWidth+=iAscCharWidth;
			lpStr+=1;
		}
	}
	switch(uFormat & DT_VMASK){
	case DT_TOP:
		y=lpRect->top;
		break;
	case DT_VCENTER:
		if(iChnCharHeight>=iRectHeight)
			y= lpRect->top - (iChnCharHeight-iRectHeight)/2;
		else
			y=lpRect->top + (iRectHeight-iChnCharHeight)/2;
		break;
	case DT_BOTTOM:
		y=lpRect->bottom - iChnCharHeight;
		break;
	default:
		if(iChnCharHeight>=iRectHeight)
			y= lpRect->top - (iChnCharHeight-iRectHeight)/2;
		else
			y=lpRect->top + (iRectHeight-iChnCharHeight)/2;
		break;

	}

	switch(uFormat & DT_HMASK){
	case DT_LEFT:
		x=lpRect->left;
		break;
	case DT_CENTER:
		if(iStrWidth>iRectWidth)
			x=lpRect->left - (iStrWidth-iRectWidth)/2;
		else
			x=lpRect->left + (iRectWidth-iStrWidth)/2;

		break;
	case DT_RIGHT:
		x=lpRect->right-iStrWidth;
		break;
	default:
		if(iStrWidth>iRectWidth)
			x=lpRect->left - (iStrWidth-iRectWidth)/2;
		else
			x=lpRect->left + (iRectWidth-iStrWidth)/2;

		break;
	}

	iCurWidth = 0;
	lpStr=lpString;
	while(*lpStr){
		if((*lpStr) & 0x80){
			iCurWidth += iChnCharWidth;
			if(iCurWidth > iRectWidth)
				break;
			DrawaChnChar(hDC, x, y, lpStr);
			x+=iChnCharWidth;
			lpStr+=2;
		}
		else{
			iCurWidth += iAscCharWidth;
			if(iCurWidth > iRectWidth)
				break;
			DrawaAscChar(hDC, x, y, lpStr);
			x+=iAscCharWidth;
			lpStr+=1;
		}
	}

	return true;
}


//para ncount is invalid
//this function implemented for gis
//lpPoint is left top of string position
BOOL GUIAPI 
DrawTextAngle(
	HDC hDC,
	char* lpString,
	int nCount,
	POINT* lpPoint,
	double fDirection
)
{
	ClientToWindow(hDC->hWnd,&lpPoint->x,&lpPoint->y);
	return winDrawTextAngle(hDC,lpString,nCount,lpPoint,fDirection);

}


BOOL 
winDrawTextAngle(
	HDC hDC,
	char* lpString,
	int nCount,
	POINT* lpPoint,
	double fDirection
)
{
	POINT point;
	char* lpStr;
	FONT* pFont;
	int iChnCharWidth,iChnCharHeight,iAscCharWidth;
	int iCurWidth;

	if(!lpString)
		return false;
	if(!lpPoint)
		return false;

	memcpy(&point,lpPoint,sizeof(POINT));
	pFont=GetFont(hDC);
	
	iAscCharWidth=pFont->FontLibHeader.iAscWidth;
	iChnCharWidth=pFont->FontLibHeader.iChnWidth;
	iChnCharHeight=pFont->FontLibHeader.iChnHeight;

	iCurWidth = 0;
	lpStr=lpString;

	while(*lpStr){
		if((*lpStr) & 0x80){
			DrawaChnCharAngle(hDC, 
				point.x + CalXPos(iCurWidth,fDirection), 
				point.y + CalYPos(iCurWidth,fDirection), lpStr, fDirection);
			iCurWidth += iChnCharWidth;
			lpStr+=2;
		}
		else{
			//is line end character "\n"  
/*			if(*lpStr == '\\' && *(lpStr+1) == 'n'){
				point.x = point.x + CalXPos(iChnCharHeight, fDirection - 90);
				point.y = point.y + CalYPos(iChnCharHeight, fDirection - 90);
				iCurWidth = 0;
				lpStr += 2;
			}*/
			if(*lpStr == '\n'){
				point.x = point.x + CalXPos(iChnCharHeight, fDirection-90);
				point.y = point.y + CalYPos(iChnCharHeight, fDirection-90);
				iCurWidth = 0;
				lpStr += 1;
			}
			else{
				DrawaAscCharAngle(hDC, 
					point.x + CalXPos(iCurWidth,fDirection),
					point.y + CalYPos(iCurWidth,fDirection), lpStr, fDirection);
				iCurWidth += iAscCharWidth;
				lpStr += 1;
			}
		}
	}
	return 1;
}


static int /*inline*/ 
CalXPos(
	int iLen, 
	double fDirection
)
{
	while(fDirection < 0)
		fDirection += 360;
	while(fDirection > 360)
		fDirection -= 360;

	if(!iLen)
		return 0;
	else if(fDirection == 0)
		return iLen;
	else if(fDirection == 180)
		return (-1) * iLen;
	else if(fDirection == 90 || fDirection == 270)
		return 0;
	else
		return (int)(((double)iLen)*cos(DEG2RAD(fDirection)));
}


static int /*inline*/ 
CalYPos(
	int iLen, 
	double fDirection
)
{

	while(fDirection < 0)
		fDirection += 360;
	while(fDirection > 360)
		fDirection -= 360;

	if(!iLen)
		return 0;
	else if(fDirection == 0 || fDirection == 180)
		return 0;
	else if(fDirection == 90)
		return (-1)*iLen;
	else if(fDirection == 270)
		return iLen;
	else
		return (-1)*(int)(((double)iLen)*sin(DEG2RAD(fDirection)));
}



static void 
DrawaAscCharAngle(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar, 
	double fDirection
)
{

	char pBuffer[128],*pCurPointer;

	int iCharWidth,iCharHeight;
	int iCharWidthBytes;
	RECT rc;
	FONT* pFont;
	COLORREF crColor;
	COLORREF crTextBkColor;
	BOOL isTextBkTrans;
	int x,y;
	int xCoord,yCoord;

	
	GetClientRect(hDC->hWnd,&rc);
	SetRect(&rc,0,0,rc.right - rc.left, rc.bottom - rc.top);

	crColor			=hDC->crTextColor;
	crTextBkColor	=hDC->crTextBkColor;
	isTextBkTrans	=hDC->isTextBkTrans;

	pFont=GetFont(hDC);

	GetAscFontData(pBuffer,pFont,pChar);
	pCurPointer=pBuffer;


	iCharWidth=pFont->FontLibHeader.iAscWidth;
	iCharHeight=pFont->FontLibHeader.iAscHeight;
	if(iCharWidth%8==0)
		iCharWidthBytes=iCharWidth/8;
	else
		iCharWidthBytes=iCharWidth/8+1;

	for(y=0;y<iCharHeight;y++){

		for(x=0;x<iCharWidth;x++){

			xCoord = (int)((double)xPos + (double)x * cos(DEG2RAD(fDirection)) + (double)y * sin(DEG2RAD(fDirection)));
			yCoord = (int)((double)yPos - (double)x * sin(DEG2RAD(fDirection)) + (double)y * cos(DEG2RAD(fDirection)));
			if(xCoord<rc.left || xCoord>rc.right)
				continue;
			if(yCoord<rc.top || yCoord>rc.bottom)
				continue;

			if(GetBitValue(pCurPointer,x))
				winSetPixel(hDC, xCoord, yCoord, crColor);
			else{
				if(!isTextBkTrans)
					winSetPixel(hDC,xCoord,yCoord,crTextBkColor);
			}
		}
		pCurPointer=pCurPointer + iCharWidthBytes;
	}
}



static void 
DrawaChnCharAngle(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pChar, 
	double fDirection
)
{
	char pBuffer[128],*pCurPointer;
	int iCharWidth,iCharHeight;
	int iCharWidthBytes;
	RECT rc;
	FONT* pFont;
	COLORREF crColor;
	COLORREF crTextBkColor;
	BOOL isTextBkTrans;
	int x,y;
	int xCoord,yCoord;


	GetClientRect(hDC->hWnd,&rc);
	SetRect(&rc,0,0,rc.right - rc.left, rc.bottom - rc.top);

	crColor			=hDC->crTextColor;
	crTextBkColor	=hDC->crTextBkColor;
	isTextBkTrans	=hDC->isTextBkTrans;

	pFont=GetFont(hDC);
	GetChnFontData(pBuffer,pFont,pChar);
	pCurPointer=pBuffer;

	iCharWidth=pFont->FontLibHeader.iChnWidth;
	iCharHeight=pFont->FontLibHeader.iChnHeight;
	if(iCharWidth%8==0)
		iCharWidthBytes=iCharWidth/8;
	else
		iCharWidthBytes=iCharWidth/8+1;

	for(y=0;y<iCharHeight;y++){
		for(x=0;x<iCharWidth;x++){

			xCoord = (int)((double)xPos + (double)x * cos(DEG2RAD(fDirection)) + (double)y * sin(DEG2RAD(fDirection)));
			yCoord = (int)((double)yPos - (double)x * sin(DEG2RAD(fDirection)) + (double)y * cos(DEG2RAD(fDirection)));
			if(xCoord<rc.left || xCoord>rc.right)
				continue;
			if(yCoord<rc.top || yCoord>rc.bottom)
				continue;

			if(GetBitValue(pCurPointer,x))
				winSetPixel(hDC, xCoord, yCoord, crColor);
			else{
				if(!isTextBkTrans)
					winSetPixel(hDC,xCoord,yCoord,crTextBkColor);
			}
		}
		pCurPointer=pCurPointer + iCharWidthBytes;
	}
}

//x′＝xr＋（x－xr)cosθ－（y－yr)sinθ

//y′＝yr＋（y－yr）cosθ＋（x－xr）sinθ

































