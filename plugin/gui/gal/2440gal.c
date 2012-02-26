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
static BOOL Init();
static void UnInit();

static void  
PutPixelRaw(
	int x,
	int y, 
	COLORREF color
);

static COLORREF /*inline*/ 
GetPixelRaw(
	int x,
	int y
);



static void /*inline*/ 
PutLineRaw(
	int xPos,
	int yPos, 
	int iLen, 
	void* pData
);


static void  
GetLineRaw(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
);

static void  
PutRectRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
);

static void  
PutRectWithTrCorRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
);

static void  
GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

static void  
PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
);

static void  
PutBoxWithTrCorRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
);

static void  
EraseRectRaw(
	PRECT lpRc,
	COLORREF color
);

struct GalOps s3c2440GalOps={
	Init,
	UnInit,
	PutPixelRaw,
	GetPixelRaw,
	PutLineRaw,
	GetLineRaw,
	PutRectRaw,
	NULL,//GetRectRaw,
	PutRectWithTrCorRaw,
	PutBoxRaw,
	GetBoxRaw,
	PutBoxWithTrCorRaw,
	EraseRectRaw
};
struct GalDev s3c2440GalDev={
	&s3c2440GalOps,
	0,
	0,
	0
};

static int iFrameWidth;
static int iFrameHeight;
static unsigned char* pFrameBuffer;
static int iLineSize;
static int iBufferSize;
static int iBytesPerPixel = 0;
static int iBytesDataType = 0;
static acoral_screen_info *vInfo;
static BOOL 
Init(){
	acoral_id screen_id;
	screen_id=acoral_dev_open("screen");
	if(screen_id==ACORAL_DEV_ERR_ID){
		acoral_prints("open screen error\n");
	}
	acoral_dev_config(screen_id,CMD_SCREEN_INFO,&vInfo,0);
	iFrameHeight=vInfo->yres;
	iFrameWidth=vInfo->xres;
	iLineSize = vInfo->xres * vInfo->bits_per_pixel / 8;
	iBufferSize = iLineSize * vInfo->yres;
	iBytesPerPixel = vInfo->bits_per_pixel / 8;
	if (iBytesPerPixel == 3)
		iBytesDataType = 4;
	else
		iBytesDataType = iBytesPerPixel;
	pFrameBuffer=vInfo->screen_buf;
	s3c2440GalDev.ScreenHeight=iFrameHeight;
	s3c2440GalDev.ScreenWidth=iFrameWidth;
	s3c2440GalDev.iBytesDataType=iBytesDataType;
	return true;
}

static void 
UnInit(){

}

static void  
PutPixelRaw(
	int x,
	int y, 
	COLORREF color
)
{
	unsigned char* pDest;
	if(y>=iFrameHeight||x>=iFrameWidth)
		return;
	pDest = pFrameBuffer + iLineSize * y + (x * iBytesPerPixel);

	if(iBytesPerPixel == 3){
		*pDest = B(color);
		*(pDest+1) = G(color);
		*(pDest+2) = R(color);
	}
	else	
		*((PCOLORREF)pDest)=color;

}

static COLORREF /*inline*/ 
GetPixelRaw(
	int x,
	int y
)
{
	unsigned char* pDest;
	COLORREF crColor;
	pDest = pFrameBuffer + iLineSize * y + (x * iBytesPerPixel);

	if(iBytesPerPixel == 3)
		crColor = 0xff000000 | RGB(*(pDest+2), *(pDest+1), *pDest);
	else
		crColor = *((PCOLORREF)pDest);
	
	return crColor;
}

static void  
PutRectRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData
){
	int x,y;
	unsigned char* pCurPointer;
	unsigned char* pDest;
	int iOrgWidth;
	int iSourceLeftMarge,iTargetLeftMarge;

	iOrgWidth=(lpOrgRc->right-lpOrgRc->left+1)* iBytesDataType;
	iSourceLeftMarge=(lpOutRc->left-lpOrgRc->left)*iBytesDataType;
	iTargetLeftMarge=lpOutRc->left*iBytesPerPixel;

	if(iBytesPerPixel == 3){
		for(y = lpOutRc->top; y <= lpOutRc->bottom; y++){
			pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top) + iSourceLeftMarge;
			pDest=pFrameBuffer + iLineSize*y + iTargetLeftMarge;

			for(x = lpOutRc->left; x <= lpOutRc->right; x++){
				*pDest = B(*pCurPointer);
				*(pDest+1) = G(*pCurPointer);
				*(pDest+2) = R(*pCurPointer);
				pDest +=3;
				pCurPointer += iBytesDataType;
			}
		}
	}
	else{
		for(y=lpOutRc->top;y<=lpOutRc->bottom;y++){
			pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top) + iSourceLeftMarge;
			pDest=pFrameBuffer + iLineSize*y + iTargetLeftMarge;
			memcpy(pDest,pCurPointer,(lpOutRc->right-lpOutRc->left+1)*iBytesPerPixel);
		}
	}

}

