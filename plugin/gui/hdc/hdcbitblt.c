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
#include "../include/semaphore.h"
#include "../include/shmem.h"
#include "../include/blockheap.h"
#include "../include/rect.h"
#include "../include/invalidregion.h"
#include "../include/clipregion.h"
#include "../include/gmessage.h"
#include "../include/hdc.h"
#include "../include/caret.h"
#include "../include/winnc.h"
#include "../include/winbase.h"
#include "../include/window.h"
#include "../include/bmp.h"
#include "../include/gal.h"


#include "../include/hdcbitblt.h"

extern int Gal_iBytesDataType;

//nXDest,nYDest,nXSrc,nYSrc
BOOL 
winBitBlt(
  HDC hdcDest, // handle to destination device context
  int nXDest,  // x-coordinate of destination winRectangle's upper-left
               // corner
  int nYDest,  // y-coordinate of destination winRectangle's upper-left
               // corner
  int nWidth,  // width of destination winRectangle
  int nHeight, // height of destination winRectangle
  HDC hdcSrc,  // handle to source device context
  int nXSrc,   // x-coordinate of source winRectangle's upper-left
               // corner
  int nYSrc,   // y-coordinate of source winRectangle's upper-left
               // corner
  DWORD dwRop  // raster operation code
)
{
	if((hdcDest->iType==DC_TYPE_MEM) && (hdcSrc->iType==DC_TYPE_MEM)){//both mem
		return BitBltM2M(hdcDest,nXDest,nYDest,nWidth,nHeight,hdcSrc,nXSrc,nYSrc,dwRop);
	}
	else if((hdcDest->iType==DC_TYPE_MEM) && (hdcSrc->iType==DC_TYPE_WIN)){//Dest mem
		return BitBltW2M(hdcDest,nXDest,nYDest,nWidth,nHeight,hdcSrc,nXSrc,nYSrc,dwRop);
	}
	else if((hdcDest->iType==DC_TYPE_WIN) && (hdcSrc->iType==DC_TYPE_MEM)){//Source meme
		return BitBltM2W(hdcDest,nXDest,nYDest,nWidth,nHeight,hdcSrc,nXSrc,nYSrc,dwRop);
	}
	else{//both win
		return BitBltW2W(hdcDest,nXDest,nYDest,nWidth,nHeight,hdcSrc,nXSrc,nYSrc,dwRop);
	}

}

