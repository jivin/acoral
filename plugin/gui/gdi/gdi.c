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
#include "../include/gdi.h"
    

COLORREF GUIAPI 
SetBkColor(
	HDC hDC, 
	COLORREF crColor
)
{
	if(!hDC)
		return RGB(0,0,0);
	hDC->crTextBkColor=crColor;
	hDC->isTextBkTrans = false;
	return crColor;
}


COLORREF GUIAPI 
SetTextColor(
	HDC hDC, 
	COLORREF crColor
)
{
	if(!hDC)
		return RGB(0,0,0);
	hDC->crTextColor=crColor;
	return crColor;
}


void 
SetTextBkTrans(
	HDC hDC
)
{
	if(!hDC)
		return;
	hDC->isTextBkTrans=true;
}

COLORREF GUIAPI /*inline*/
SetPixel(
	HDC hDC,
	int x,
	int y,
	COLORREF crColor
)
{
	if(hDC->iCoordType == DC_COORDTYPE_CLIENT){
		return cliSetPixel(hDC,x,y,crColor);
	}
	else{
		return winSetPixel(hDC,x,y,crColor);
	}
}
//client area coordsys
COLORREF /*inline*/ 
cliSetPixel(
	HDC hDC, 
	int x,
	int y, 
	COLORREF crColor
)
{

	RECT rc;
	GetClientRect(hDC->hWnd,&rc);
	SetRect(&rc,0,0,rc.right - rc.left,rc.bottom - rc.top);
	if(PtInRect(&rc,x,y)){
		ClientToWindow(hDC->hWnd,&x,&y);
		return winSetPixel(hDC,x,y,crColor);
	}
	return RGB(0,0,0);
}

//window area coordsys
COLORREF /*inline*/ 
winSetPixel(
	HDC hDC, 
	int x,
	int y, 
	COLORREF crColor
)
{

	if(hDC->iType==DC_TYPE_WIN)
		return SetPixel_win(hDC, x, y, crColor);
	else
		return SetPixel_mem(hDC, x, y, crColor);
}


COLORREF /*inline*/ 
SetPixel_win(
	HDC hDC, 
	int x,
	int y, 
	COLORREF crColor
)
{
	PClipRect pClipRect;

	pClipRect=((PWindowsTree)(hDC->hWnd))->pClipRgn->pHead;
	WindowToScreen(hDC->hWnd,&x,&y);
	while(pClipRect){
		if(PtInRect(&(pClipRect->rect),x,y)){
			Gal_PutPixel(x,y,crColor);
			return crColor;
		}
		pClipRect=pClipRect->pNext;
	}
	return RGB(0,0,0);
}


COLORREF /*inline*/ 
SetPixel_mem(
	HDC hDC, 
	int x,
	int y, 
	COLORREF crColor
)
{
	int iWinWidth,iWinHeight;
	int iBorder;
	COLORREF* pData;
	if(!hDC->pData)
		return RGB(0,0,0);
	iBorder = wndGetBorder(hDC->hWnd);	
	iWinWidth	=((BITMAP*)(hDC->hBitmap))->bmWidth;
	iWinHeight	=((BITMAP*)(hDC->hBitmap))->bmHeight;
	if((x<0) || (x >=iWinWidth) || (y<0) || (y>=iWinHeight))
		return RGB(0,0,0);	
	pData=(PCOLORREF)(hDC->pData);
	*(pData + y*iWinWidth + x)=crColor;
	return crColor;
}


COLORREF GUIAPI /*inline*/ 
GetPixel(
	HDC hDC, 
	int x, 
	int y
)
{
	if(hDC->iCoordType == DC_COORDTYPE_CLIENT)
		return cliGetPixel(hDC,x,y);
	else
		return winGetPixel(hDC,x,y);
}

COLORREF /*inline*/ 
cliGetPixel(
	HDC hDC,
	int x,
	int y
)
{
	ClientToWindow(hDC->hWnd,&x,&y);
	return winGetPixel(hDC,x,y);

}

COLORREF /*inline*/ 
winGetPixel(
	HDC hDC, 
	int x, 
	int y
)
{
	if(hDC->iType==DC_TYPE_WIN)
		return GetPixel_win(hDC, x, y);
	else
		return GetPixel_mem(hDC, x, y);
}


