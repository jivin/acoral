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
#include "../include/gdi.h"
#include "../include/gal.h"
#include "../include/mouse.h"


extern int Gal_iBytesDataType;



BOOL GUIAPI 
CreateStockObject()
{
	char pString[100];

	FILE_P fd;
	char* pointer;
	char* pShmAddr;
	int* pIdxAddr;
	int iObjOffset;
	int size;
	PEN		pen;
	BRUSH	brush;
	FONT	font;

	pShmAddr=GetShmAddr();
	if(!pShmAddr)
		return false;
	pIdxAddr=(int*)(pShmAddr+SHMEM_GDI_INDEX_OFFSET);
	iObjOffset=SHMEM_GDI_OFFSET;


	//#define WHITE_PEN			0x00000000
	pen.iGdiType	=GDI_PEN;
	pen.iPenStyle	=PS_SOLID;
	pen.iPenWidth		=1;
	pen.crPenColor	=RGB(0xff,0xff,0xff);
	memcpy((void*)(pShmAddr+iObjOffset),&pen,sizeof(PEN));
	*(pIdxAddr+WHITE_PEN)=iObjOffset;
	iObjOffset+=sizeof(PEN);



	//#define LTGRAY_PEN		0x00000001
	pen.iGdiType	=GDI_PEN;
	pen.iPenStyle	=PS_SOLID;
	pen.iPenWidth		=1;
	pen.crPenColor	=RGB(0xd4,0xd0,0xc8);
	memcpy((void*)(pShmAddr+iObjOffset),&pen,sizeof(PEN));	
	*(pIdxAddr+LTGRAY_PEN)=iObjOffset;
	iObjOffset+=sizeof(PEN);

	//#define GRAY_PEN			0x00000002
	pen.iGdiType	=GDI_PEN;
	pen.iPenStyle	=PS_SOLID;
	pen.iPenWidth		=1;
	pen.crPenColor	=RGB(0xc0,0xc0,0xc0);
	memcpy((void*)(pShmAddr+iObjOffset),&pen,sizeof(PEN));
	*(pIdxAddr+GRAY_PEN)=iObjOffset;
	iObjOffset+=sizeof(PEN);


	//#define DKGRAY_PEN		0x00000003
	pen.iGdiType	=GDI_PEN;
	pen.iPenStyle	=PS_SOLID;
	pen.iPenWidth		=1;
	pen.crPenColor	=RGB(0x80,0x80,0x80);
	memcpy((void*)(pShmAddr+iObjOffset),&pen,sizeof(PEN));
	*(pIdxAddr+DKGRAY_PEN)=iObjOffset;
	iObjOffset+=sizeof(PEN);

	//#define BLACK_PEN			0x00000004
	pen.iGdiType	=GDI_PEN;
	pen.iPenStyle	=PS_SOLID;
	pen.iPenWidth		=1;
	pen.crPenColor	=RGB(0x00,0x00,0x00);
	memcpy((void*)(pShmAddr+iObjOffset),&pen,sizeof(PEN));
	*(pIdxAddr+BLACK_PEN)=iObjOffset;
	iObjOffset+=sizeof(PEN);

	//#define NULL_PEN			0x00000005
	pen.iGdiType	=GDI_PEN;
	pen.iPenStyle	=PS_NULL;
	pen.iPenWidth		=1;
	pen.crPenColor	=RGB(0x00,0x00,0x00);
	memcpy((void*)(pShmAddr+iObjOffset),&pen,sizeof(PEN));
	*(pIdxAddr+NULL_PEN)=iObjOffset;
	iObjOffset+=sizeof(PEN);


	//#define WHITE_BRUSH		0x00000006
	brush.iGdiType		=GDI_BRUSH;
	brush.iBrushStyle	=BS_SOLID;	// brush style
	brush.crBrushColor	=RGB(0xFF,0xFF,0xFF);
	memcpy((void*)(pShmAddr+iObjOffset),&brush,sizeof(BRUSH));
	*(pIdxAddr+WHITE_BRUSH)=iObjOffset;
	iObjOffset+=sizeof(BRUSH);


	//#define LTGRAY_BRUSH		0x00000007
	brush.iGdiType		=GDI_BRUSH;
	brush.iBrushStyle	=BS_SOLID;	// brush style
	brush.crBrushColor	=RGB(0xd4,0xd0,0xc8);
	memcpy((void*)(pShmAddr+iObjOffset),&brush,sizeof(BRUSH));
	*(pIdxAddr+LTGRAY_BRUSH)=iObjOffset;
	iObjOffset+=sizeof(BRUSH);

	//#define GRAY_BRUSH		0x00000008
	brush.iGdiType		=GDI_BRUSH;
	brush.iBrushStyle	=BS_SOLID;	// brush style
	brush.crBrushColor	=RGB(0xc0,0xc0,0xc0);
	memcpy((void*)(pShmAddr+iObjOffset),&brush,sizeof(BRUSH));
	*(pIdxAddr+GRAY_BRUSH)=iObjOffset;
	iObjOffset+=sizeof(BRUSH);

	//#define DKGRAY_BRUSH		0x00000009
	brush.iGdiType		=GDI_BRUSH;
	brush.iBrushStyle	=BS_SOLID;	// brush style
	brush.crBrushColor	=RGB(0x80,0x80,0x80);
	memcpy((void*)(pShmAddr+iObjOffset),&brush,sizeof(BRUSH));
	*(pIdxAddr+DKGRAY_BRUSH)=iObjOffset;
	iObjOffset+=sizeof(BRUSH);
	
	//#define BLACK_BRUSH		0x0000000A
	brush.iGdiType		=GDI_BRUSH;
	brush.iBrushStyle	=BS_SOLID;	// brush style
	brush.crBrushColor	=RGB(0x00,0x00,0x00);
	memcpy((void*)(pShmAddr+iObjOffset),&brush,sizeof(BRUSH));
	*(pIdxAddr+BLACK_BRUSH)=iObjOffset;
	iObjOffset+=sizeof(BRUSH);

	//#define NULL_BRUSH		0x0000000B
	brush.iGdiType		=GDI_BRUSH;
	brush.iBrushStyle	=BS_NULL;	// brush style
	brush.crBrushColor	=RGB(0x00,0x00,0x00);
	memcpy((void*)(pShmAddr+iObjOffset),&brush,sizeof(BRUSH));
	*(pIdxAddr+NULL_BRUSH)=iObjOffset;
	iObjOffset+=sizeof(BRUSH);	

		
	//#define FONT_16_16			0x0000000C
	strcpy(pString,pEnvStr);
	strcat(pString,FONT_16_16_SPEC);
	fd=OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	pointer=pShmAddr+SHMEM_FONT_16_16_OFFSET;
	while(size=READ(fd,pointer,512)){
		pointer+=size;
	}

	CLOSE(fd);

	font.iGdiType		=GDI_FONT;
	font.iFontStyle		=FONT_16_16;	// font style
	font.iOffset		=SHMEM_FONT_16_16_OFFSET;

	memcpy(&(font.FontLibHeader),
		(void*)(pShmAddr+SHMEM_FONT_16_16_OFFSET),
		sizeof(FONTLIBHEADER));

	memcpy((void*)(pShmAddr+iObjOffset),&font,sizeof(FONT));	
	
	*(pIdxAddr+FONT_16_16)=iObjOffset;
	iObjOffset+=sizeof(FONT);


	
	//#define FONT_14_14			0x0000000D					
	strcpy(pString,pEnvStr);
	strcat(pString,FONT_14_14_SPEC);
	fd=OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	pointer=pShmAddr+SHMEM_FONT_14_14_OFFSET;
	while(size=READ(fd,pointer,512)){
		pointer+=size;
	}

	CLOSE(fd);
	
	font.iGdiType		=GDI_FONT;
	font.iFontStyle		=FONT_14_14;	// font style
	font.iOffset		=SHMEM_FONT_14_14_OFFSET;
	
	
	memcpy(&(font.FontLibHeader),
		(void*)(pShmAddr+SHMEM_FONT_14_14_OFFSET),
		sizeof(FONTLIBHEADER));

	memcpy((void*)(pShmAddr+iObjOffset),&font,sizeof(FONT));			
	
	*(pIdxAddr+FONT_14_14)=iObjOffset;
	iObjOffset+=sizeof(FONT);

	//#define FONT_12_12			0x0000000E
	strcpy(pString,pEnvStr);
	strcat(pString,FONT_12_12_SPEC);
	fd=OPEN(pString,RDONLY);
	if(fd<0)
		return false;
	pointer=pShmAddr+SHMEM_FONT_12_12_OFFSET;
	while(size=READ(fd,pointer,512)){
		pointer+=size;
	}

	CLOSE(fd);
	
	font.iGdiType		=GDI_FONT;
	font.iFontStyle		=FONT_12_12;	// font style
	font.iOffset		=SHMEM_FONT_12_12_OFFSET;
	memcpy(&(font.FontLibHeader),
		(void*)(pShmAddr+SHMEM_FONT_12_12_OFFSET),
		sizeof(FONTLIBHEADER));

	memcpy((void*)(pShmAddr+iObjOffset),&font,sizeof(FONT));	
	*(pIdxAddr+FONT_12_12)=iObjOffset;
	iObjOffset+=sizeof(FONT);

	return true;		
}



