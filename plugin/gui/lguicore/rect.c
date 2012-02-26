/*
	Copyright (C) 2004-2005 Li Yudong
	Some ideas come from MiniGUI
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
#include "../include/rect.h"

BOOL GUIAPI
SetRect(
	LPRECT lprc, // pointer to structure with rectangle to set
	int xLeft,   // left side
	int yTop,    // top side
	int xRight,  // right side
	int yBottom  // bottom side
)
{
	if(!lprc)
		return false;
	lprc->left		= xLeft;
	lprc->top		= yTop;
	lprc->right		= xRight;
	lprc->bottom	= yBottom;
	return true;
}


BOOL GUIAPI
SetRectEmpty(
	LPRECT lprc   // pointer to structure with rectangle set to empty
)
{
	if(!lprc)
		return false;
	memset(lprc,0,sizeof(RECT));
	return true;
}
 
BOOL GUIAPI
IsRectEmpty(
	const RECT *lprc   // pointer to structure with rectangle
)
{
	if((lprc->left==0) && ( lprc->right==0) 
		&& (lprc->top==0) && (lprc->bottom==0))
		return true;
	else
		return false;
}

BOOL GUIAPI
CopyRect(
	LPRECT lprcDst,      // pointer to structure for destination rectangle
	const RECT *lprcSrc  // pointer to structure for source rectangle
)
{
	if(!lprcDst)
		return false;
	if(!lprcSrc)
		return false;
	memcpy(lprcDst,lprcSrc,sizeof(RECT));
	return true;
}


BOOL GUIAPI
EqualRect(
	const RECT *lprc1,  // pointer to structure with first rectangle
	const RECT *lprc2   // pointer to structure with second rectangle
)
{
	if(!lprc1)
		return false;
	if(!lprc2)
		return false;
    if(lprc1->left		!= lprc2->left)		return false;
    if(lprc1->top		!= lprc2->top)		return false;
    if(lprc1->right		!= lprc2->right)	return false;
    if(lprc1->bottom	!= lprc2->bottom)	return false;

    return true;

}

BOOL GUIAPI
NormalizeRect(
	LPRECT lprc
)
{
	if(!lprc)
		return false;
	if(lprc->left > lprc->right)
		swap(lprc->left,lprc->right);
	if(lprc->top > lprc->bottom)
		swap(lprc->top,lprc->bottom);

	return true;
}

BOOL GUIAPI
IntersectRect(
	LPRECT lprcDst,        // pointer to structure for intersection
	const RECT *lprcSrc1,  // pointer to structure with first rectangle
	const RECT *lprcSrc2   // pointer to structure with second rectangle
)
{
	if(!lprcDst)
		return false;
	if(!lprcSrc1)
		return false;
	if(!lprcSrc2)
		return false;
	lprcDst->left	= max(lprcSrc1->left, lprcSrc2->left);
	lprcDst->top	= max(lprcSrc1->top, lprcSrc2->top);
	lprcDst->right	= min(lprcSrc1->right, lprcSrc2->right);
	lprcDst->bottom = min(lprcSrc1->bottom, lprcSrc2->bottom);

	if(lprcDst->left > lprcDst->right)
		return false;
	if(lprcDst->top > lprcDst->bottom)
		return false;

	return true;

}



BOOL GUIAPI
IsIntersect(
	const RECT *lprc1,  // pointer to structure with first rectangle
	const RECT *lprc2   // pointer to structure with second rectangle
)
{
    int left, top, right, bottom;

	if(!lprc1)
		return false;
	if(!lprc2)
		return false;
	left	= max(lprc1->left, lprc2->left);
	top		= max(lprc1->top, lprc2->top);
	right	= min(lprc1->right, lprc2->right);
	bottom	= min(lprc1->bottom, lprc2->bottom);

    if((left > right) || (top > bottom))
        return false;

    return true;
	
}




BOOL GUIAPI
OffsetRect(
	LPRECT lprc,  // pointer to structure with rectangle
	int dx,       // horizontal offset
	int dy        // vertical offset
)
{
	if(!lprc)
		return false;
	lprc->left += dx;
	lprc->top += dy;
	lprc->right += dx;
	lprc->bottom += dy;
	return true;
}
 

BOOL GUIAPI
InflateRect(
	LPRECT lprc,  // pointer to rectangle
	int dx,       // amount to increase or decrease width
	int dy        // amount to increase or decrease height
)
{
	if(!lprc)
		return false;
	lprc->left -= dx;
	lprc->top -= dy;
	lprc->right += dx;
	lprc->bottom += dy;

	return true;
}
 

BOOL GUIAPI
PtInRect(
	const RECT *lprc,  // pointer to structure with rectangle
	int x,
	int y
)
{
	if (   x >= lprc->left 
        && x <= lprc->right 
        && y >= lprc->top 
        && y <= lprc->bottom) 

        return TRUE;
    else
		return FALSE;
}
 