COLORREF /*inline*/ 
GetPixel_win(
	HDC hDC, 
	int x, 
	int y
)
{
	PClipRect pClipRect;
	pClipRect=((PWindowsTree)(hDC->hWnd))->pClipRgn->pHead;

	WindowToScreen(hDC->hWnd,&x,&y);

	while(pClipRect){
		if(PtInRect(&(pClipRect->rect),x,y))
			return Gal_GetPixel(x,y);
		pClipRect=pClipRect->pNext;
	}
	return RGB(0,0,0);
}


COLORREF /*inline*/ 
GetPixel_mem(
	HDC hDC, 
	int x, 
	int y
)
{
	int iWinWidth,iWinHeight;
	COLORREF* pData;
	if(!hDC->pData)
		return RGB(0,0,0);

	iWinWidth	=((BITMAP*)(hDC->hBitmap))->bmWidth;
	iWinHeight	=((BITMAP*)(hDC->hBitmap))->bmHeight;
	if((x<0) || (x >=iWinWidth) || (y<0) || (y>=iWinHeight))
		return RGB(0,0,0);		
	pData=(PCOLORREF)(hDC->pData);
	
	return *(pData + y*iWinWidth + x);
}


//client coordsys 
BOOL GUIAPI 
MoveToEx(
	HDC hDC,
	int x,
	int y,
	LPPOINT lpPoint
)
{
	if(hDC->iCoordType = DC_COORDTYPE_CLIENT)
		return cliMoveToEx(hDC,x,y,lpPoint);
	else
		return winMoveToEx(hDC,x,y,lpPoint);
	
}

BOOL 
cliMoveToEx(
	HDC hDC,
	int x,
	int y,
	LPPOINT lpPoint
)
{
	//Convert coordsys to window coordsys 	
	ClientToWindow(hDC->hWnd,&x,&y);
	//Since the coordsys of hDC->point is window coordsys
	//we must convert it to the client coordsys before return 
	WindowToClient(hDC->hWnd,&(lpPoint->x),&(lpPoint->y));
	lpPoint->x = hDC->point.x;
	lpPoint->y = hDC->point.y;
	hDC->point.x = x;
	hDC->point.y = y;
	
	return true;

}
// window coordsys
BOOL 
winMoveToEx(
	HDC hDC,
	int x,
	int y,
	LPPOINT lpPoint
)
{

	lpPoint->x=hDC->point.x;
	lpPoint->y=hDC->point.y;
	hDC->point.x=x;
	hDC->point.y=y;
	return true;
}


//client coordsys
BOOL GUIAPI 
LineTo(
	 HDC hDC,
	 int x2,
	 int y2
)
{
	if(hDC->iCoordType == DC_COORDTYPE_CLIENT)
		return cliLineTo(hDC,x2,y2);
	else
		return winLineTo(hDC,x2,y2);
}
//for we have to judeg whether this para point out range of client area
//we sould call cliSetPixel function instead winSetPixel
BOOL /*inline*/
cliLineTo(
	HDC hDC,
	int x2,
	int y2
)
{
	int winCoordx2,winCoordy2;
	float k;
	float dx,dy;
	float fx,fy;
	int x,y;
	int x1,y1;
	COLORREF crColor;
	if(!hDC)
		return false;

	x1=hDC->point.x;
	y1=hDC->point.y;
	//the point saved as current position in the hDC->point is window coordsys.
	//we must convert it to client coordsys before use it.
	WindowToClient(hDC->hWnd,&x1,&y1);
	//and we should convert the x2, y2 into window coordsys before store it into hDC->point
	winCoordx2 = x2;
	winCoordy2 = y2;
	ClientToWindow(hDC->hWnd,&winCoordx2,&winCoordy2);
	hDC->point.x = winCoordx2;
	hDC->point.y = winCoordy2;
	
	dx=(float)(x2-x1);
	dy=(float)(y2-y1);

	crColor=((PEN*)(hDC->hPen))->crPenColor;

	//a point
	if((x1==x2)&&(y1==y2)){
		cliSetPixel(hDC,x1,y1,crColor);
		return true;
	}
	if(x1==x2){//vertical line
		if(y1>y2)
			swap(y1,y2);
		for(y=y1;y<=y2;y++)
			cliSetPixel(hDC,x1,y,crColor);
		return true;
	}

	if(y1==y2){//horizon line
		if(x1>x2)
			swap(x1,x2);
		for(x=x1;x<=x2;x++)
			cliSetPixel(hDC,x,y1,crColor);
		return true;
	}

	k=dy/dx;
	if(fabs(k)<=1){// x is loop control variable
		if(x1>x2){
			swap(x1,x2);
			swap(y1,y2);
		}
		fy=(float)y1;
		for(x=x1;x<=x2;x++){
			cliSetPixel(hDC,x,(int)(fy+0.5),crColor);
			fy=fy+k;
		}
	}
	else{//y is loop control variable
		if(y1>y2){
			swap(x1,x2);
			swap(y1,y2);
		}
		fx=(float)x1;
		for(y=y1;y<=y2;y++){
			cliSetPixel(hDC,(int)(fx+0.5),y,crColor);
			fx=fx+1/k;
		}
	}
	return true;
}