HGDIOBJ	GUIAPI 
GetStockObject(
	int iStyle
)
{
	char* pShmAddr;
	int iObjOffset;
	int iType;
	PEN* pPen;
	BRUSH* pBrush;
	FONT* pFont;

	pShmAddr=GetShmAddr();
	if(!pShmAddr)
		return false;
	
	
	iObjOffset=*(((int*)(pShmAddr+SHMEM_GDI_INDEX_OFFSET))+iStyle);
	//get gdi type
	shm_read((char*)(&iType),iObjOffset,sizeof(int));
	switch(iType){
	case GDI_PEN:
		pPen=(PEN*)malloc(sizeof(PEN));
		if(!pPen)
			return NULL;
		shm_read((char*)pPen,iObjOffset,sizeof(PEN));
		return (HGDIOBJ)pPen;

	case GDI_BRUSH:
		pBrush=(BRUSH*)malloc(sizeof(BRUSH));
		if(!pBrush)
			return NULL;
		shm_read((char*)pBrush,iObjOffset,sizeof(BRUSH));
		return (HGDIOBJ)pBrush;
	case GDI_FONT:
		pFont=(FONT*)malloc(sizeof(FONT));
		if(!pFont)
			return NULL;
		shm_read((char*)pFont,iObjOffset,sizeof(FONT));
		return (HGDIOBJ)pFont;
	}
	return NULL;
}


