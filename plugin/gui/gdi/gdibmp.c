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
#include "../include/bmp.h"
#include "../include/gal.h"
#include "../include/gdibmp.h"
    
extern int Gal_iBytesDataType;

BOOL GUIAPI 
ShowBitmapWithTrColor(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
)
{
	ClientToWindow(hDC->hWnd,&xPos,&yPos);
	return winShowBitmapWithTrColor(hDC,xPos,yPos,pFile,crTrColor);
}


BOOL 
winShowBitmapWithTrColor(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
)
{
	int iType;
	iType=hDC->iType;
 	if(iType==DC_TYPE_WIN)
		if(ShowBitmapWithTrColorWin(hDC, xPos, yPos, pFile, crTrColor))
			return true;
		else
			return false;
	else
		if(ShowBitmapWithTrColorMem(hDC, xPos, yPos, pFile, crTrColor))
			return true;
		else
			return false;
}


BOOL 
ShowBitmapWithTrColorWin(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
)
{
	PClipRect pClipRect;
	FILE_P fd;
	BMPINFOHEADER infoHeader;

	void* pData;
	RECT rcSource,rcDest;
	int iWinWidth, iWinHeight;
	int iBmpWidth, iBmpHeight;
	int iWidth,iHeight;

	GetWindowRect(hDC->hWnd,&rcSource);
	WindowToScreen(hDC->hWnd,&xPos,&yPos);

	if(xPos>rcSource.right || xPos<rcSource.left)
		return false;
	if(yPos>rcSource.bottom || yPos<rcSource.top)
		return false;

	iWinWidth	=rcSource.right-xPos+1;
	iWinHeight	=rcSource.bottom-yPos+1;

	fd=OPEN(pFile,RDONLY);
	if(fd<0)
		return false;
	SEEK(fd,0x12,0);
	READ(fd,&(infoHeader.biWidth),sizeof(infoHeader.biWidth));
	READ(fd,&(infoHeader.biHeight),sizeof(infoHeader.biHeight));

	SEEK(fd,0x1c,0);
	READ(fd,&(infoHeader.biBitCount),sizeof(infoHeader.biBitCount));

	iBmpWidth	=infoHeader.biWidth;
	iBmpHeight	=infoHeader.biHeight;

	iWidth=min(iBmpWidth,iWinWidth);
	iHeight=min(iBmpHeight,iWinHeight);

	pData=malloc(iWidth*iHeight*Gal_iBytesDataType);
	if(!pData){
		CLOSE(fd);
		return false;
	}
	SetRect(&rcSource,xPos,yPos,xPos+iWidth-1,yPos+iHeight-1);

	if(!ReadBmpToBuf(fd, iWidth, iHeight, pData)){
		free(pData);
		CLOSE(fd);
		return false;
	}

	pClipRect=((PWindowsTree)(hDC->hWnd))->pClipRgn->pHead;
	while(pClipRect){
		if(IntersectRect(&rcDest,&rcSource,&(pClipRect->rect)))
			Gal_PutRectWithTrCor(&rcDest,&rcSource,pData,crTrColor);
		pClipRect=pClipRect->pNext;
	}

	free(pData);
	CLOSE(fd);
	return true;
}


