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

#ifndef _HDC_H_
#define _HDC_H_

#ifdef __cplusplus                     
extern "C" {
#endif

#define SYS_DEFAULT_FONT	FONT_14_14//default font

//#define GDI_SYS				8 /* pre-defined GDI object  */
//#define GDI_USR				0  /* user defined GDI object */


/* Pen Styles */
#define PS_NULL             0x00000010
#define PS_SOLID            0x00000011
#define PS_DASH             0x00000012	/* -------  */
#define PS_DOT              0x00000013	/* .......  */
#define PS_DASHDOT          0x00000014	/* _._._._  */
#define PS_DASHDOTDOT       0x00000015	/* _.._.._  */

/* Brush Styles */
#define BS_NULL				0x00000020
#define BS_SOLID			0x00000021
#define BS_HATCHED			0x00000022
#define BS_AGAINST			0x00000023


/* Hatch Styles */
#define HS_HORIZONTAL       0	/* ----- */
#define HS_VERTICAL         1	/* ||||| */
#define HS_FDIAGONAL        2	/* \\\\\ */
#define HS_BDIAGONAL        3	/* ///// */
#define HS_CROSS            4	/* +++++ */
#define HS_DIAGCROSS        5	/* xxxxx */


//System Pre-defined GDI

#define WHITE_PEN			0x00000000
#define LTGRAY_PEN			0x00000001
#define GRAY_PEN			0x00000002
#define DKGRAY_PEN			0x00000003
#define BLACK_PEN			0x00000004
#define NULL_PEN			0x00000005

#define WHITE_BRUSH			0x00000006
#define LTGRAY_BRUSH		0x00000007
#define GRAY_BRUSH			0x00000008
#define DKGRAY_BRUSH		0x00000009
#define BLACK_BRUSH			0x0000000A
#define NULL_BRUSH			0x0000000B

#define FONT_16_16			0x0000000C
#define FONT_14_14			0x0000000D
#define FONT_12_12			0x0000000E

//GDI TYPE
#define GDI_PEN				0x00000000
#define GDI_BRUSH			0x00000001
#define GDI_FONT			0x00000002
#define GDI_BITMAP			0x00000003
#define GDI_RES				0x00000004




//字库结构
typedef struct tagFONTLIBHEADER{
	int iSize;			//size of this struction
	int iAscWidth;		//width of Ascii character 
	int iAscHeight;		//height of Ascii character
	int iAscBytes;		//bytes of a Ascii character used
	int iChnWidth;		//width of chinese character
	int iChnHeight;		//height of chinese character
	int iChnBytes;		//bytes of a chinese character used 
	int iAscOffset;		//offset address of Ascii character(from the end of this structure)
	int iChnSymOffset;	//offset address of chinese symbol (from the end of this structure)
	int iChnOffset;		//offset address of chinese character(from the end of this structure)
} FONTLIBHEADER;
typedef FONTLIBHEADER* PFONTLIBHEADER;



typedef unsigned int GDITYPE;


typedef struct tagPEN{
	GDITYPE		iGdiType;
	int			iPenStyle;		// pen style
	int			iPenWidth;		// pen width
	COLORREF	crPenColor;		// pen color
} PEN;
typedef PEN* PPEN;

typedef struct tagBRUSH{
	GDITYPE		iGdiType;
	int			iBrushStyle;	// brush style
	int			iHatchStyle;	// hatch style
	COLORREF	crBrushColor;	// color value
} BRUSH;
typedef BRUSH* PBRUSH;

typedef struct tagFONT{
	GDITYPE		iGdiType;
	int iFontStyle;				//font type
	int iOffset;				//offset address in the shared memory
	FONTLIBHEADER FontLibHeader;//font library head structure 
} FONT;
typedef FONT* PFONT;

typedef struct tagBITMAP { 
	GDITYPE		iGdiType;		//
	LONG		bmType;		//Specifies the bitmap type. This member must be zero
	LONG		bmWidth;		//Specifies the width, in pixels, of the bitmap
	LONG		bmHeight;		//Specifies the height, in pixels, of the bitmap
	LONG		bmWidthBytes;	//Specifies the number of bytes in each scan line. This value must be divisible by 2
	WORD		bmPlanes;		//Specifies the count of color planes
	WORD		bmBitsPixel;	//Specifies the number of bits required to indicate the color of a pixel
	LPVOID		bmBits;			//Pointer to the location of the bit values for the bitmap
} BITMAP; 
typedef BITMAP* PBITMAP; 


#define DC_TYPE_MEM		0x00000000	//device context type: memory
#define DC_TYPE_WIN		0x00000001	//device context type: window

#define DC_COORDTYPE_WINDOW	0x00000000		//device context coord type: window
#define DC_COORDTYPE_CLIENT	0x00000001		//device contect coord type: client
typedef struct tagDC{
	int				iType;			//type(window/memory)
	int				iCoordType;		//CoordType(Window/Client);
	COLORREF		crTextBkColor;	//text background color
	COLORREF		crTextColor;	//text color
	BOOL			isTextBkTrans;	//the text is transparent ?
	POINT			point;			//current point 

	HPEN			hPen;
	HBRUSH			hBrush;
	HFONT			hFont;
	HBITMAP			hBitmap;
	void*			pData;			//memory device context only 
	HWND			hWnd;			//handle of window
} DC;
typedef DC* HDC;


HDC	GUIAPI 	
GetDC(
	 HWND hWnd
);

HDC GUIAPI
GetWindowDC(
	HWND hWnd
);

int	GUIAPI 	
ReleaseDC(
	HWND hWnd, 
	HDC hDC
);


HDC	GUIAPI
CreateCompatibleDC(
	HDC hDCRefer
);


BOOL GUIAPI 
CreateStockObject();


HGDIOBJ	GUIAPI 
GetStockObject(
	int iStyle
);


HPEN GUIAPI 
CreatePen(
	int iPenStyle, 
	int iWidth, 
	COLORREF crColor
);


HBRUSH GUIAPI 
CreateSolidBrush(
	COLORREF crColor
);


HBRUSH GUIAPI 
CreateHatchBrush(
	int iStyle, 
	COLORREF clrref
);


HBRUSH GUIAPI 
CreateAgainstBrush();


HBITMAP GUIAPI 
CreateBitmap(
	char* pFile
);

BOOL GUIAPI 
SelectObject(
	HDC hDC,
	HGDIOBJ hGdiObj
);


void GUIAPI 
DeleteObject(
	HGDIOBJ  hObject
);


BOOL 
IsNullBrush(
	HDC hDC
);


BOOL 
IsSolidBrush(
	HDC hDC
);


COLORREF 
GetSolidBrushColor(
	HDC hDC
);


void 
PutSavedBoxOnDC(
	HDC hDC,
	int x, 
	int y, 
	int nWidth, 
	int nHeight, 
	PCOLORREF pSavedBox
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 