static void  
PutRectWithTrCorRaw(
	PRECT lpOutRc,
	PRECT lpOrgRc,
	void* pData, 
	COLORREF crTrColor
)
{
	int x,y;
	unsigned char* pCurPointer;
	unsigned char* pDest;
	int iOrgWidth;

	iOrgWidth=(lpOrgRc->right-lpOrgRc->left+1)* iBytesDataType;

	if(iBytesPerPixel == 3){
		for(y=lpOutRc->top;y<=lpOutRc->bottom;y++){
			for(x=lpOutRc->left;x<=lpOutRc->right;x++){
				pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top)+(x-lpOrgRc->left)*iBytesDataType;
				if(*((PCOLORREF)pCurPointer)!=crTrColor){
					
					pDest = pFrameBuffer + iLineSize*y + x* iBytesPerPixel;
					
					*pDest = B(*pCurPointer);
					*(pDest+1) = G(*pCurPointer);
					*(pDest+2) = R(*pCurPointer);
					
				}
			}
		}

	}
	else{
		for(y=lpOutRc->top;y<=lpOutRc->bottom;y++){
			for(x=lpOutRc->left;x<=lpOutRc->right;x++){
				pCurPointer=(acoral_u8*)pData + iOrgWidth*(y-lpOrgRc->top)+(x-lpOrgRc->left)*iBytesDataType;
				if(*((PCOLORREF)pCurPointer)!=crTrColor){
					pDest=pFrameBuffer + iLineSize*y + x* iBytesDataType;
					memcpy(pDest,pCurPointer,iBytesDataType);
				}
			}
		}

	}
}


static void  
PutLineRaw(
	int xPos,
	int yPos, 
	int iLen, 
	void* pData
)
{

	int i;
	unsigned char* pDest;
	PCOLORREF pSource;

	pSource = (PCOLORREF)pData;

	pDest=pFrameBuffer + iLineSize*yPos + xPos * iBytesPerPixel;
	if(iBytesPerPixel == 3)
		for(i=0;i<iLen;i++){
			*pDest = B(*pSource);
			*(pDest+1) = G(*pSource);
			*(pDest+2) = R(*pSource);

			pDest +=3;
			pSource += 1;
			
		}
	else
		memcpy(pDest,pData,iLen*iBytesPerPixel);

}


static void  
GetLineRaw(
	int xPos, 
	int yPos, 
	int iLen, 
	void* pData
)
{
	int i;
	unsigned char* pSrc;
	PCOLORREF pTarget;
	
	pSrc=pFrameBuffer + iLineSize*yPos + xPos * iBytesPerPixel;
	
	if(iBytesPerPixel == 3){
		for(i=0;i<iLen;i++){
			*pTarget = 0xff000000 | RGB(*(pSrc+2), *(pSrc+1),*pSrc);
			pTarget +=1;
			pSrc += iBytesPerPixel;
		}
	}
	else
		memcpy(pData,pSrc,iLen*iBytesPerPixel);
}

static void  
EraseRectRaw(
	PRECT lpRc,
	COLORREF color
)
{
	int x,y;
	int iLeftMarge;

	iLeftMarge=lpRc->left;
	for(y=lpRc->top;y<=lpRc->bottom;y++)
		for(x=lpRc->left;x<=lpRc->right;x++)
			PutPixelRaw(x,y,color);

}

static void  
GetBoxRaw(
	int x,
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
	int i,j;
	for(i=0; (i<nHeight)&&(y+i<SCREEN_HEIGHT); i++){
		for(j=0;(j<nWidth)&&(x+j<SCREEN_WIDTH); j++){
			*(pData + i*nWidth + j) = GetPixelRaw(x+j,y+i);
		}
	}
}

static void  
PutBoxRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData
)
{
	int i,j;
	for(i=0; (i<nHeight) && (y + i < SCREEN_HEIGHT); i++){
		for(j=0; (j<nWidth) && (x + j < SCREEN_WIDTH);j++){
			PutPixelRaw(x+j,y+i,*(pData + i*nWidth + j));
		}
	}
}

static void  
PutBoxWithTrCorRaw(
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pData, 
	COLORREF crColor
)
{
	int i,j;
	COLORREF crPointColor;
	for(i=0; (i<nHeight) && (y + i < SCREEN_HEIGHT); i++){
		for(j=0; (j<nWidth) && (x + j < SCREEN_WIDTH);j++){
			crPointColor = *(pData + i*nWidth + j);
			if(crPointColor!=crColor){
				PutPixelRaw(x+j,y+i,crPointColor);
			}
		}
	}
}