//window coordsys
//we use this function in the no client area drawing function
BOOL /*inline*/
winLineTo(
	HDC hDC,
	int x2,
	int y2
)
{
	float k;
	float dx,dy;
	float fx,fy;
	int x,y;
	int x1,y1;
	COLORREF crColor;
	if(!hDC)
		return false;

	x1=hDC->point.x;
	y1=hDC->point.y;

	hDC->point.x=x2;
	hDC->point.y=y2;
	
	dx=(float)(x2-x1);
	dy=(float)(y2-y1);

	crColor=((PEN*)(hDC->hPen))->crPenColor;

	//a point
	if((x1==x2)&&(y1==y2)){
		winSetPixel(hDC,x1,y1,crColor);
		return true;
	}
	if(x1==x2){//vertical line
		if(y1>y2)
			swap(y1,y2);
		for(y=y1;y<=y2;y++)
			winSetPixel(hDC,x1,y,crColor);
		return true;
	}

	if(y1==y2){//horizon line
		if(x1>x2)
			swap(x1,x2);
		for(x=x1;x<=x2;x++)
			winSetPixel(hDC,x,y1,crColor);
		return true;
	}

	k=dy/dx;
	if(fabs(k)<=1){// x is loop control variable
		if(x1>x2){
			swap(x1,x2);
			swap(y1,y2);
		}
		fy=(float)y1;
		for(x=x1;x<=x2;x++){
			winSetPixel(hDC,x,(int)(fy+0.5),crColor);
			fy=fy+k;
		}
	}
	else{//y is loop control variable
		if(y1>y2){
			swap(x1,x2);
			swap(y1,y2);
		}
		fx=(float)x1;
		for(y=y1;y<=y2;y++){
			winSetPixel(hDC,(int)(fx+0.5),y,crColor);
			fx=fx+1/k;
		}
	}
	return true;
}


BOOL GUIAPI
Rectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
)
{
	if(hDC->iCoordType == DC_COORDTYPE_CLIENT)
		return cliRectangle(hDC,nLeftRect,nTopRect,nRightRect,nBottomRect);
	else
		return winRectangle(hDC,nLeftRect,nTopRect,nRightRect,nBottomRect);
}

//client coordsys
BOOL /*inline*/ 
cliRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
)
{
	COLORREF crBackColor;
	COLORREF crColor;
	POINT Point;
	int iCount;
	if(!hDC)
		return false;
	//backup current point position
	Point.x = hDC->point.x;
	Point.y = hDC->point.y;

	if(!IsNullBrush(hDC)){
		if(IsSolidBrush(hDC)){
			crColor = GetSolidBrushColor(hDC);
			//backup pen color of current DC
			crBackColor = ((PEN*)(hDC->hPen))->crPenColor;
			((PEN*)(hDC->hPen))->crPenColor = crColor;
			for(iCount = nTopRect+1;iCount<nBottomRect;iCount++){
				cliMoveToEx(hDC,nLeftRect+1,iCount,&Point);
				cliLineTo(hDC,nRightRect-1,iCount);
			}				
			//restore pen color of current DC
			((PPEN)(hDC->hPen))->crPenColor=crBackColor;
		}
	}
				
	cliMoveToEx(hDC,nLeftRect,nTopRect,&Point);
	cliLineTo(hDC,nLeftRect,nBottomRect);
	cliLineTo(hDC,nRightRect,nBottomRect);
	cliLineTo(hDC,nRightRect,nTopRect);
	cliLineTo(hDC,nLeftRect,nTopRect);

	//restore current point position
	hDC->point.x = Point.x;
	hDC->point.y = Point.y;
	return true;
}


