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
#include "../include/bmp.h"

static BOOL 
ReadBmp2Buf1 (
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
);

static BOOL 
ReadBmp2Buf4 (
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
);


static BOOL 
ReadBmp2Buf8 (
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
);


static BOOL 
ReadBmp2Buf24(
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
);


BOOL
ReadBmpToBuf(
	FILE_P fd, 
	int iWidth, 
	int iHeight, 
	void* pData
)
{
	BMPINFOHEADER infoHeader;
	SEEK(fd,0x1c,0);
	READ(fd,&(infoHeader.biBitCount),sizeof(infoHeader.biBitCount));
	switch(infoHeader.biBitCount){
	case 1:
		return ReadBmp2Buf1 (fd, iWidth, iHeight, pData);
	case 4:
		return ReadBmp2Buf4 (fd, iWidth, iHeight, pData);
	case 8:
		return ReadBmp2Buf8 (fd, iWidth, iHeight, pData);
	case 24:
		return ReadBmp2Buf24(fd, iWidth, iHeight, pData);
	}
	return true;
}



static BOOL 
ReadBmp2Buf1(
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
)
{
	return true;
}


static BOOL 
ReadBmp2Buf4(
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
)
{
	return true;
}


static BOOL 
ReadBmp2Buf8(
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
)
{
	return true;
}


static BOOL 
ReadBmp2Buf24(
	FILE_P fd, 
	int iBufWidth,
	int iBufHeight,
	void* pData
)
{
	int iOffset;
	int x,y;
	unsigned char* pSource;
	COLORREF* pDest;
	int iBmpWidth,iBmpHeight;
	int iLineSize;

	BMPINFOHEADER infoHeader;

	SEEK(fd,0x12,0);
	READ(fd,&(infoHeader.biWidth),sizeof(infoHeader.biWidth));
	READ(fd,&(infoHeader.biHeight),sizeof(infoHeader.biHeight));
	iBmpWidth=infoHeader.biWidth;
	iBmpHeight=infoHeader.biHeight;

	pSource=(unsigned char*)malloc(iBufWidth*3);
	if(!pSource)
		return false;
	pDest=(COLORREF*)pData;


	
	iLineSize=iBmpWidth*3;
	if(iLineSize%4!=0)
		iLineSize=iLineSize + (4-iLineSize%4);
	
	for(y=0;y<iBufHeight;y++){
		iOffset=0x36 + iLineSize*(iBufHeight-y-1);
		SEEK(fd,iOffset,0);
		READ(fd,pSource,iBufWidth*3);
		
		for(x=0;x<iBufWidth;x++)
			*(pDest+x)=RGB(*(pSource+3*x+2),*(pSource+3*x+1),*(pSource+3*x));
		pDest=pDest+iBufWidth;
	}
	free(pSource);
	return true;
}



