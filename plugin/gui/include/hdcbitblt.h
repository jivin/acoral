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

#ifndef _HDCBITBLT_H_
#define _HDCBITBLT_H_

#ifdef __cplusplus                     
extern "C" {
#endif



#define SRCCOPY				0x00000001//	Copies the source rectangle directly to the destination rectangle
#define SRCAND				0x00000002//	Combines the colors of the source and destination rectangles by using the Boolean AND operator.
#define SRCPAINT			0x00000003//	Combines the colors of the source and destination rectangles by using the Boolean OR operator.
#define SRCINVERT			0x00000004//	Combines the colors of the source and destination rectangles by using the Boolean XOR operator.
#define SRCTRANS			0x00000005//	copies the source rectangle exclude sys trans color(only from mem to scr)



BOOL winBitBlt(
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
);

BOOL GUIAPI BitBlt(
  HDC hdcDest, // handle to destination device context
  int nXDest,  // x-coordinate of destination rectangle's upper-left
               // corner
  int nYDest,  // y-coordinate of destination rectangle's upper-left
               // corner
  int nWidth,  // width of destination rectangle
  int nHeight, // height of destination rectangle
  HDC hdcSrc,  // handle to source device context
  int nXSrc,   // x-coordinate of source rectangle's upper-left
               // corner
  int nYSrc,   // y-coordinate of source rectangle's upper-left
               // corner
  DWORD dwRop  // raster operation code
);

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
);


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
);


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
);


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
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 