//window coordsys
//we use this function in the no client area drawing function
BOOL /*inline*/
winRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect
)
{
	COLORREF crBackColor;
	COLORREF crColor;
	POINT Point;
	int iCount;
	if(!hDC)
		return false;
	//backup current point position
	Point.x = hDC->point.x;
	Point.y = hDC->point.y;

	if(!IsNullBrush(hDC)){
		if(IsSolidBrush(hDC)){
			crColor = GetSolidBrushColor(hDC);
			//backup pen color of current DC
			crBackColor = ((PEN*)(hDC->hPen))->crPenColor;
			((PEN*)(hDC->hPen))->crPenColor = crColor;
			for(iCount = nTopRect+1;iCount<nBottomRect;iCount++){
				winMoveToEx(hDC,nLeftRect+1,iCount,&Point);
				winLineTo(hDC,nRightRect-1,iCount);
			}				
			//restore pen color of current DC
			((PPEN)(hDC->hPen))->crPenColor=crBackColor;
		}
	}
				
	winMoveToEx(hDC,nLeftRect,nTopRect,&Point);
	winLineTo(hDC,nLeftRect,nBottomRect);
	winLineTo(hDC,nRightRect,nBottomRect);
	winLineTo(hDC,nRightRect,nTopRect);
	winLineTo(hDC,nLeftRect,nTopRect);

	//restore current point position
	hDC->point.x = Point.x;
	hDC->point.y = Point.y;
	return true;
}



BOOL GUIAPI
FillRect(
	HDC hDC, 
	RECT *lprc,
	HBRUSH hbr
)//exclude border
{
	if(hDC->iCoordType == DC_COORDTYPE_CLIENT)
		return	cliFillRect(hDC,lprc,hbr);
	else
		return winFillRect(hDC,lprc,hbr);

}

BOOL /*inline*/
cliFillRect(
	HDC hDC, 
	RECT *lprc,
	HBRUSH hbr
)//exclude border
{
	POINT ptOriginal,point;
	BRUSH* pBrush;
	COLORREF crColor,crBackColor;
	int i;
	if(!hDC)
		return false;
	pBrush = (BRUSH*)hbr;
	//backup pen color
	crBackColor = ((PEN*)(hDC->hPen))->crPenColor;
	crColor = pBrush->crBrushColor;
	((PEN*)(hDC->hPen))->crPenColor = crColor;
	//back original hdc point position
	ptOriginal.x = hDC->point.x;
	ptOriginal.y = hDC->point.y;

	for(i=lprc->top+1;i<lprc->bottom;i++){
		cliMoveToEx(hDC,lprc->left+1,i,&point);
		cliLineTo(hDC,lprc->right-1,i);
	}
	//restore pen color
	((PEN*)(hDC->hPen))->crPenColor = crBackColor;
	//restore original hdc point position
	hDC->point.x = ptOriginal.x;
	hDC->point.y = ptOriginal.y;
	return true;	
}


BOOL 
winFillRect(
	HDC hDC, 
	RECT *lprc,
	HBRUSH hbr
)//exclude border
{
	POINT ptOriginal,point;
	BRUSH* pBrush;
	COLORREF crColor,crBackColor;
	int i;
	if(!hDC)
		return false;
	pBrush = (BRUSH*)hbr;
	//backup pen color
	crBackColor = ((PEN*)(hDC->hPen))->crPenColor;
	crColor = pBrush->crBrushColor;
	((PEN*)(hDC->hPen))->crPenColor = crColor;
	//back original hdc point position
	ptOriginal.x = hDC->point.x;
	ptOriginal.y = hDC->point.y;

	for(i=lprc->top+1;i<lprc->bottom;i++){
		winMoveToEx(hDC,lprc->left+1,i,&point);
		winLineTo(hDC,lprc->right-1,i);
	}
	//restore pen color
	((PEN*)(hDC->hPen))->crPenColor = crBackColor;
	//restore original hdc point position
	hDC->point.x = ptOriginal.x;
	hDC->point.y = ptOriginal.y;
	return true;	
}



BOOL GUIAPI 
FillRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect,
	COLORREF crColor,
	COLORREF crFillColor
)
{
	if(hDC->iCoordType == DC_COORDTYPE_CLIENT)
		return cliFillRectangle(hDC, nLeftRect, nTopRect, nRightRect, nBottomRect, crColor, crFillColor);
	else
		return	winFillRectangle(hDC, nLeftRect, nTopRect, nRightRect, nBottomRect, crColor, crFillColor);
}


