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

#include "common.h"

#ifndef _GDI_H_
#define _GDI_H_

#ifdef __cplusplus                     
extern "C" {
#endif


COLORREF GUIAPI 
SetBkColor(
	HDC hDC, 
	COLORREF crColor
);


COLORREF GUIAPI 
SetTextColor(
	HDC hDC, 
	COLORREF crColor
);


void 
SetTextBkTrans();


COLORREF GUIAPI  /*inline*/  
SetPixel(
	HDC hDC, 
	int x, 
	int y, 
	COLORREF crColor
);

COLORREF /*inline*/ 
cliSetPixel(
	HDC hDC, 
	int x,
	int y, 
	COLORREF crColor
);

COLORREF /*inline*/ 
winSetPixel(
	HDC hDC, 
	int x, 
	int y, 
	COLORREF crColor
);


COLORREF /*inline*/ 
SetPixel_win(
	HDC hDC, 
	int x,
	int y, 
	COLORREF crColor
);


COLORREF /*inline*/ 
SetPixel_mem(
	HDC hDC, 
	int x,
	int y, 
	COLORREF crColor
);


COLORREF GUIAPI /*inline*/ 
GetPixel(
	HDC hDC, 
	int x, 
	int y
);


COLORREF /*inline*/ 
cliGetPixel(
	HDC hDC,
	int x,
	int y
);


COLORREF /*inline*/ 
winGetPixel(
	HDC hDC, 
	int x, 
	int y
);


COLORREF /*inline*/ 
GetPixel_win(
	HDC hDC, 
	int x, 
	int y
);


COLORREF /*inline*/ 
GetPixel_mem(
	HDC hDC, 
	int x, 
	int y
);


BOOL GUIAPI 
MoveToEx(
	HDC hDC, 
	int X, 
	int Y, 
	LPPOINT lpPoint
);


BOOL 
cliMoveToEx(
	HDC hDC,
	int x,
	int y,
	LPPOINT lpPoint
);


BOOL 
winMoveToEx(
	HDC hDC, 
	int X, 
	int Y, 
	LPPOINT lpPoint
);


BOOL GUIAPI 
LineTo(
	HDC hDC, 
	int nXEnd, 
	int nYEnd
);



BOOL /*inline*/
cliLineTo(
	HDC hDC,
	int x2,
	int y2
);



BOOL /*inline*/
winLineTo(
	HDC hDC, 
	int nXEnd, 
	int nYEnd
);



BOOL GUIAPI 
Rectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
);

BOOL /*inline*/ 
cliRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
);


BOOL /*inline*/
winRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
);


BOOL GUIAPI
FillRect(
	HDC hDC, 
	RECT *lprc
	,HBRUSH hbr
);//exclude border


BOOL /*inline*/
cliFillRect(
	HDC hDC, 
	RECT *lprc
	,HBRUSH hbr
);//exclude border


BOOL /*inline*/
winFillRect(
	HDC hDC, 
	RECT *lprc
	,HBRUSH hbr
);//exclude border


BOOL GUIAPI 
FillRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect,
	COLORREF crColor,
	COLORREF crFillColor
);


 BOOL /*inline*/
cliFillRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect,
	COLORREF crColor,
	COLORREF crFillColor
);


 BOOL /*inline*/
winFillRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect,
	COLORREF crColor,
	COLORREF crFillColor
);


BOOL GUIAPI 
Circle(
	 HDC hDC, 
	 int xCenter, 
	 int yCenter, 
	 int radius
);


void 
circleMiddlePoint(
	HDC hDC, 
	int xCenter, 
	int yCenter, 
	int radius
);


void 
circleMiddlePointFill(
	HDC hDC, 
	int xCenter, 
	int yCenter, 
	int radius
);


BOOL GUIAPI 
Ellipse(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
);


BOOL 
EllipseFill(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect, 
	int nBottomRect
);


BOOL 
EllipseBorder(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect, 
	int nBottomRect
);


BOOL GUIAPI 
Polyline(
	HDC hDC, 
	POINT *lpPoints, 
	int nCount
);

/////
#define MAXVERTICAL     1024

struct edge {
    struct edge *next;
    long yTop, yBot;
    long xNowWhole, xNowNum, xNowDen, xNowDir;
    long xNowNumStep;
};
typedef struct tagPOLYGON{
	int num;
	POINT* points;
} POLYGON;


BOOL GUIAPI 
Polygon(
	HDC hDC, 
	POINT *lpPoints,
	int nCount
);


/*inline*/ int 
polygon_GetN(
	POLYGON* p
);


/*inline*/ POINT* 
polygon_RefNth(
	POLYGON* p, 
	int i
);


/*inline*/ int 
point_GetY(
	POINT* point
);


/*inline*/ int 
point_GetX(
	POINT* point
);


/*inline*/ int 
SGN(
	int data
);


void 
FillEdges(
	POLYGON *p, 
	struct edge *edgeTable[], 
	int iMinY
);

struct edge*
UpdateActive(
	struct edge* active, 
	struct edge* edgeTable[], 
	int curY, 
	int iMinY
);


void 
DrawRuns(
	HDC hDC, 
	COLORREF crColor, 
	struct edge *active,
	int curY
);


void 
ras_FillPolygon(
	HDC hDC, 
	COLORREF crColor, 
	POLYGON *p
);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
