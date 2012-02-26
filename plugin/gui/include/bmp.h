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

#ifndef _BMP_H
#define _BMP_H

#ifdef __cplusplus                     
extern "C" {
#endif
//bmp file information head
typedef struct tagBMPFILEHEADER
{
	UINT16 bfType;      		//file type the value is 'BM'
	UINT32 bfSize;      		//size of the file
	UINT16 bfReserved1; 		//reserved 
	UINT16 bfReserved2; 		//reserved
	UINT32 bfOffbits;   		//from start  position of file to the start position of image data
} BMPFILEHEADER;

//bmp image information head
typedef struct tagBMPINFOHEADER
{
	UINT32 biSize;          		//size of this structure ，Windows:0x28; OS/2:0x0C
	UINT32 biWidth;			//image width（Pixel)
	UINT32 biHeight;			//image height（Pixel)
	UINT16 biPlanes;			//color planes，always 1
	UINT16 biBitCount;		//bit per point  1-->2color ；4-->16color；8-->256color；24-->true color；
	UINT32 biCompression;   	//BI_RGB/BI_RLE8/BI_RLE4;
	UINT32 biSizeImage;     	//size of image（in byte）
	UINT32 biXPelsPerMeter; 	//horizon resolution（point per meter）
	UINT32 biYPelsPerMeter; 	//vertical resolution（point per meter）
	UINT32 biClrUsed;		//color number used indeed （if 0,color number will be set by iBitCount）
	UINT32 biClrImportant;  	//coloe number important，（if 0,means all colors in pallette are important)
} BMPINFOHEADER;

BOOL 
ReadBmp2Buf(
	FILE_P fd, 
	int iWidth, 
	int iHeight, 
	void* pData
);

#ifdef __cplusplus
}
#endif 


#endif 