//this function only called by functions to draw no client area of a window
BOOL 
cliFillRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect,
	COLORREF crColor,
	COLORREF crFillColor
)
{
	int y;
	POINT Point;
	COLORREF crBackColor;
	if(!hDC)
		return false;
	//backup pen color
	crBackColor=((PPEN)(hDC->hPen))->crPenColor;
	((PPEN)(hDC->hPen))->crPenColor=crColor;

	cliRectangle(hDC,nLeftRect,nTopRect,nRightRect,nBottomRect);
	((PPEN)(hDC->hPen))->crPenColor=crFillColor;
	for(y=nTopRect+1;y<nBottomRect;y++){
		cliMoveToEx(hDC,nLeftRect+1,y,&Point);
		cliLineTo(hDC,nRightRect-1,y);
	}
	//restore pen color
	((PPEN)(hDC->hPen))->crPenColor=crBackColor;
	return true;
}



//this function only called by functions to draw no client area of a window
BOOL 
winFillRectangle(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect,
	int nBottomRect,
	COLORREF crColor,
	COLORREF crFillColor
)
{
	int y;
	POINT Point;
	COLORREF crBackColor;
	if(!hDC)
		return false;
	//backup pen color
	crBackColor=((PPEN)(hDC->hPen))->crPenColor;
	((PPEN)(hDC->hPen))->crPenColor=crColor;

	winRectangle(hDC,nLeftRect,nTopRect,nRightRect,nBottomRect);
	((PPEN)(hDC->hPen))->crPenColor=crFillColor;
	for(y=nTopRect+1;y<nBottomRect;y++){
		winMoveToEx(hDC,nLeftRect+1,y,&Point);
		winLineTo(hDC,nRightRect-1,y);
	}
	//restore pen color
	((PPEN)(hDC->hPen))->crPenColor=crBackColor;
	return true;
}



BOOL GUIAPI 
Circle(
	 HDC hDC, 
	 int xCenter, 
	 int yCenter, 
	 int radius
)
{
	POINT ptOriginal;
	COLORREF crColor;
	COLORREF crBackColor;

	if(!hDC)
		return false;
	
	ptOriginal.x = hDC->point.x;
	ptOriginal.y = hDC->point.y;	
	
	if(!IsNullBrush(hDC)){
		if(IsSolidBrush(hDC)){
			crColor = GetSolidBrushColor(hDC);
			crBackColor = ((PEN*)(hDC->hPen))->crPenColor;
			((PEN*)(hDC->hPen))->crPenColor = crColor;
			circleMiddlePointFill(hDC,xCenter, yCenter,radius);
			((PEN*)(hDC->hPen))->crPenColor = crBackColor;
		}
	}

	circleMiddlePoint(hDC,xCenter,yCenter, radius);

	hDC->point.x = ptOriginal.x;
	hDC->point.y = ptOriginal.y;

	return true;
}



void 
circleMiddlePoint(
	HDC hDC, 
	int xCenter, 
	int yCenter, 
	int radius
)
{
	COLORREF color;
	int x =0;
	int y = radius;
	int p = 1-radius;

	color = ((PEN*)(hDC->hPen))->crPenColor;
	
	SetPixel(hDC,xCenter+x,yCenter+y,color);
	SetPixel(hDC,xCenter-x,yCenter+y,color);
	SetPixel(hDC,xCenter+x,yCenter-y,color);
	SetPixel(hDC,xCenter-x,yCenter-y,color);
	
	SetPixel(hDC,xCenter+y,yCenter+x,color);
	SetPixel(hDC,xCenter-y,yCenter+x,color);
	SetPixel(hDC,xCenter+y,yCenter-x,color);
	SetPixel(hDC,xCenter-y,yCenter-x,color);

	while(x<y){
		x++;
		if(p<0)
			p+=2*x+1;
		else{
			y--;
			p+=2*(x-y)+1;
		}
		SetPixel(hDC,xCenter+x,yCenter+y,color);
		SetPixel(hDC,xCenter-x,yCenter+y,color);
		SetPixel(hDC,xCenter+x,yCenter-y,color);
		SetPixel(hDC,xCenter-x,yCenter-y,color);

		SetPixel(hDC,xCenter+y,yCenter+x,color);
		SetPixel(hDC,xCenter-y,yCenter+x,color);
		SetPixel(hDC,xCenter+y,yCenter-x,color);
		SetPixel(hDC,xCenter-y,yCenter-x,color);
	}
}