HDC GUIAPI 
GetDC(
	 HWND hWnd
)
{
	HDC hDC;
	BITMAP *pBitmap;
	hDC=malloc(sizeof(DC));
	if(!hDC)
		return NULL;
	memset(hDC,0,sizeof(DC));

	pBitmap=malloc(sizeof(BITMAP));
	if(!pBitmap)
		return NULL;

	memset(pBitmap,0,sizeof(BITMAP));
	hDC->iType=DC_TYPE_WIN;	
	hDC->iCoordType = DC_COORDTYPE_CLIENT;
	hDC->hWnd=hWnd;

	hDC->isTextBkTrans	=true;
	hDC->crTextBkColor	=RGB(0xff, 0xff, 0xff);
	hDC->crTextColor	=RGB(0x00, 0x00, 0x00);

	hDC->hPen	=(HPEN)(GetStockObject(BLACK_PEN));
	hDC->hBrush	=(HBRUSH)(GetStockObject(NULL_BRUSH));
	hDC->hFont	=(HFONT)(GetStockObject(SYS_DEFAULT_FONT));
	hDC->hBitmap=(HBITMAP)pBitmap;
	hDC->pData	=NULL;

	return hDC;
}



HDC GUIAPI 
GetWindowDC(
	 HWND hWnd
)
{
	HDC hDC;
	BITMAP *pBitmap;
	hDC=malloc(sizeof(DC));
	if(!hDC)
		return NULL;
	memset(hDC,0,sizeof(DC));

	pBitmap=malloc(sizeof(BITMAP));
	if(!pBitmap)
		return NULL;

	memset(pBitmap,0,sizeof(BITMAP));
	hDC->iType=DC_TYPE_WIN;	
	hDC->iCoordType = DC_COORDTYPE_WINDOW;
	hDC->hWnd=hWnd;

	hDC->isTextBkTrans	=true;
	hDC->crTextBkColor	=RGB(0xff, 0xff, 0xff);
	hDC->crTextColor	=RGB(0x00, 0x00, 0x00);

	hDC->hPen	=(HPEN)(GetStockObject(BLACK_PEN));
	hDC->hBrush	=(HBRUSH)(GetStockObject(NULL_BRUSH));
	hDC->hFont	=(HFONT)(GetStockObject(SYS_DEFAULT_FONT));
	hDC->hBitmap=(HBITMAP)pBitmap;
	hDC->pData	=NULL;

	return hDC;
}
HPEN GUIAPI 
CreatePen(
	int iPenStyle, 
	int iWidth, 
	COLORREF crColor
)
{
	PEN* pPen;
	pPen=(PEN*)malloc(sizeof(PEN));
	if(!pPen)
		return NULL;
	pPen->iGdiType	=GDI_PEN;
	if(iPenStyle==0)
		pPen->iPenStyle=PS_SOLID;
	else		
		pPen->iPenStyle	=iPenStyle;
	pPen->iPenWidth	=iWidth;
	pPen->crPenColor=crColor;
	return (HPEN)pPen;
}