BOOL 
ShowBitmapWithTrColorMem(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile, 
	COLORREF crTrColor
)
{
	FILE_P fd;
	BMPINFOHEADER infoHeader;
	int iBmpWidth, iBmpHeight;
	int iCliWidth,iCliHeight;
	int iWinWidth,iWinHeight;
	int iWidth,iHeight;

	int x,y;
	void* pCurPointer;
	void* pOutPointer;

	void *pData;
	RECT rc;


	if(!(hDC->hWnd))
		return false;
	GetWindowRect(hDC->hWnd,&rc);
	iWinWidth = rc.right - rc.left + 1;
	iWinHeight = rc.bottom - rc.top + 1;


	if(!(hDC->hWnd))
		return false;
	GetWindowRect(hDC->hWnd,&rc);
	iWinWidth = rc.right - rc.left + 1;
	iWinHeight = rc.bottom - rc.top + 1;

	GetClientRect(hDC->hWnd,&rc);
	iCliWidth = rc.right - rc.left + 1;
	iCliHeight = rc.bottom - rc.top + 1;


	if(xPos>=iWinWidth || xPos<0){
		printerror("show bitmap out of range.");
		return false;
	}
	if(yPos>=iWinHeight || yPos<0){
		printerror("show bitmap out of range.");
		return false;
	}
	fd=OPEN(pFile,RDONLY);
	if(fd<0){
		printerror("open file error!");
		return false;
	}

	SEEK(fd,0x12,0);
	READ(fd,&(infoHeader.biWidth),sizeof(infoHeader.biWidth));
	READ(fd,&(infoHeader.biHeight),sizeof(infoHeader.biHeight));

	SEEK(fd,0x1c,0);
	READ(fd,&(infoHeader.biBitCount),sizeof(infoHeader.biBitCount));

	iBmpWidth=infoHeader.biWidth;
	iBmpHeight=infoHeader.biHeight;

	iWidth=min(iBmpWidth,iCliWidth-xPos);
	iHeight=min(iBmpHeight,iCliHeight-yPos);

	pData=malloc(iWidth*iHeight*Gal_iBytesDataType);
	if(!pData){
		printerror("alloc memory error!");
		return false;
	}

	if(!ReadBmpToBuf(fd, iWidth, iHeight, pData)){
		free(pData);
		CLOSE(fd);
		return false;
	}
	for(y=0;y<iHeight;y++){
		for(x=0;x<iWidth;x++){
			pCurPointer=(acoral_u8*)pData + (y*iWidth+x)*Gal_iBytesDataType;
			if(*((PCOLORREF)(pCurPointer))!=crTrColor){
				pOutPointer=(acoral_u8*)hDC->pData + (y + yPos)*iWinWidth*Gal_iBytesDataType + 
					(x + xPos)*Gal_iBytesDataType;
				memcpy(pOutPointer,pCurPointer,Gal_iBytesDataType);
			}
		}
	}

	free(pData);
	CLOSE(fd);
	return true;
}



BOOL GUIAPI 
ShowBitmap(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile
)
{
	ClientToWindow(hDC->hWnd,&xPos,&yPos);
	return winShowBitmap(hDC,xPos,yPos,pFile);
}


BOOL 
winShowBitmap(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile
)
{
	int iType;
	iType=hDC->iType;
	if(iType==DC_TYPE_WIN){
		if(ShowBitmapWin(hDC, xPos, yPos, pFile))
			return true;
		else
			return false;
	}
	else{
		if(ShowBitmapMem(hDC, xPos, yPos, pFile))
			return true;
		else
			return false;
	}

}


BOOL 
ShowBitmapWin(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile
)
{
	PClipRect pClipRect;
	FILE_P fd;
	BMPINFOHEADER infoHeader;

	void* pData;
	RECT rcSource,rcDest;
	int iWinWidth, iWinHeight;
	int iBmpWidth, iBmpHeight;
	int iWidth,iHeight;

	GetWindowRect(hDC->hWnd,&rcSource);
	scrGetClientRect(hDC->hWnd,&rcSource);
	WindowToScreen(hDC->hWnd,&xPos,&yPos);

	if(xPos>rcSource.right || xPos<rcSource.left)
		return false;
	if(yPos>rcSource.bottom || yPos<rcSource.top)
		return false;

	iWinWidth	=rcSource.right-xPos+1;
	iWinHeight	=rcSource.bottom-yPos+1;

	fd=OPEN(pFile,RDONLY);
	if(fd<0)
		return false;
	SEEK(fd,0x12,0);
	READ(fd,&(infoHeader.biWidth),sizeof(infoHeader.biWidth));
	READ(fd,&(infoHeader.biHeight),sizeof(infoHeader.biHeight));

	SEEK(fd,0x1c,0);
	READ(fd,&(infoHeader.biBitCount),sizeof(infoHeader.biBitCount));

	iBmpWidth	=infoHeader.biWidth;
	iBmpHeight	=infoHeader.biHeight;

	iWidth=min(iBmpWidth,iWinWidth);
	iHeight=min(iBmpHeight,iWinHeight);

	pData=malloc(iWidth*iHeight*Gal_iBytesDataType);
	if(!pData){
		CLOSE(fd);
		return false;
	}
	SetRect(&rcSource,xPos,yPos,xPos+iWidth-1,yPos+iHeight-1);

	if(!ReadBmpToBuf(fd, iWidth, iHeight, pData)){
		free(pData);
		CLOSE(fd);
		return false;
	}
	pClipRect=((PWindowsTree)(hDC->hWnd))->pClipRgn->pHead;
	while(pClipRect){
		if(IntersectRect(&rcDest,&rcSource,&(pClipRect->rect)))
			Gal_PutRect(&rcDest,&rcSource,pData);
		pClipRect=pClipRect->pNext;
	}

	free(pData);
	CLOSE(fd);
	return true;	
}