void 
circleMiddlePointFill(
	HDC hDC, 
	int xCenter, 
	int yCenter, 
	int radius
)
{
	POINT point;
	COLORREF color;
	int x =0;
	int y = radius;
	int p = 1-radius;

	color = ((PEN*)(hDC->hPen))->crPenColor;

	MoveToEx(hDC,xCenter-y,yCenter,&point);
	LineTo(hDC,xCenter+y,yCenter);

	while(x<y){
		x++;
		if(p<0)
			p+=2*x+1;
		else{
			y--;
			p+=2*(x-y)+1;
		}
		MoveToEx(hDC,xCenter+x,yCenter+y,&point);
		LineTo(hDC,xCenter-x,yCenter+y);
		MoveToEx(hDC,xCenter+x,yCenter-y,&point);
		LineTo(hDC,xCenter-x,yCenter-y);
		MoveToEx(hDC,xCenter+y,yCenter+x,&point);
		LineTo(hDC,xCenter-y,yCenter+x);

		MoveToEx(hDC,xCenter+y,yCenter-x,&point);
		LineTo(hDC,xCenter-y,yCenter-x);
	}
}


BOOL GUIAPI
Ellipse(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect, 
	int nBottomRect
)
{
	POINT ptOriginal;
	COLORREF crColor;
	COLORREF crBackColor;

	ClientToWindow(hDC->hWnd,&nLeftRect,&nTopRect);
	ClientToWindow(hDC->hWnd,&nRightRect,&nBottomRect);
	
	//backup original current point;
	ptOriginal.x = hDC->point.x;
	ptOriginal.y = hDC->point.y;
	if(!IsNullBrush(hDC)){
		if(IsSolidBrush){
			crBackColor = ((PPEN)(hDC->hPen))->crPenColor;
			crColor = GetSolidBrushColor(hDC);
			((PPEN)(hDC->hPen))->crPenColor = crColor;
		
			EllipseFill(hDC,nLeftRect,nTopRect,nRightRect,nBottomRect);
			((PPEN)(hDC->hPen))->crPenColor = crBackColor;			
		}
	}
	EllipseBorder(hDC,nLeftRect,nTopRect,nRightRect,nBottomRect);
	
	//restore current point of dc
	hDC->point.x = ptOriginal.x;
	hDC->point.y = ptOriginal.y;
	return true;
}


BOOL 
EllipseBorder(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect, 
	int nBottomRect
)
{
	int x0,y0,a,b;
	int x,y;
	float d1,d2;

	COLORREF crColor;
	if(!hDC)
		return false;

	crColor=((PEN*)(hDC->hPen))->crPenColor;

	x0=nLeftRect+(nRightRect-nLeftRect)/2;
	y0=nTopRect+(nBottomRect-nTopRect)/2;
	a=nRightRect-x0;
	b=nBottomRect-y0;


	x=0;
	y=b;
	d1=(float)(b*b+a*a*(0.25-b));
	SetPixel(hDC,x0+x,y0+y,crColor);
	SetPixel(hDC,x0+x,y0-y,crColor);
	SetPixel(hDC,x0-x,y0-y,crColor);
	SetPixel(hDC,x0-x,y0+y,crColor);

	while(b*b*(x+1)<a*a*(y-0.5))
	{
		if(d1<0){
			d1+=b*b*(2*x+3);
			x++;
		}
		else{
			d1+=(b*b*(2*x+3)+a*a*(2-2*y));
			x++;
			y--;
		}
		SetPixel(hDC,x0+x,y0+y,crColor);
		SetPixel(hDC,x0+x,y0-y,crColor);
		SetPixel(hDC,x0-x,y0-y,crColor);
		SetPixel(hDC,x0-x,y0+y,crColor);

	}
	d2=(float)(sqrt(b*(x+0.5))+a*(y-1)-a*b);
	while(y>0){
		if(d2<0){
			d2+=b*b*(2*x+2)+a*a*(3-2*y);
			x++;
			y--;
		}
		else{
			d2+=a*a*(3-2*y);
			y--;
		}
		SetPixel(hDC,x0+x,y0+y,crColor);
		SetPixel(hDC,x0+x,y0-y,crColor);
		SetPixel(hDC,x0-x,y0-y,crColor);
		SetPixel(hDC,x0-x,y0+y,crColor);
	}
	return true;
}