BOOL GUIAPI 
BitBlt(
  HDC hdcDest, // handle to destination device context
  int nXDest,  // x-coordinate of destination winRectangle's upper-left
               // corner
  int nYDest,  // y-coordinate of destination winRectangle's upper-left
               // corner
  int nWidth,  // width of destination winRectangle
  int nHeight, // height of destination winRectangle
  HDC hdcSrc,  // handle to source device context
  int nXSrc,   // x-coordinate of source winRectangle's upper-left
               // corner
  int nYSrc,   // y-coordinate of source winRectangle's upper-left
               // corner
  DWORD dwRop  // raster operation code
)
{

	ClientToWindow(hdcDest->hWnd,&nXDest,&nYDest);
	ClientToWindow(hdcSrc->hWnd,&nXSrc,&nYSrc);
	return winBitBlt(hdcDest,nXDest,nYDest,nWidth,nHeight,hdcSrc,nXSrc,nYSrc,dwRop);
}
//mem dc mem dc
BOOL 
BitBltM2M(
	HDC hdcDest, 
	int nXDest, 
	int nYDest, 
	int nWidth, 
	int nHeight,
	HDC hdcSrc, 
	int nXSrc, 
	int nYSrc, 
	DWORD dwRop
)
{
	int iSrcWidth,iSrcHeight;	//width & height of source DC
	int iDestWidth,iDestHeight; //width & height of target DC

	int iSrcRectWidth,iSrcRectHeight;	//width and height clipped by bound of source DC
	int iDestRectWidth,iDestRectHeight;	//width and height clipped by bound of dest DC

	int iCopyWidth,iCopyHeight;			//result area calculated to copy

	int x,y;

	PCOLORREF pSrcData;
	PCOLORREF pDestData;

	RECT rcSrc,rcDest;

	iSrcWidth	=((PBITMAP)(hdcSrc->hBitmap))->bmWidth;
	iSrcHeight	=((PBITMAP)(hdcSrc->hBitmap))->bmHeight;

	iDestWidth	=((PBITMAP)(hdcDest->hBitmap))->bmWidth;
	iDestHeight	=((PBITMAP)(hdcDest->hBitmap))->bmHeight;

	if(nXSrc>=iSrcWidth)
		return false;
	if(nYSrc>=iSrcHeight)
		return false;
	if(nXDest>=iDestWidth)
		return false;
	if(nYDest>=iDestHeight)
		return false;

	pSrcData	=(PCOLORREF)(hdcSrc->pData);
	pDestData	=(PCOLORREF)(hdcDest->pData);

	//Source
	if(nXSrc + nWidth	<= iSrcWidth)
		iSrcRectWidth	=nWidth;
	else
		iSrcRectWidth	=iSrcWidth - nXSrc;

	if(nYSrc + nHeight	<= iSrcHeight)
		iSrcRectHeight	=nHeight;
	else
		iSrcRectHeight	=iSrcHeight - nYSrc;
	//Dest
	if(nXDest + nWidth	<= iDestWidth)
		iDestRectWidth	=nWidth;
	else
		iDestRectWidth	=iDestWidth - nXDest;

	if(nYDest + nHeight	<= iDestHeight)
		iDestRectHeight	=nHeight;
	else
		iDestRectHeight	=iDestHeight - nYDest;


	iCopyWidth	=min(iSrcRectWidth,iDestRectWidth);
	iCopyHeight	=min(iSrcRectHeight,iDestRectHeight);

	//source
	rcSrc.left		=nXSrc;
	rcSrc.top		=nYSrc;
	rcSrc.right		=nXSrc + iCopyWidth - 1;
	rcSrc.bottom	=nYSrc + iCopyHeight -1;

	//Dest
	rcDest.left		=nXDest;
	rcDest.top		=nYDest;
	rcDest.right	=nXDest + iCopyWidth - 1;
	rcDest.bottom	=nYDest + iCopyHeight -1;

	//begin copy here
	switch(dwRop){
	case SRCCOPY:	//	Copies the source winRectangle directly
		//memset(pDestData,255,iCopyWidth*iCopyHeight*Gal_iBytesDataType);
		//break;
//		for(y=0;y<iCopyHeight;y++){
//			memcpy((void*)(pDestData + y*iDestWidth),
//				(void*)(pSrcData  + y*iSrcWidth),
//				   iCopyWidth*Gal_iBytesDataType);
//		}
//		break;
		for(y=0;y<iCopyHeight;y++){
			memcpy((void*)(pDestData + (rcDest.top + y)*iDestWidth + rcDest.left),
					(void*)(pSrcData  + (rcSrc.top + y)*iSrcWidth + rcSrc.left),
				   iCopyWidth*Gal_iBytesDataType);
		}
		break;
	case SRCAND:	//	Combines by using the Boolean AND operator.
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) &
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCPAINT:	//	Combines by using the Boolean OR operator.
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) |
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCINVERT:	//	Combines by using the Boolean XOR operator.
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) ^
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));

			}
		}
		break;
	default:
		break;
	}
	return true;
}