BOOL 
ShowBitmapMem(
	HDC hDC, 
	int xPos, 
	int yPos, 
	char* pFile
)
{
	FILE_P fd;
	BMPINFOHEADER infoHeader;
	int iBmpWidth, iBmpHeight;
	int iWinWidth,iWinHeight;
	int iCliWidth,iCliHeight;
	int iWidth,iHeight;
	int iLeftMarge,iTopMarge,iBottom;

	int y;
	void* pCurPointer;
	void* pOutPointer;

	void *pData;
	RECT rc;


	if(!(hDC->hWnd))
		return false;
	GetWindowRect(hDC->hWnd,&rc);
	iWinWidth = rc.right - rc.left + 1;
	iWinHeight = rc.bottom - rc.top + 1;

	GetClientRect(hDC->hWnd,&rc);
	iCliWidth = rc.right - rc.left + 1;
	iCliHeight = rc.bottom - rc.top + 1;

	if(xPos>=iWinWidth || xPos<0){
		printerror("show bitmap out of range.");
		return false;
	}
	if(yPos>=iWinHeight || yPos<0){
		printerror("show bitmap out of range.");
		return false;
	}
	fd=OPEN(pFile,RDONLY);
	if(fd<0){
		printerror("open file error!");
		return false;
	}

	SEEK(fd,0x12,0);
	READ(fd,&(infoHeader.biWidth),sizeof(infoHeader.biWidth));
	READ(fd,&(infoHeader.biHeight),sizeof(infoHeader.biHeight));

	SEEK(fd,0x1c,0);
	READ(fd,&(infoHeader.biBitCount),sizeof(infoHeader.biBitCount));

	iBmpWidth=infoHeader.biWidth;
	iBmpHeight=infoHeader.biHeight;

	iWidth=min(iBmpWidth,iCliWidth-xPos);
	iHeight=min(iBmpHeight,iCliHeight-yPos);

	pData=malloc(iWidth*iHeight*Gal_iBytesDataType);
	if(!pData){
		printerror("alloc memory error!");
		return false;
	}
	if(!ReadBmpToBuf(fd, iWidth, iHeight, pData)){
		free(pData);
		CLOSE(fd);
		return false;
	}


	iBottom=yPos+iHeight-1;

	iLeftMarge=xPos*Gal_iBytesDataType;
	iTopMarge=iWinWidth*yPos*Gal_iBytesDataType;
	pCurPointer=pData;
	pOutPointer=(acoral_u8*)hDC->pData+iTopMarge+iLeftMarge;
	for(y=yPos;y<=iBottom;y++){

		memcpy(pOutPointer,pCurPointer,iWidth*Gal_iBytesDataType);
		pOutPointer = (void*)((acoral_u8*)pOutPointer+iWinWidth*Gal_iBytesDataType);
		pCurPointer= (void*)((acoral_u8*) pCurPointer + iWidth*Gal_iBytesDataType);
	}

	free(pData);
	CLOSE(fd);
	return true;

}



BOOL 
OutputBmpObj(
	HDC hDC,
	int x,
	int y,
	int iDestWidth, 
	int iDestHeight, 
	HBITMAP hBitmap
)
{
	if(hDC->iType == DC_TYPE_MEM)
		return OutputBmpObjMem(hDC,x,y,iDestWidth,iDestHeight,hBitmap);
	else
		return OutputBmpObjWin(hDC,x,y,iDestWidth,iDestHeight,hBitmap);
}