BOOL 
EllipseFill(
	HDC hDC,
	int nLeftRect,
	int nTopRect,
	int nRightRect, 
	int nBottomRect
)
{
	int x0,y0,a,b;
	int x,y;
	float d1,d2;
	POINT point;


	if(!hDC)
		return false;


	x0=nLeftRect+(nRightRect-nLeftRect)/2;
	y0=nTopRect+(nBottomRect-nTopRect)/2;
	a=nRightRect-x0;
	b=nBottomRect-y0;


	x=0;
	y=b;
	d1=(float)(b*b+a*a*(0.25-b));
	MoveToEx(hDC,x0+x,y0+y,&point);
	LineTo(hDC,x0+x,y0-y);
	MoveToEx(hDC,x0-x,y0-y,&point);
	LineTo(hDC,x0-x,y0+y);

	while(b*b*(x+1)<a*a*(y-0.5))
	{
		if(d1<0){
			d1+=b*b*(2*x+3);
			x++;
		}
		else{
			d1+=(b*b*(2*x+3)+a*a*(2-2*y));
			x++;
			y--;
		}
		MoveToEx(hDC,x0+x,y0+y,&point);
		LineTo(hDC,x0+x,y0-y);
		MoveToEx(hDC,x0-x,y0-y,&point);
		LineTo(hDC,x0-x,y0+y);		
	}
	d2=(float)(sqrt(b*(x+0.5))+a*(y-1)-a*b);
	while(y>0){
		if(d2<0){
			d2+=b*b*(2*x+2)+a*a*(3-2*y);
			x++;
			y--;
		}
		else{
			d2+=a*a*(3-2*y);
			y--;
		}
		MoveToEx(hDC,x0+x,y0+y,&point);
		LineTo(hDC,x0+x,y0-y);
		MoveToEx(hDC,x0-x,y0-y,&point);
		LineTo(hDC,x0-x,y0+y);
	}

	return true;
}


/*----------------  
	Polyline
-----------------*/
BOOL GUIAPI
Polyline(
	HDC hDC, 
	POINT *lpPoints, 
	int nCount
)
{
	POINT tmpPoint;
	POINT point;
	int i;
	if(!hDC)
		return false;
	if(nCount<3)
		return false;

	//backup current point
	point.x = hDC->point.x;
	point.y = hDC->point.y;

	MoveToEx(hDC,lpPoints[0].x,lpPoints[0].y,&tmpPoint);
	for(i=1;i<nCount-1;i++){
		//MoveToEx(hDC,lpPoints[i].x,lpPoints[i].y,&tmpPoint);
		LineTo(hDC,lpPoints[i].x,lpPoints[i].y);
	}
	//restore current point 
	hDC->point.x = point.x;
	hDC->point.y = point.y;
	return true;
}



/*----------------  
	Polygon 
-----------------*/
BOOL GUIAPI
Polygon(
	HDC hDC, 
	POINT *lpPoints,
	int nCount
)
{
	POINT tmpPoint;
	POINT point;
	int i;
	POLYGON p;
	COLORREF crColor;
	COLORREF crBackColor;

	if(nCount<3)
		return false;
	if(!hDC)
		return false;
	
	//backup current point position
	point.x = hDC->point.x;
	point.y = hDC->point.y;

	if(!IsNullBrush(hDC)){
		if(IsSolidBrush(hDC)){
			p.num = nCount;
			p.points = lpPoints;
			//get brush color
			crColor = GetSolidBrushColor(hDC);
			//backup old pen color
			crBackColor = ((PPEN)(hDC->hPen))->crPenColor;
			((PPEN)(hDC->hPen))->crPenColor = crColor;
			ras_FillPolygon(hDC, crColor, &p);
			//restore old pen color
			((PPEN)(hDC->hPen))->crPenColor = crBackColor;
		}
	}


	MoveToEx(hDC,lpPoints[0].x,lpPoints[0].y,&tmpPoint);
	for(i=1;i<nCount;i++){
		LineTo(hDC,lpPoints[i].x,lpPoints[i].y);
	}
	
	LineTo(hDC,lpPoints[0].x,lpPoints[0].y);
	
	//restore point position of dc
	hDC->point.x = point.x;
	hDC->point.y = point.y;


	return true;
}
 


/*inline*/ int 
polygon_GetN(
	POLYGON* p
)
{
	return p->num;
}


/*inline*/ POINT* 
polygon_RefNth(
	POLYGON* p, 
	int i
)
{
	return &(p->points[i]);
}


/*inline*/ int 
point_GetY(
	POINT* point
)
{
	return point->y;
}


/*inline*/ int 
point_GetX(
	POINT* point
)
{
	return point->x;
}


/*inline*/ int 
SGN(
	int data
)
{
	if(data > 0)
		return 1;
	if(data < 0)
		return -1;
	if(data == 0)
		return 0;
	return 0;
}


/*inline*/ int 
GetMinY(
	POLYGON* p
)
{
	int i;
	int iTotal;
	int iMinY;
	iTotal = p->num;
	iMinY = p->points[0].y;
	for(i=1;i<iTotal;i++){
		if(p->points[i].y < iMinY)
			iMinY = p->points[i].y;
	}
	if(iMinY < 0)
		return iMinY;
	else
		return 0;
}