HBRUSH GUIAPI 
CreateSolidBrush(
	COLORREF crColor
)
{
	BRUSH* pBrush;
	pBrush=(BRUSH*)malloc(sizeof(BRUSH));
	if(!pBrush)
		return NULL;
	pBrush->iGdiType	=GDI_BRUSH;
	pBrush->iBrushStyle	=BS_SOLID;
	pBrush->crBrushColor=crColor;
	return (HBRUSH)pBrush;
}


HBRUSH GUIAPI 
CreateHatchBrush(
	int iStyle, 
	COLORREF crColor
)
{
	BRUSH* pBrush;
	pBrush=(BRUSH*)malloc(sizeof(BRUSH));
	if(!pBrush)
		return NULL;
	pBrush->iGdiType	=GDI_BRUSH;
	pBrush->iBrushStyle	=BS_HATCHED;
	pBrush->iHatchStyle	=iStyle;
	pBrush->crBrushColor=crColor;
	
	return (HBRUSH)pBrush;
}


HBRUSH GUIAPI 
CreateAgainstBrush()
{
	BRUSH* pBrush;
	pBrush=(BRUSH*)malloc(sizeof(BRUSH));
	if(!pBrush)
		return NULL;
	pBrush->iGdiType	=GDI_BRUSH;
	pBrush->iBrushStyle	=BS_AGAINST;
	
	return (HBRUSH)pBrush;
}


BOOL 
IsNullBrush(HDC hDC)
{
	BRUSH* pBrush;
	pBrush = (BRUSH*)(hDC->hBrush);
	if(pBrush->iBrushStyle == BS_NULL)
		return true;
	else
		return false;
}


BOOL  
IsSolidBrush(HDC hDC)
{
	BRUSH* pBrush;
	pBrush = (BRUSH*)(hDC->hBrush);
	if(pBrush->iBrushStyle == BS_SOLID)
		return true;
	else
		return false;
}


COLORREF 
GetSolidBrushColor(
	HDC hDC
)
{
	BRUSH* pBrush;
	pBrush = (BRUSH*)(hDC->hBrush);
	return pBrush->crBrushColor;
}



int GUIAPI 
ReleaseDC(
	HWND hWnd, 
	HDC hDC
)
{
	if(!hDC)
		return -1;
	if(hDC->hPen)
		free(hDC->hPen);
	if(hDC->hBrush)
		free(hDC->hBrush);
	if(hDC->hFont)
		free(hDC->hFont);
	if(((BITMAP*)(hDC->hBitmap))->bmBits)
		free(((BITMAP*)(hDC->hBitmap))->bmBits);
	if(hDC->hBitmap)
		free(hDC->hBitmap);
	if(hDC->pData)
		bfree(hDC->pData);	
	free(hDC);
	return 0;
}