BOOL 
OutputBmpObjWin(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight,
	HBITMAP hBitmap
)
{
	PBITMAP pBitmap;
	int iWidth, iHeight;
	int i,j;
	PCOLORREF pData;
	if(!pBitmap)
		return false;
	iWidth = pBitmap->bmWidth;
	iHeight = pBitmap->bmHeight;
	pData = (PCOLORREF)(pBitmap->bmBits);
	for(i=0;i<iHeight;i++){
		for(j=0;j<iWidth;j++){
			SetPixel(hDC,x+j,y+i,*pData);
			pData++;		
		}
	}
	return true;
}


BOOL 
OutputBmpObjMem(
	HDC hDC, 
	int x,
	int y,
	int iDestWidth, 
	int iDestHeight,
	HBITMAP hBitmap
)
{
	PBITMAP pBitmap;
	int iWidth, iHeight;
	int i;
	PCOLORREF pDestData;
	PCOLORREF pSrcData;
	pBitmap=(PBITMAP)hBitmap;
	if(!pBitmap)
		return false;
	iWidth = pBitmap->bmWidth;
	iHeight = pBitmap->bmHeight;
	pDestData = (PCOLORREF)(hDC->pData);
	if(!pDestData)
		return false;
	pDestData += y*iDestWidth+x;
	pSrcData = (PCOLORREF)(pBitmap->bmBits);
	for(i=0;i<iHeight;i++){
		memcpy((void*)pDestData,(void*)pSrcData,iWidth*Gal_iBytesDataType);
		pSrcData += iWidth;
		pDestData += iDestWidth;
	}
	return true;
}



BOOL 
OutputBmpObjWithTrColor(
	HDC hDC,
	int x,
	int y,
	int iDestWidth, 
	int iDestHeight, 
	HBITMAP hBitmap,
	COLORREF crColor
)
{
	if(hDC->iType == DC_TYPE_MEM)
		return OutputBmpObjMemWithTrColor(hDC,x,y,iDestWidth,iDestHeight,hBitmap,crColor);
	else
		return OutputBmpObjWinWithTrColor(hDC,x,y,iDestWidth,iDestHeight,hBitmap, crColor);
}


BOOL 
OutputBmpObjWinWithTrColor(
	HDC hDC, 
	int x, 
	int y, 
	int iDestWidth, 
	int iDestHeight,
	HBITMAP hBitmap,
	COLORREF crColor
)
{
	PBITMAP pBitmap;
	int iWidth, iHeight;
	int i,j;
	PCOLORREF pData;
	pBitmap=(PBITMAP)hBitmap;
	if(!pBitmap)
		return false;
	iWidth = pBitmap->bmWidth;
	iHeight = pBitmap->bmHeight;
	pData = (PCOLORREF)(pBitmap->bmBits);
	for(i=0;i<iHeight;i++){
		for(j=0;j<iWidth;j++){
			if(*pData != crColor)
				SetPixel(hDC,x+j,y+i,*pData);
			pData++;		
		}
	}
	return true;
}


BOOL 
OutputBmpObjMemWithTrColor(
	HDC hDC, 
	int x,
	int y,
	int iDestWidth, 
	int iDestHeight,
	HBITMAP hBitmap,
	COLORREF crColor
)
{
	PBITMAP pBitmap;
	int iWidth, iHeight;
	int i,j;
	PCOLORREF pDestData;
	PCOLORREF pSrcData;
	pBitmap=(PBITMAP)hBitmap;
	if(!pBitmap)
		return false;
	iWidth = pBitmap->bmWidth;
	iHeight = pBitmap->bmHeight;
	pDestData = (PCOLORREF)(hDC->pData);
	if(!pDestData)
		return false;
	pDestData += y*iDestWidth+x;
	pSrcData = (PCOLORREF)(pBitmap->bmBits);
	for(i=0;i<iHeight;i++){
		for(j=0;j<iWidth;j++){
			if(*pSrcData != crColor)
			*(pDestData+j) = *pSrcData;
			pSrcData++;
		}
		pDestData += iDestWidth;		
	}
	return true;
}