void 
ras_FillPolygon(
	HDC hDC, 
	COLORREF crColor, 
	POLYGON *p
)
{
	static struct edge* edgeTable[MAXVERTICAL];
	struct edge *active;
	int curY;
	int iMinY;
	
	iMinY = GetMinY(p);
	FillEdges(p, edgeTable,iMinY);

	for (curY = 0; edgeTable[curY] == NULL; curY++)
		if (curY == MAXVERTICAL - 1)
			return;     /* No edges in polygon */

	for (active = NULL; (active = UpdateActive(active, edgeTable, curY, iMinY)) != NULL; curY++)
		DrawRuns(hDC, crColor, active, curY + iMinY);
}


void 
FillEdges(
	POLYGON *p, 
	struct edge *edgeTable[], 
	int iMinY
)
{
	int i, j, n = polygon_GetN(p);

	for (i = 0; i < MAXVERTICAL; i++)
		edgeTable[i] = NULL;

	for (i = 0; i < n; i++) {
		POINT *p1, *p2, *p3;
		struct edge *e;
		p1 = polygon_RefNth(p, i);
		p2 = polygon_RefNth(p, (i + 1) % n);
		if (point_GetY(p1) == point_GetY(p2))
			continue;   /* Skip horiz. edges */
		/* Find next vertex not level with p2 */
		for (j = (i + 2) % n; ; j = (j + 1) % n) {
			p3 = polygon_RefNth(p, j);
			if (point_GetY(p2) != point_GetY(p3))
				break;
		}
		e = (struct edge*)malloc(sizeof(struct edge));

		//e = New(struct edge);

		e->xNowNumStep = abs(point_GetX(p1) - point_GetX(p2));
		if (point_GetY(p2) > point_GetY(p1)) {
			e->yTop = point_GetY(p1);
			e->yBot = point_GetY(p2);
			e->xNowWhole = point_GetX(p1);
			e->xNowDir = SGN(point_GetX(p2) - point_GetX(p1));
			e->xNowDen = e->yBot - e->yTop;
			e->xNowNum = (e->xNowDen >> 1);
			if (point_GetY(p3) > point_GetY(p2))
				e->yBot--;
		}
		 else {
			e->yTop = point_GetY(p2);
			e->yBot = point_GetY(p1);
			e->xNowWhole = point_GetX(p2);
			e->xNowDir = SGN(point_GetX(p1) - point_GetX(p2));
			e->xNowDen = e->yBot - e->yTop;
			e->xNowNum = (e->xNowDen >> 1);
			if (point_GetY(p3) < point_GetY(p2)) {
				e->yTop++;
				e->xNowNum += e->xNowNumStep;
				while (e->xNowNum >= e->xNowDen) {
				e->xNowWhole += e->xNowDir;
				e->xNowNum -= e->xNowDen;
				}
			}
		}
		e->next = edgeTable[e->yTop - iMinY];
		edgeTable[e->yTop - iMinY] = e;
	}
}


struct edge*
UpdateActive(
	struct edge* active, 
	struct edge* edgeTable[], 
	int curY, 
	int iMinY
)
{
	struct edge *e, **ep;
	for (ep = &active, e = *ep; e != NULL; e = *ep)
		if (e->yBot < curY+ iMinY) {
			*ep = e->next;
			free(e);
		}
		 else
			ep = &e->next;
	*ep = edgeTable[curY];
	return active;
}


void 
DrawRuns(
	HDC hDC, 
	COLORREF crColor, 
	struct edge* active,
	int curY
)
{
	struct edge *e;
	static int xCoords[100];
	int numCoords = 0;
	int i, x;
	for (e = active; e != NULL; e = e->next) {
		for (i = numCoords; i > 0 &&
		xCoords[i - 1] > e->xNowWhole; i--)
		xCoords[i] = xCoords[i - 1];
		xCoords[i] = e->xNowWhole;
		numCoords++;
		e->xNowNum += e->xNowNumStep;
		while (e->xNowNum >= e->xNowDen) {
		e->xNowWhole += e->xNowDir;
		e->xNowNum -= e->xNowDen;
		}
	}
	if (numCoords % 2)  /* Protect from degenerate polygons */
		xCoords[numCoords] = xCoords[numCoords - 1],
		numCoords++;
	for (i = 0; i < numCoords; i += 2) {
		for (x = xCoords[i] + 1; x <xCoords[i + 1]; x++)
			SetPixel(hDC,x,curY,crColor);
	}
}