//Win-->mem
BOOL 
BitBltW2M(
	HDC hdcDest, 
	int nXDest, 
	int nYDest, 
	int nWidth, 
	int nHeight, 
	HDC hdcSrc, 
	int nXSrc, 
	int nYSrc, 
	DWORD dwRop
)
{
	int iSrcWidth,iSrcHeight;			//width & height of source DC
	int iDestWidth,iDestHeight;			//width & height of target DC

	int iSrcRectWidth,iSrcRectHeight;	//width and height clipped by bound of source DC
	int iDestRectWidth,iDestRectHeight;	//width and height clipped by bound of dest DC

	int iCopyWidth,iCopyHeight;			//result area calculated to copy

	int x,y;

	PCOLORREF pDestData;
	void* pData;
	RECT rcSrc,rcDest;
	RECT rcOut;

	PWindowsTree pWin;

	PClipRect	pClipRect;

	pWin=(PWindowsTree)(hdcSrc->hWnd);

	iSrcWidth	=pWin->rect.right - pWin->rect.left +1;
	iSrcHeight	=pWin->rect.bottom - pWin->rect.top +1;

	iDestWidth	=((PBITMAP)(hdcDest->hBitmap))->bmWidth;
	iDestHeight	=((PBITMAP)(hdcDest->hBitmap))->bmHeight;

	ClientToScreen(hdcSrc->hWnd,&nXSrc,&nYSrc);

	if(nXSrc>pWin->rect.right)
		return false;
	if(nYSrc>pWin->rect.bottom)
		return false;
	if(nXDest>=iSrcWidth)
		return false;
	if(nYDest>=iSrcHeight)
		return false;

	pDestData	=(PCOLORREF)(hdcDest->pData);

	//Dest
	if(nXDest + nWidth	<= iDestWidth)
		iDestRectWidth	=nWidth;
	else
		iDestRectWidth	=iDestWidth - nXDest;

	if(nYDest + nHeight	<= iDestHeight)
		iDestRectHeight	=nHeight;
	else
		iDestRectHeight	=iDestHeight - nYDest;

	//Src
	if(nXSrc + nWidth	<= pWin->rect.right)
		iSrcRectWidth	=nWidth;
	else
		iSrcRectWidth	=pWin->rect.right - nXSrc + 1;

	if(nYSrc + nHeight	<= pWin->rect.bottom)
		iSrcRectHeight	=nHeight;
	else
		iSrcRectHeight	=pWin->rect.bottom - nYSrc + 1;

	iCopyWidth	=min(iSrcRectWidth,iDestRectWidth);
	iCopyHeight	=min(iSrcRectHeight,iDestRectHeight);
	
	//source
	rcSrc.left		=nXSrc;
	rcSrc.top		=nYSrc;
	rcSrc.right		=nXSrc + iCopyWidth - 1;
	rcSrc.bottom	=nYSrc + iCopyHeight -1;

	//Dest
	rcDest.left		=nXDest;
	rcDest.top		=nYDest;
	rcDest.right	=nXDest + iCopyWidth - 1;
	rcDest.bottom	=nYDest + iCopyHeight -1;

	//begin copy here
	switch(dwRop){
	case SRCCOPY:	//	Copies the source winRectangle directly
		pClipRect=pWin->pClipRgn->pHead;
		while(pClipRect){
			if(IntersectRect(&rcOut,&(pClipRect->rect),&rcSrc)){
				for(y=rcOut.top;y<=rcOut.bottom;y++){
					pData=(void*)(pDestData + (rcDest.top + y - rcOut.top)*iDestWidth + rcDest.left);
					Gal_GetLine(rcOut.left,y,rcOut.right-rcOut.left+1,pData);
				}
			}
			pClipRect=pClipRect->pNext;
		}
		break;
	case SRCAND:	//	Combines by using the Boolean AND operator.
		ScreenToClientRect(hdcDest->hWnd,&rcDest);
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) &
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCPAINT:	//	Combines by using the Boolean OR operator.
		ScreenToClientRect(hdcDest->hWnd,&rcDest);
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top, 
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) |
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCINVERT:	//	Combines by using the Boolean XOR operator.
		ScreenToClientRect(hdcDest->hWnd,&rcDest);
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) ^
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	default:
		break;
	}
	return true;
}
//Mem --> Win
BOOL 
BitBltM2W(
	HDC hdcDest, 
	int nXDest, 
	int nYDest, 
	int nWidth, 
	int nHeight,
	HDC hdcSrc, 
	int nXSrc, 
	int nYSrc, 
	DWORD dwRop
)
{
	int iSrcWidth,iSrcHeight;			//width & height of source DC
	int iDestWidth,iDestHeight;			//width & height of target DC

	int iSrcRectWidth,iSrcRectHeight;	//width and height clipped by bound of source DC
	int iDestRectWidth,iDestRectHeight;	//width and height clipped by bound of dest DC

	int iCopyWidth,iCopyHeight;			//result area calculated to copy

	int x,y;

	PCOLORREF pSrcData;
	void* pData;

	RECT rcSrc,rcDest;
	RECT rcOut;

	PWindowsTree pWin;
	//PClipRegion	pClipRgn;
	PClipRect	pClipRect;

	pWin=(PWindowsTree)(hdcDest->hWnd);

	iDestWidth	=pWin->rect.right - pWin->rect.left +1;
	iDestHeight	=pWin->rect.bottom - pWin->rect.top +1;

	iSrcWidth	=((PBITMAP)(hdcSrc->hBitmap))->bmWidth;
	iSrcHeight	=((PBITMAP)(hdcSrc->hBitmap))->bmHeight;

	WindowToScreen(hdcDest->hWnd,&nXDest,&nYDest);

	if(nXDest>pWin->rect.right)
		return false;
	if(nYDest>pWin->rect.bottom)
		return false;
	if(nXSrc>=iSrcWidth)
		return false;
	if(nYSrc>=iSrcHeight)
		return false;


	pSrcData	=(PCOLORREF)(hdcSrc->pData);

	//Source
	if(nXSrc + nWidth	<= iSrcWidth)
		iSrcRectWidth	=nWidth;
	else
		iSrcRectWidth	=iSrcWidth - nXSrc;

	if(nYSrc + nHeight	<= iSrcHeight)
		iSrcRectHeight	=nHeight;
	else
		iSrcRectHeight	=iSrcHeight - nYSrc;
	//Dest
	if(nXDest + nWidth	<= pWin->rect.right)
		iDestRectWidth	=nWidth;
	else
		iDestRectWidth	=pWin->rect.right - nXDest + 1;

	if(nYDest + nHeight	<= pWin->rect.bottom)
		iDestRectHeight	=nHeight;
	else
		iDestRectHeight	=pWin->rect.bottom - nYDest + 1;

	iCopyWidth	=min(iSrcRectWidth,iDestRectWidth);
	iCopyHeight	=min(iSrcRectHeight,iDestRectHeight);

	//source
	rcSrc.left		=nXSrc;
	rcSrc.top		=nYSrc;
	rcSrc.right		=nXSrc + iCopyWidth - 1;
	rcSrc.bottom	=nYSrc + iCopyHeight -1;

	//Dest
	rcDest.left		=nXDest;
	rcDest.top		=nYDest;
	rcDest.right	=nXDest + iCopyWidth - 1;
	rcDest.bottom	=nYDest + iCopyHeight -1;

	//begin copy here

	switch(dwRop){
	case SRCCOPY:	//	Copies the source winRectangle directly
		pClipRect=pWin->pClipRgn->pHead;

		if(IsIntersect(&(pWin->pClipRgn->rcBound),&rcDest)){

			RequestPaint(&rcDest);


			while(pClipRect){
				if(IntersectRect(&rcOut,&(pClipRect->rect),&rcDest)){
					for(y=rcOut.top;y<=rcOut.bottom;y++){
						pData=(void*)(pSrcData
							+ ((rcSrc.top + y - rcDest.top)*iSrcWidth
							+ rcSrc.left + rcOut.left - rcDest.left));
						Gal_PutLineRaw(rcOut.left,y,rcOut.right-rcOut.left+1,pData);
					}
				}
				pClipRect=pClipRect->pNext;
			}
			CompletePaint();

		}

		break;
	case SRCAND:	
		ScreenToClientRect(hdcDest->hWnd,&rcDest);
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) &
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCPAINT:	//	Combines by using the Boolean OR operator.
		ScreenToClientRect(hdcDest->hWnd,&rcDest);
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) |
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCINVERT:	//	Combines by using the Boolean XOR operator.
		ScreenToClientRect(hdcDest->hWnd,&rcDest);
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top,
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) ^
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	default:
		break;
	}
	return true;
}