BOOL GUIAPI 
SelectObject(
	HDC hDC,
	HGDIOBJ hGdiObj
)
{
	int iType;
	iType= *((int*)hGdiObj);
	switch (iType){
	case GDI_PEN:
		memcpy((void*)(hDC->hPen),(void*)hGdiObj,sizeof(PEN));
		return true;
	case GDI_BRUSH:
		memcpy((void*)(hDC->hBrush),(void*)hGdiObj,sizeof(BRUSH));
		return true;
	case GDI_FONT:
		memcpy((void*)(hDC->hFont),(void*)hGdiObj,sizeof(FONT));
		return true;
	case GDI_BITMAP:
		if(hDC->iType!=DC_TYPE_MEM)
			return false;
		if(hDC->pData){
			if(
				((BITMAP*)hGdiObj)->bmHeight * ((BITMAP*)hGdiObj)->bmWidth !=
				((BITMAP*)(hDC->hBitmap))->bmHeight *((BITMAP*)(hDC->hBitmap))->bmWidth
				){

				bfree(hDC->pData);
				hDC->pData=bmalloc(((BITMAP*)(hGdiObj))->bmWidth *
					              ((BITMAP*)(hGdiObj))->bmHeight *
								  Gal_iBytesDataType);
				if(!(hDC->pData)){
					return false;
				}
			}
		}
		else{
			hDC->pData=bmalloc(((BITMAP*)(hGdiObj))->bmWidth *
				              ((BITMAP*)(hGdiObj))->bmHeight *
							  Gal_iBytesDataType);
			if(!(hDC->pData))
				return false;
		}

		memcpy(hDC->hBitmap,
			   hGdiObj,sizeof(BITMAP));

		memcpy(hDC->pData,
			(void*)(((BITMAP*)hGdiObj)->bmBits),
			((BITMAP*)hGdiObj)->bmWidth * ((BITMAP*)hGdiObj)->bmHeight * Gal_iBytesDataType);
		return true;
	case GDI_RES:
		break;
	}
	return false;
}


void GUIAPI 
DeleteObject(
	HGDIOBJ hGdiObj
)
{
	BITMAP* pBitmap;
	if(*((int*)hGdiObj)==GDI_BITMAP){
		pBitmap=(BITMAP*)hGdiObj;
		if(pBitmap->bmBits)
			free(pBitmap->bmBits);
	}
	free(hGdiObj);
}



HDC GUIAPI 
CreateCompatibleDC(
	HDC hDCRefer
)
{
	HDC hDC;
	PWindowsTree pWin;
	BITMAP *pBitmap;

	hDC=(HDC)malloc(sizeof(DC));
	if(!hDC)
		return NULL;
	memset(hDC,0,sizeof(DC));

	pBitmap=malloc(sizeof(BITMAP));
	if(!pBitmap)
		return NULL;
	memset(pBitmap,0,sizeof(BITMAP));
	////////////////////////////////////////////////
	hDC->iType=DC_TYPE_MEM;	
	////////////////////////////////////////////////


	if(hDCRefer){
		hDC->iCoordType = hDCRefer->iCoordType;
	
		hDC->isTextBkTrans	=hDCRefer->isTextBkTrans;
		hDC->crTextBkColor	=hDCRefer->crTextBkColor;
		hDC->crTextColor	=hDCRefer->crTextColor;

		pWin=(PWindowsTree)(hDCRefer->hWnd);
		pBitmap->bmWidth=pWin->rect.right-pWin->rect.left + 1;
		pBitmap->bmHeight=pWin->rect.bottom-pWin->rect.top + 1;

		hDC->pData=bmalloc(pBitmap->bmWidth * pBitmap->bmHeight * Gal_iBytesDataType);
		if(!(hDC->pData)){
			free(pBitmap);
			return NULL;
		}
		hDC->hPen=(HPEN)malloc(sizeof(PEN));
		if(!(hDC->hPen)){
			free(pBitmap);
			bfree(hDC->pData);
			return NULL;
		}
		hDC->hBrush=(HBRUSH)malloc(sizeof(BRUSH));
		if(!(hDC->hBrush)){
			free(pBitmap);
			free(hDC->hPen);
			bfree(hDC->pData);
			return NULL;
		}
		hDC->hFont=(HFONT)malloc(sizeof(FONT));
		if(!(hDC->hFont)){
			free(pBitmap);
			free(hDC->hPen);
			free(hDC->hBrush);
			bfree(hDC->pData);
			return NULL;
		}
		memcpy((void*)(hDC->hPen),(void*)(hDCRefer->hPen),sizeof(PEN));
		memcpy((void*)(hDC->hBrush),(void*)(hDCRefer->hBrush),sizeof(BRUSH));
		memcpy((void*)(hDC->hFont),(void*)(hDCRefer->hFont),sizeof(FONT));
		hDC->hBitmap=(HBITMAP)pBitmap;
		hDC->hWnd = hDCRefer->hWnd;
	}
	else{
		hDC->isTextBkTrans	=true;
		hDC->crTextBkColor	=RGB(0xff, 0xff, 0xff);
		hDC->crTextColor	=RGB(0x00, 0x00, 0x00);

		pBitmap->bmWidth=SCREEN_WIDTH;
		pBitmap->bmHeight=SCREEN_HEIGHT;
		hDC->pData=bmalloc(pBitmap->bmWidth * pBitmap->bmHeight * Gal_iBytesDataType);
		if(!(hDC->pData)){
			free(pBitmap);
			return NULL;
		}
		hDC->hPen	=(HPEN)(GetStockObject(BLACK_PEN));
		hDC->hBrush	=(HBRUSH)(GetStockObject(NULL_BRUSH));
		hDC->hFont	=(HFONT)(GetStockObject(SYS_DEFAULT_FONT));
		hDC->hBitmap=(HBITMAP)pBitmap;
		hDC->hWnd	= _lGUI_pWindowsTree;

	}
	return hDC;
}



HBITMAP GUIAPI 
CreateBitmap(
	char* pFile
)
{
	FILE_P fd;
	BITMAP* pBitmap;
	BMPINFOHEADER infoHeader;

	fd=OPEN(pFile,RDONLY);
	if(fd<0){
		printerror("Open BitMap File Error\n");
		return NULL;
	}
	pBitmap=(BITMAP*)malloc(sizeof(BITMAP));
	if(!pBitmap){
		printerror("CreateBitmap BitMap Malloc Error\n");
		return NULL;
	}
	memset(pBitmap,0,sizeof(BITMAP));
	pBitmap->iGdiType=GDI_BITMAP;
	pBitmap->bmType=0;

	SEEK(fd,0x12,0);
	READ(fd,&(infoHeader.biWidth),sizeof(infoHeader.biWidth));
	READ(fd,&(infoHeader.biHeight),sizeof(infoHeader.biHeight));
	
	//READ(&fileHeader,sizeof(BMPFILEHEADER),1,fd);
	//READ(&infoHeader,sizeof(BMPINFOHEADER),1,fd);

	pBitmap->bmWidth=infoHeader.biWidth;
	pBitmap->bmHeight=infoHeader.biHeight;
	//pBitmap->bmPlanes=infoHeader.biPlanes;
	//pBitmap->bmBitsPixel=infoHeader.biBitCount;
	pBitmap->bmBits=(unsigned char*)malloc(pBitmap->bmWidth*pBitmap->bmHeight*Gal_iBytesDataType);
	if(!(pBitmap->bmBits)){
		free(pBitmap);
		CLOSE(fd);
		return NULL;
	}
	if(!ReadBmpToBuf(fd,pBitmap->bmWidth,pBitmap->bmHeight,pBitmap->bmBits)){
		free(pBitmap->bmBits);
		free(pBitmap);
		CLOSE(fd);
		return NULL;
	}

	CLOSE(fd);
	return (HBITMAP)pBitmap;
}



void 
PutSavedBoxOnDC(
	HDC hDC,
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pSavedBox
)
{
	int i,j;
	COLORREF crColor;
	for(i=0; i<nWidth; i++){
		for(j=0; j<nHeight; j++){
			crColor = *(pSavedBox + i*nWidth + j);
			SetPixel(hDC,x+i,y+j,crColor);
		}
	}
}