//Win->Win
BOOL 
BitBltW2W(
	HDC hdcDest, 
	int nXDest, 
	int nYDest, 
	int nWidth, 
	int nHeight,
	HDC hdcSrc, 
	int nXSrc, 
	int nYSrc, 
	DWORD dwRop
)
{
	int iSrcWidth,iSrcHeight;			//width & height of source DC
	int iDestWidth,iDestHeight;			//width & height of target DC

	int iSrcRectWidth,iSrcRectHeight;	//width and height clipped by bound of source DC
	int iDestRectWidth,iDestRectHeight;	//width and height clipped by bound of dest DC

	int iCopyWidth,iCopyHeight;			//result area calculated to copy

	int x,y;

	RECT rcSrc,rcDest;

	PWindowsTree pSrcWin,pDestWin;


	pSrcWin=(PWindowsTree)(hdcSrc->hWnd);
	pDestWin=(PWindowsTree)(hdcDest->hWnd);

	iSrcWidth	=pSrcWin->rect.right - pSrcWin->rect.left +1;
	iSrcHeight	=pSrcWin->rect.bottom - pSrcWin->rect.top +1;

	iDestWidth	=pDestWin->rect.right - pDestWin->rect.left +1;
	iDestHeight	=pDestWin->rect.bottom - pDestWin->rect.top +1;

	ClientToScreen(hdcSrc->hWnd,&nXSrc,&nYSrc);
	
	ClientToScreen(hdcSrc->hWnd,&nXSrc,&nYSrc);

	if(nXSrc>pSrcWin->rect.right)
		return false;
	if(nYSrc>pSrcWin->rect.bottom)
		return false;

	if(nXDest>pDestWin->rect.right)
		return false;
	if(nYDest>pDestWin->rect.bottom)
		return false;

	//Dest
	if(nXDest + nWidth	<= pDestWin->rect.right)
		iDestRectWidth	=nWidth;
	else
		iDestRectWidth	=pDestWin->rect.right - nXDest + 1;

	if(nYDest + nHeight	<= pDestWin->rect.bottom)
		iDestRectHeight	=nHeight;
	else
		iDestRectHeight	=pDestWin->rect.bottom - nYDest;
	
	//Src
	if(nXSrc + nWidth	<= pSrcWin->rect.right)
		iSrcRectWidth	=nWidth;
	else
		iSrcRectWidth	=pSrcWin->rect.right - nXSrc + 1;

	if(nYSrc + nHeight	<= pSrcWin->rect.bottom)
		iSrcRectHeight	=nHeight;
	else
		iSrcRectHeight	=pSrcWin->rect.bottom - nYSrc + 1;

	iCopyWidth	=min(iSrcRectWidth,iDestRectWidth);
	iCopyHeight	=min(iSrcRectHeight,iDestRectHeight);
	
	//source
	rcSrc.left		=nXSrc;
	rcSrc.top		=nYSrc;
	rcSrc.right		=nXSrc + iCopyWidth - 1;
	rcSrc.bottom	=nYSrc + iCopyHeight -1;

	//Dest
	rcDest.left		=nXDest;
	rcDest.top		=nYDest;
	rcDest.right	=nXDest + iCopyWidth - 1;
	rcDest.bottom	=nYDest + iCopyHeight -1;

	//begin copy here
	ScreenToClientRect(hdcDest->hWnd,&rcDest);
	switch(dwRop){
	case SRCCOPY:	//	Copies the source winRectangle directly
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top, 
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCAND:	//	Combines by using the Boolean AND operator.
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top, 
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) &
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCPAINT:	//	Combines by using the Boolean OR operator.
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top, 
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) |
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	case SRCINVERT:	//	Combines by using the Boolean XOR operator.
		for(y=0;y<iCopyHeight;y++){
			for(x=0;x<iCopyWidth;x++){
				winSetPixel(hdcDest, x+rcDest.left, y+rcDest.top, 
					winGetPixel(hdcDest,x+rcDest.left, y+rcDest.top) ^
					winGetPixel(hdcSrc,x+rcSrc.left,y+rcSrc.top));
			}
		}
		break;
	default:
		break;
	}
	return true;
}

